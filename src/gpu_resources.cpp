#include "gpu_resources.hpp"

#include <stdexcept>

#include "engine.hpp"
raii::Buffer createStagingBuffer(raii::VMA& vma, void* data, size_t size) {
    VkBufferCreateInfo bufferInfo = {.sType =
                                         VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.pNext = nullptr;
    bufferInfo.size = size;

    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    return raii::Buffer(vma, bufferInfo, vmaAllocInfo);
}  // namespace gpu_resources

void BufferWriter::updateWrites(CommandBuffer& cmd) {
    for (auto& [texture, buffer] : textureWrites) {
        cmd.transitionTexture(texture, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        cmd.copyToTexture(texture, buffer);
        engine.deletionQueue.rawBuffers.push_back(std::move(buffer));
    }
    textureWrites.clear();
}

void BufferWriter::enqueueTextureWrite(Texture* tex, void* data,
                                       uint32_t size) {
    const auto numPixels = tex->size.w * tex->size.h;
    if (tex->format == TextureFormat::RGBA8 && size != numPixels * 4) {
        throw std::runtime_error("Texture size and buffer size dont match!");
    } else if (tex->format == TextureFormat::RGBA16 && size != numPixels * 8) {
        throw std::runtime_error("Texture size and buffer size dont match!");
    }
    auto upload = createStagingBuffer(engine.vma, data, size);
    memcpy(upload.allocInfo.pMappedData, data, size);

    textureWrites.push_back({.texture = tex, .buffer = std::move(upload)});
}
