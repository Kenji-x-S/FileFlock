#include "server.hpp"
#include <iostream>
#include <thread>
#include <winsock2.h>  // For Windows socket programming
#include <vector>

#pragma comment(lib, "ws2_32.lib")  // Link against the Winsock library

void Server::startServer(const std::string& ip, int port, const std::vector<std::string>& chunks) {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return;
    }

    // Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Setup address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());  // Convert IP address
    serverAddr.sin_port = htons(port);  // Convert port to network byte order

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server started on " << ip << ":" << port << std::endl;

    // Accept a client connection
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept client connection: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Client connected!" << std::endl;

    sendChunks(clientSocket, chunks);

    // Close the server socket (client socket will be closed after sending all chunks)
    closesocket(serverSocket);

    std::cout << "Server stopped." << std::endl;

    // Clean up Winsock
    WSACleanup();
}

void Server::sendChunks(int clientSocket, const std::vector<std::string>& chunks) {
    for (const auto& chunk : chunks) {
        size_t chunkSize = chunk.size();

        // Send the chunk size first (can be used by the client to know how much data to expect)
        int bytesSent = send(clientSocket, reinterpret_cast<const char*>(&chunkSize), sizeof(chunkSize), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error sending chunk size." << std::endl;
            break;  // Stop sending chunks if there's an error
        }

        // Send the actual chunk data
        bytesSent = send(clientSocket, chunk.data(), chunkSize, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error sending chunk data." << std::endl;
            break;  // Stop sending chunks if there's an error
        }

        std::cout << "Sent chunk of size " << chunkSize << " bytes to client." << std::endl;

        // Optionally, you could add a small delay here (e.g., using `std::this_thread::sleep_for`) to control the rate of sending.
    }

    // Optionally, you can send a message or signal that no more chunks are coming (e.g., sending a zero-length chunk or a special message).
    std::cout << "All chunks sent to client." << std::endl;
}
