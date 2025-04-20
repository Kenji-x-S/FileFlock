#include "chunker.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <json/json.h>  // from vcpkg

Chunker::Chunker(size_t chunkSize) : chunkSize(chunkSize) {}

bool Chunker::chunkFile(const std::string& inputPath, const std::string& outputMetaPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) return false;

    Json::Value metadataJson;
    std::vector<uint8_t> buffer(chunkSize);
    size_t index = 0;
    std::vector<std::string> chunkHashes;

    while (!in.eof()) {
        in.read(reinterpret_cast<char*>(buffer.data()), chunkSize);
        std::streamsize bytesRead = in.gcount();
        if (bytesRead <= 0) break;

        buffer.resize(static_cast<size_t>(bytesRead));

        std::string hash = computeSHA1(buffer);
        chunkHashes.push_back(hash);

        Json::Value chunkInfo;
        chunkInfo["index"] = static_cast<Json::UInt64>(index);
        chunkInfo["hash"] = hash;

        metadataJson["chunks"].append(chunkInfo);
        ++index;
        buffer.resize(chunkSize);
    }

    std::string fullFileHash = computeSHA1FromHashes(chunkHashes);

    in.clear();
    in.seekg(0, std::ios::end);
    auto fileSize = in.tellg();

    metadataJson["file_hash"] = fullFileHash;
    metadataJson["filename"] = inputPath;
    metadataJson["size"] = static_cast<Json::UInt64>(fileSize);
    metadataJson["chunk_size"] = static_cast<Json::UInt64>(chunkSize);
    metadataJson["total_chunks"] = static_cast<Json::UInt64>(index);
    metadataJson["peers"] = Json::arrayValue;

    std::ofstream outMeta(outputMetaPath);
    outMeta << metadataJson;

    return true;
}

std::vector<uint8_t> Chunker::readChunk(const std::string& filepath, size_t offset, size_t size) {
    std::vector<uint8_t> buffer(size);
    std::ifstream in(filepath, std::ios::binary);
    if (!in) return {};

    in.seekg(offset);
    in.read(reinterpret_cast<char*>(buffer.data()), size);

    buffer.resize(in.gcount());
    return buffer;
}

std::string Chunker::computeSHA1(const std::vector<uint8_t>& data) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data.data(), data.size(), hash);

    std::ostringstream oss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);

    return oss.str();
}

std::string Chunker::computeSHA1FromHashes(const std::vector<std::string>& chunkHashes) {
    std::ostringstream concatenatedHashes;
    for (const auto& hash : chunkHashes) {
        concatenatedHashes << hash;
    }

    std::string concatenatedStr = concatenatedHashes.str();
    return computeSHA1(std::vector<uint8_t>(concatenatedStr.begin(), concatenatedStr.end()));
}
