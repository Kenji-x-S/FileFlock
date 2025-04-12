#include "server.hpp"
#include "client.hpp"
#include "config.hpp"
#include "file_utils.hpp"
#include <iostream>
#include <filesystem>

int main() {
    Config::loadConfig();

    std::filesystem::path cwd = std::filesystem::current_path();

    std::filesystem::path projectDir = cwd.parent_path().parent_path();
    std::filesystem::current_path(projectDir);
    std::filesystem::path filePath = "data/input_file.txt";

    if (Config::isServerMode()) {
        Server server;
        FileUtils fileUtils;
        auto chunkFiles = fileUtils.splitFile(filePath.string(), 1024);  // Split input_file.txt into 1024-byte chunks
        server.startServer("127.0.0.1", 8080,chunkFiles);
    } else {
        Client client;
        client.runClient("127.0.0.1", 8080);
    }

    return 0;
}
