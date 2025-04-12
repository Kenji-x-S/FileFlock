#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <string>
#include <vector>

class FileUtils {
public:
    static std::vector<std::string> splitFile(const std::string& filename, size_t chunkSize);
    static bool joinFiles(const std::vector<std::string>& chunkFiles, const std::string& outputFile);
};

#endif // FILE_UTILS_HPP
