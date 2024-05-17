#include "file.hpp"

#include <fstream>

namespace file {
std::vector<uint8_t> readBinary(const std::string& path) {
    std::ifstream file(std::string(RESPATH) + path,
                       std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot read file: " + path);
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);

    file.read((char*)buffer.data(), fileSize);
    return buffer;
}
}  // namespace file
