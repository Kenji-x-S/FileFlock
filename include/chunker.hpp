#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct ChunkMetadata {
    std::string hash;
    uint64_t index;
};

class Chunker {
public:
    Chunker(size_t chunkSize = 512 * 1024); // Default 512KB

    bool chunkFile(const std::string& inputPath, const std::string& outputMetaPath);
    static std::vector<uint8_t> readChunk(const std::string& filepath, size_t offset, size_t size);

private:
    std::string computeSHA1(const std::vector<uint8_t>& data);
    std::string computeSHA1FromHashes(const std::vector<std::string>& chunkHashes);
    size_t chunkSize;
};
