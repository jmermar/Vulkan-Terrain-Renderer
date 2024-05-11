#pragma once

#include "raii.hpp"
#include "types.hpp"

struct StorageBuffer {
    BindPoint<StorageBuffer> bindPoint{};
    raii::Buffer buffer{};

    size_t size{};
};

struct Texture {
    BindPoint<Texture> bindPoint{};
    raii::Image image{};
    vk::raii::ImageView imageView{nullptr};
    Size size{};
    TextureFormat format{};
    TextureSampler sampler{};
};

struct Mesh {
    size_t indicesCount{};
    raii::Buffer vertices{};
    raii::Buffer indices{};
};
