#include <iostream>
#include <string>
#include "peer_server.hpp"
#include "peer_client.hpp"

int main(int argc, char* argv[]) {
    if (argc < 4) { // Adjusted to check if the port number is provided
        std::cout << "Usage:\n";
        std::cout << "  Seeder:   ./fileflock --seed <metadata.json> <filepath> <port>\n";
        std::cout << "  Downloader: ./fileflock --download <metadata.json> <outputfile>\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "--seed") {
        std::string metadataFile = argv[2];
        std::string filePath = argv[3];
        int port = std::stoi(argv[4]);  // Parse the port from command line argument
        std::cout << "[Main] Port passed: " << port << "\n";  // Debug line
        runSeeder(metadataFile, filePath, port);
    }
    else if (mode == "--download") {
        std::string metadataFile = argv[2];
        std::string outputFile = argv[3];
        runDownloader(metadataFile, outputFile);
    }
    else {
        std::cout << "Unknown mode.\n";
    }

    return 0;
}
