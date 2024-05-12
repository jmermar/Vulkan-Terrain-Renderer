#pragma once

#include <span>

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
class Engine;
class CommandBuffer;
class BufferWriter {
    friend class Engine;

   private:
    struct TextureWriteOperation {
        Texture* texture;
        raii::Buffer buffer;
    };

    std::vector<TextureWriteOperation> textureWrites;

    Engine& engine;
    BufferWriter(Engine& engine) : engine(engine) {}

    void updateWrites(CommandBuffer& cmd);

    void enqueueTextureWrite(Texture* tex, void* data, uint32_t size);
};