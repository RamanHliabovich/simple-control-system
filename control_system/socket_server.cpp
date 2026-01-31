#include "socket_server.h"
#include "conveyor_subsystem.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <ws2tcpip.h>

using json = nlohmann::json;

socket_server::socket_server(conveyor_subsystem* subsystem, int port)
    : subsystem(subsystem), is_running(false), listening_socket(INVALID_SOCKET)
{
    // Initialize Winsock
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed.\n";
        // Optionally, handle the error (e.g., throw, set a flag, or return)
        // For example:
        is_running = false;
        return;
    }
}

socket_server::~socket_server()
{
    stop();
    WSACleanup();
}

bool socket_server::start()
{
    is_running = true;
    server_thread = std::thread(&socket_server::run_server, this);
    return true;
}

void socket_server::stop()
{
    is_running = false;
    if (server_thread.joinable())
    {
        server_thread.join();
    }
    if (listening_socket != INVALID_SOCKET)
    {
        closesocket(listening_socket);
        listening_socket = INVALID_SOCKET;
    }
}

void socket_server::run_server()
{
    listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr));
    server_addr.sin_port = htons(5000);
    bind(listening_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(listening_socket, SOMAXCONN);
    
    std::cout << "Socket server listening on port 5000...\n";
    
    while (is_running)
    {
        SOCKET client_socket = accept(listening_socket, nullptr, nullptr);
        if (client_socket != INVALID_SOCKET)
        {
            // Handle client communication
            char buffer[1024] = {};
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            
            if (bytes_received > 0)
            {
                std::string response = process_json_command(std::string(buffer));
                send(client_socket, response.c_str(), static_cast<int>(response.length()), 0);
            }
            
            closesocket(client_socket);
        }
    }
}

std::string socket_server::process_json_command(const std::string& json_command)
{
    try
    {
        auto request = json::parse(json_command);
        std::string action = request["action"];
        std::string conveyor_name = request["conveyor"];
        
        if (action == "start")
        {
            int result = subsystem->start_conveyor(conveyor_name);
            return json{{"status", result == 0 ? "success" : "failed"}}.dump();
        }
        // ... handle other actions
    }
    catch (const std::exception& e)
    {
        return json{{"error", e.what()}}.dump();
    }
    
    return json{{"error", "invalid command"}}.dump();
}