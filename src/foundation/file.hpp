#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace file {
std::vector<uint8_t> readBinary(const std::string& path);
};
