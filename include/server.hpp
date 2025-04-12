#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>

class Server {
public:
    void startServer(const std::string& ip, int port,const std::vector<std::string>& chunks);
    void sendChunks(int clientSocket, const std::vector<std::string>& chunks);
};

#endif // SERVER_HPP
