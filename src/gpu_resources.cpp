#include "gpu_resources.hpp"

#include <cassert>
#include <stdexcept>

#include "engine.hpp"
#include "helpers.hpp"
void BufferWriter::updateWrites(CommandBuffer& cmd) {
    cmd.memoryBarrier(
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eTransfer,
        vk::AccessFlagBits2::eMemoryWrite);

    for (auto& [texture, buffer] : textureWrites) {
        cmd.transitionTexture(texture, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        cmd.copyToTexture(texture, buffer);
        engine.destroyCpuBuffer(buffer);
    }
    textureWrites.clear();

    for (auto& [buffer, start, size, upload] : bufferWrites) {
        cmd.copyBufferToBuffer(buffer, upload, start, 0, size);
        engine.destroyCpuBuffer(upload);
    }

    bufferWrites.clear();

    cmd.memoryBarrier(
        vk::PipelineStageFlagBits2::eTransfer,
        vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead);
}

void BufferWriter::enqueueTextureWrite(Texture* tex, void* data) {
    const auto size =
        helpers::getTextureSizeFromSizeAndFormat(tex->size, tex->format);

    auto upload = engine.createCpuBuffer(size);
    engine.updateCPUBuffer(upload, data, size);

    textureWrites.push_back({.texture = tex, .buffer = upload});
}

void BufferWriter::enqueueBufferWrite(StorageBuffer* buffer, void* data,
                                      uint32_t start, size_t size) {
    assert(data);
    assert(start + size <= buffer->size);

    auto upload = engine.createCpuBuffer(size);
    engine.updateCPUBuffer(upload, data, size);

    bufferWrites.push_back({.buffer = buffer,
                            .start = start,
                            .size = size,
                            .uploadBuffer = upload});
}
