#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "types.hpp"

namespace file {
std::vector<uint8_t> readBinary(const std::string& path);
ImageData loadImage(const std::string& path);
ImageData loadImageArray(std::span<std::string> paths);
};  // namespace file
