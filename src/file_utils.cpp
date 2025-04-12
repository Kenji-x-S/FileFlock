#include "file_utils.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

std::vector<std::string> FileUtils::splitFile(const std::string& filename, size_t chunkSize) {
    // Check if file exists before proceeding
    if (!std::filesystem::exists(filename)) {
        std::cerr << "Error: File does not exist: " << filename << "\n";
        return {};
    }

    std::ifstream inputFile(filename, std::ios::binary);
    std::vector<std::string> chunkFiles;

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file for splitting.\n";
        return chunkFiles;
    }

    while (!inputFile.eof()) {
        std::string buffer(chunkSize, '\0');  // Pre-allocate buffer
        inputFile.read(&buffer[0], chunkSize);
        size_t bytesRead = inputFile.gcount();

        if (bytesRead > 0) {
            buffer.resize(bytesRead);  // Resize to actual bytes read
            chunkFiles.push_back(std::move(buffer));
        }
    }

    std::cout << "Chunks loaded in memory (as strings): " << chunkFiles.size() << std::endl;
    inputFile.close();
    return chunkFiles;
}

bool FileUtils::joinFiles(const std::vector<std::string>& chunkFiles, const std::string& outputFile) {
    std::ofstream output(outputFile, std::ios::binary);

    for (const auto& chunk : chunkFiles) {
        std::ifstream input(chunk, std::ios::binary);
        if (input.is_open()) {
            output << input.rdbuf();
            input.close();
        } else {
            std::cerr << "Error reading chunk file: " << chunk << "\n";
            return false;
        }
    }

    output.close();
    return true;
}
