#include "file.hpp"

#include <stb_image.h>

#include <cassert>
#include <cstring>
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

ImageData loadImage(const std::string& path) {
    int w, h, channels;
    std::string fullPath = std::string(RESPATH) + path;
    uint8_t* img = stbi_load(fullPath.c_str(), &w, &h, &channels, 4);
    ImageData ret;
    ret.size.w = w;
    ret.size.h = h;
    ret.size.depth = 1;
    ret.data.resize(w * h * 4);
    memcpy(ret.data.data(), img, w * h * 4);
    stbi_image_free(img);
    return ret;
}
ImageData loadImageArray(std::span<std::string> paths) {
    assert(paths.size() > 0);
    Size3D dimensions;
    dimensions.depth = paths.size();
    std::vector<ImageData> imgs;
    for (const auto& path : paths) {
        imgs.push_back(loadImage(path));
    }
    dimensions.w = imgs[0].size.w;
    dimensions.h = imgs[0].size.h;

    ImageData ret;
    ret.size = dimensions;
    ret.data.resize(dimensions.w * dimensions.h * dimensions.depth * 4);
    int i = 0;
    for (const auto& img : imgs) {
        assert(img.size.w == dimensions.w && img.size.h == dimensions.h);
        memcpy(ret.data.data() + i * img.size.w * img.size.h * 4,
               img.data.data(), img.size.w * img.size.h * 4);
        i++;
    }

    return ret;
}
}  // namespace file
