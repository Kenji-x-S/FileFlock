#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class Client {
public:
    void runClient(const std::string& ip, int port);
    SOCKET clientSocket;
};

#endif // CLIENT_HPP
