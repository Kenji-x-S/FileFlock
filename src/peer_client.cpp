#include "peer_client.hpp"
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <winsock2.h> // For Windows socket programming
#include <ws2tcpip.h> // For Windows socket functions
#include <cstdio>
#include "peer_utils.hpp"


void runDownloader(const std::string& metadataPath, const std::string& outputFile) {
    std::ifstream in(metadataPath);
    Json::Value metaJson;
    in >> metaJson;

    size_t chunkSize = metaJson["chunk_size"].asUInt64();
    size_t totalChunks = metaJson["total_chunks"].asUInt64();
    const Json::Value& peers = metaJson["peers"];

    std::ofstream out(outputFile, std::ios::binary | std::ios::trunc);

    // Initialize Winsock
    initWinsock();

    for (size_t i = 0; i < totalChunks; ++i) {
        bool downloaded = false;

        for (const auto& peer : peers) {
            std::string ip = peer["ip"].asString();
            int port = peer["port"].asInt();

            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock == INVALID_SOCKET) {
                std::cerr << "[Downloader] Socket creation failed\n";
                continue;
            }

            sockaddr_in serverAddr{};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

            if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cerr << "[Downloader] Connection failed\n";
                closesocket(sock);
                continue;
            }

            send(sock, reinterpret_cast<const char*>(&i), sizeof(i), 0);

            uint64_t size = 0;
            recv(sock, reinterpret_cast<char*>(&size), sizeof(size), 0);

            std::vector<char> buffer(size);
            recv(sock, buffer.data(), size, 0);

            out.seekp(i * chunkSize);
            out.write(buffer.data(), size);

            closesocket(sock);
            downloaded = true;
            break;
        }
        
        if (!downloaded) {
            std::cerr << "[Downloader] Failed to download chunk " << i << "\n";
        } else {
            std::cout << "[Downloader] Downloaded chunk " << i+1 << "/" << totalChunks << "\n";
        }
    }

    out.close();

    // Cleanup Winsock
    cleanupWinsock();
}
