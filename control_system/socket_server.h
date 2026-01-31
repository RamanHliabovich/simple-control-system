#pragma once
#include <winsock2.h>
#include <thread>
#include <string>

class socket_server
{
private:
    SOCKET listening_socket;
    bool is_running;
    std::thread server_thread;
    
    class conveyor_subsystem* subsystem;
    
    void run_server();
    std::string process_json_command(const std::string& json_command);

public:
    socket_server(conveyor_subsystem* subsystem, int port = 5000);
    ~socket_server();
    
    bool start();
    void stop();
};