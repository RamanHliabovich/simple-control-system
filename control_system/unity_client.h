#pragma once
#include <winsock2.h>
#include <string>

class unity_client
{
private:
    SOCKET client_socket;
    bool is_connected;
    std::string server_host;
    int server_port;

public:
    unity_client(const std::string& host, int port);
    ~unity_client();
    
    bool connect();
    void disconnect();
    
    // Send outbit and receive inbit response in one call
    bool send_outbit_and_get_inbit(const std::string& conveyor_id, bool outbit_state, bool& inbit_response);
    
    bool is_connected_to_unity() const { return is_connected; }
};