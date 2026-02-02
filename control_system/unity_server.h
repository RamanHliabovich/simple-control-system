#pragma once
#include <winsock2.h>
#include <thread>
#include <string>
#include <functional>

class unity_server
{
private:
    SOCKET listening_socket;
    SOCKET client_socket;
    bool is_running;
    bool client_connected;
    std::thread server_thread;
    std::thread client_thread;
    int port;
    
    void run_server();
    void handle_unity_client();

public:
    unity_server(int port = 5001);
    ~unity_server();
    
    bool start();
    void stop();
    
    // Send driver outbit status to Unity client
    void send_driver_outbit(const std::string& conveyor_id, bool outbit_state);
    
    // Callback for when Unity sends inbit confirmation
    std::function<void(const std::string& conveyor_id, bool inbit_state)> on_inbit_received;
    
    bool is_client_connected() const { return client_connected; }
};