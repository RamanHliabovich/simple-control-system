#include "unity_client.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <ws2tcpip.h>

using json = nlohmann::json;

unity_client::unity_client(const std::string& host, int port)
    : client_socket(INVALID_SOCKET), is_connected(false), server_host(host), server_port(port)
{
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cerr << "Unity Client: WSAStartup failed\n";
    }
}

unity_client::~unity_client()
{
    disconnect();
    WSACleanup();
}

bool unity_client::connect()
{
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Unity client: Failed to create socket\n";
        return false;
    }
    
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_host.c_str(), &(server_addr.sin_addr));
    server_addr.sin_port = htons(server_port);
    
    if (::connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Unity client: Connection failed with error: " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
        return false;
    }
    
    is_connected = true;
    std::cout << "Connected to Unity server at " << server_host << ":" << server_port << "\n";
    return true;
}

void unity_client::disconnect()
{
    if (client_socket != INVALID_SOCKET)
    {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }
    is_connected = false;
}

bool unity_client::send_outbit_and_get_inbit(const std::string& conveyor_id, bool outbit_state, bool& inbit_response)
{
    if (!is_connected || client_socket == INVALID_SOCKET)
    {
        std::cerr << "Not connected to Unity server\n";
        return false;
    }
    
    // Create outbit request
    json request = {
        {"type", "outbit"},
        {"conveyor", conveyor_id},
        {"outbit", outbit_state}
    };
    
    std::string message = request.dump();
    
    // Send outbit
    int send_result = send(client_socket, message.c_str(), static_cast<int>(message.length()), 0);
    if (send_result == SOCKET_ERROR)
    {
        std::cerr << "Failed to send outbit to Unity. Error: " << WSAGetLastError() << "\n";
        is_connected = false;
        return false;
    }
    
    std::cout << "Sent outbit (" << (outbit_state ? "ON" : "OFF") << ") to Unity for " << conveyor_id << "\n";
    
    // Receive inbit response
    char buffer[1024] = {};
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received > 0)
    {
        std::string response(buffer, bytes_received);
        std::cout << "Unity response: " << response << "\n";
        
        try
        {
            auto json_response = json::parse(response);
            
            if (json_response.contains("inbit"))
            {
                inbit_response = json_response["inbit"];
                std::cout << "Received inbit (" << (inbit_response ? "ON" : "OFF") << ") from Unity\n";
                return true;
            }
        }
        catch (const json::exception& e)
        {
            std::cerr << "Failed to parse Unity response: " << e.what() << "\n";
            return false;
        }
    }
    else if (bytes_received == 0)
    {
        std::cout << "Unity server disconnected\n";
        is_connected = false;
    }
    else
    {
        std::cerr << "Recv error: " << WSAGetLastError() << "\n";
        is_connected = false;
    }
    
    return false;
}