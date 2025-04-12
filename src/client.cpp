#include "client.hpp"
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <vector>

void Client::runClient(const std::string& ip, int port) {
    std::cout << "Connecting to server at " << ip << ":" << port << std::endl;

    // Initialize WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return;
    }

    // Setup address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    // Connect
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    std::cout << "Connected to server." << std::endl;

    // Ensure chunk directory exists
    std::filesystem::path chunksDir = "data/chunks";
    std::filesystem::create_directories(chunksDir);

    size_t chunkIndex = 0;

    while (true) {
        size_t chunkSize = 0;

        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 0);
        if (bytesReceived <= 0) {
            break;
        }

        std::vector<char> chunkData(chunkSize);
        bytesReceived = recv(clientSocket, chunkData.data(), chunkSize, 0);
        if (bytesReceived <= 0) {
            break;
        }

        std::string chunkFilename = "data/chunks/chunk_" + std::to_string(chunkIndex++) + ".bin";
        std::ofstream chunkFile(chunkFilename, std::ios::binary);
        if (chunkFile.is_open()) {
            chunkFile.write(chunkData.data(), chunkSize);
            chunkFile.close();
            std::cout << "Saved " << chunkFilename << std::endl;
        } else {
            std::cerr << "Failed to write file: " << chunkFilename << std::endl;
        }
    }

    std::cout << "All chunks received. Cleaning up..." << std::endl;
    closesocket(clientSocket);
    WSACleanup();
}
