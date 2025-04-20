#include "peer_server.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <json/json.h>
#include <winsock2.h>  // For Windows socket programming
#include <ws2tcpip.h>  // For Windows socket functions
#include "chunker.hpp"
#include "peer_utils.hpp"


void handleClient(SOCKET clientSocket, const std::string& filePath, size_t chunkSize) {
    uint64_t chunkIndex;
    if (recv(clientSocket, reinterpret_cast<char*>(&chunkIndex), sizeof(chunkIndex), 0) <= 0) {
        closesocket(clientSocket);
        return;
    }

    size_t offset = chunkIndex * chunkSize;
    auto chunk = Chunker::readChunk(filePath, offset, chunkSize);

    uint64_t chunkSizeToSend = chunk.size();
    send(clientSocket, reinterpret_cast<const char*>(&chunkSizeToSend), sizeof(chunkSizeToSend), 0);
    send(clientSocket, reinterpret_cast<const char*>(chunk.data()), chunkSizeToSend, 0);
    closesocket(clientSocket);
}

void runSeeder(const std::string& metadataPath, const std::string& filePath, int port) {
    std::ifstream in(metadataPath);
    Json::Value metaJson;
    in >> metaJson;
    size_t chunkSize = metaJson["chunk_size"].asUInt64();

    // Initialize Winsock
    initWinsock();

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "[Seeder] Failed to create server socket\n";
        cleanupWinsock();
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "[Seeder] Bind failed\n";
        closesocket(serverSocket);
        cleanupWinsock();
        return;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "[Seeder] Listen failed\n";
        closesocket(serverSocket);
        cleanupWinsock();
        return;
    }

    std::cout << "[Seeder] Serving chunks from: " << filePath << " on port " << port << "\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "[Seeder] Accept failed\n";
            continue;
        }

        std::thread(handleClient, clientSocket, filePath, chunkSize).detach();
    }

    // Cleanup Winsock
    closesocket(serverSocket);
    cleanupWinsock();
}
