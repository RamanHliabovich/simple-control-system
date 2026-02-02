#include "unity_server.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <ws2tcpip.h>

using json = nlohmann::json;

unity_server::unity_server(int port)
    : is_running(false), 
      listening_socket(INVALID_SOCKET), 
      client_socket(INVALID_SOCKET),
      client_connected(false),
      port(port)
{
    // Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cerr << "Unity Server: WSAStartup failed\n";
    }
}

unity_server::~unity_server()
{
    stop();
    WSACleanup();
}

bool unity_server::start()
{
    is_running = true;
    server_thread = std::thread(&unity_server::run_server, this);
    std::cout << "Unity visualization server starting on port " << port << "...\n";
    return true;
}

void unity_server::stop()
{
    is_running = false;
    client_connected = false;
    
    // Close client socket
    if (client_socket != INVALID_SOCKET)
    {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }
    
    // Close listening socket
    if (listening_socket != INVALID_SOCKET)
    {
        closesocket(listening_socket);
        listening_socket = INVALID_SOCKET;
    }
    
    // Join threads
    if (client_thread.joinable())
    {
        client_thread.join();
    }
    
    if (server_thread.joinable())
    {
        server_thread.join();
    }
    
    std::cout << "Unity server stopped\n";
}

void unity_server::run_server()
{
    listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (listening_socket == INVALID_SOCKET)
    {
        std::cerr << "Unity server: Failed to create socket\n";
        return;
    }
    
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));
    server_addr.sin_port = htons(port);
    
    if (bind(listening_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Unity server: Bind failed with error: " << WSAGetLastError() << "\n";
        return;
    }
    
    if (listen(listening_socket, 1) == SOCKET_ERROR)
    {
        std::cerr << "Unity server: Listen failed\n";
        return;
    }
    
    std::cout << "Unity server listening on port " << port << "...\n";
    std::cout << "Waiting for Unity client to connect...\n";
    
    while (is_running)
    {
        // Accept only one client
        client_socket = accept(listening_socket, nullptr, nullptr);
        
        if (client_socket != INVALID_SOCKET)
        {
            std::cout << "Unity client connected\n";
            client_connected = true;
            
            // Handle this client (blocks until client disconnects)
            handle_unity_client();
            
            // Client disconnected
            client_connected = false;
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            std::cout << "Unity client disconnected, waiting for reconnection...\n";
        }
    }
}

void unity_server::handle_unity_client()
{
    char buffer[1024];
    
    while (client_connected && is_running)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0)
        {
            std::string message(buffer, bytes_received);
            std::cout << "Unity client message: " << message << std::endl;
            
            try
            {
                // Parse JSON message from Unity
                auto json_msg = json::parse(message);
                
                // Unity sends inbit confirmation
                if (json_msg.contains("inbit") && json_msg.contains("conveyor"))
                {
                    std::string conveyor_id = json_msg["conveyor"];
                    bool inbit_state = json_msg["inbit"];
                    
                    std::cout << "Received inbit from Unity - Conveyor: " << conveyor_id 
                              << ", Inbit: " << (inbit_state ? "ON" : "OFF") << "\n";
                    
                    // Trigger callback if set
                    if (on_inbit_received)
                    {
                        on_inbit_received(conveyor_id, inbit_state);
                    }
                }
            }
            catch (const json::exception& e)
            {
                std::cerr << "Unity server: Failed to parse JSON: " << e.what() << "\n";
            }
        }
        else if (bytes_received == 0)
        {
            std::cout << "Unity client disconnected gracefully\n";
            break;
        }
        else
        {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK && error != 0)
            {
                std::cerr << "Unity client recv error: " << error << std::endl;
                break;
            }
        }
    }
}

void unity_server::send_driver_outbit(const std::string& conveyor_id, bool outbit_state)
{
    if (!client_connected || client_socket == INVALID_SOCKET)
    {
        std::cerr << "Cannot send outbit: Unity client not connected\n";
        return;
    }
    
    // Create JSON message with driver outbit status
    json outbit_message = {
        {"type", "outbit"},
        {"conveyor", conveyor_id},
        {"outbit", outbit_state},
        {"timestamp", std::time(nullptr)}
    };
    
    std::string message = outbit_message.dump();
    
    int result = send(client_socket, message.c_str(), static_cast<int>(message.length()), 0);
    
    if (result != SOCKET_ERROR)
    {
        std::cout << "Sent outbit (" << (outbit_state ? "ON" : "OFF") 
                  << ") to Unity client for conveyor: " << conveyor_id << "\n";
    }
    else
    {
        std::cerr << "Failed to send outbit to Unity client. Error: " << WSAGetLastError() << "\n";
        client_connected = false;
    }
}