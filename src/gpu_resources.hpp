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

struct CPUBuffer {
    size_t size{};
    raii::Buffer buffer;
};

class Engine;
class CommandBuffer;
class BufferWriter {
    friend class Engine;

   private:
    struct TextureWriteOperation {
        Texture* texture;
        CPUBuffer* buffer;
    };

    struct BufferWrite {
        StorageBuffer* buffer{};
        uint32_t start{};
        size_t size{};
        CPUBuffer* uploadBuffer;
    };

    std::vector<TextureWriteOperation> textureWrites;
    std::vector<BufferWrite> bufferWrites;

    Engine& engine;

   public:
    BufferWriter(Engine& engine) : engine(engine) {}

    void updateWrites(CommandBuffer& cmd);

    void enqueueTextureWrite(Texture* tex, void* data);
    void enqueueBufferWrite(StorageBuffer* buffer, void* data, uint32_t start,
                            size_t size);
};