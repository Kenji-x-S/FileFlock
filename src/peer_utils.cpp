#include "peer_utils.hpp"
#include <winsock2.h>
#include <windows.h>
#include <iostream>

void initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed.\n";
        exit(1);
    }
}

void cleanupWinsock() {
    WSACleanup();
}
