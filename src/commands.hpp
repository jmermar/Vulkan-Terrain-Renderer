#pragma once

#include "gpu_resources.hpp"

class CommandBuffer {
    friend class Engine;

   private:
    vk::CommandBuffer cmd;
    CommandBuffer(vk::CommandBuffer cmd) : cmd(cmd) {}

    void begin();

    void transitionImage(vk::Image image, vk::ImageLayout srcLayout,
                         vk::PipelineStageFlagBits2 srcStage,
                         vk::ImageLayout dstLayout,
                         vk::PipelineStageFlagBits2 dstStage);

    inline void transitionImage(vk::Image image, vk::ImageLayout srcLayout,
                                vk::ImageLayout dstLayout) {
        transitionImage(image, srcLayout,
                        vk::PipelineStageFlagBits2::eAllCommands, dstLayout,
                        vk::PipelineStageFlagBits2::eAllCommands);
    }

    void copyBufferToBuffer(StorageBuffer* dst, vk::Buffer src,
                            uint32_t srcStart, uint32_t dstStart, size_t size);

    void copyToTexture(Texture* t, vk::Buffer buffer,
                       vk::PipelineStageFlagBits2 srcStage =
                           vk::PipelineStageFlagBits2::eAllCommands,
                       vk::PipelineStageFlagBits2 dstStage =
                           vk::PipelineStageFlagBits2::eAllCommands);

   public:
    inline void transitionTexture(Texture* texture, vk::ImageLayout srcLayout,
                                  vk::PipelineStageFlagBits2 srcStage,
                                  vk::ImageLayout dstLayout,
                                  vk::PipelineStageFlagBits2 dstStage) {
        transitionImage(texture->image, srcLayout, srcStage, dstLayout,
                        dstStage);
    }
    inline void transitionTexture(Texture* texture, vk::ImageLayout srcLayout,
                                  vk::ImageLayout dstLayout) {
        transitionImage(texture->image, srcLayout,
                        vk::PipelineStageFlagBits2::eAllCommands, dstLayout,
                        vk::PipelineStageFlagBits2::eAllCommands);
    }

    void copyToTexture(Texture* t, CPUBuffer* buffer,
                       vk::PipelineStageFlagBits2 srcStage =
                           vk::PipelineStageFlagBits2::eAllCommands,
                       vk::PipelineStageFlagBits2 dstStage =
                           vk::PipelineStageFlagBits2::eAllCommands) {
        copyToTexture(t, buffer->buffer, srcStage, dstStage);
    }

    void copyToTexture(Texture* t, StorageBuffer* buffer,
                       vk::PipelineStageFlagBits2 srcStage =
                           vk::PipelineStageFlagBits2::eAllCommands,
                       vk::PipelineStageFlagBits2 dstStage =
                           vk::PipelineStageFlagBits2::eAllCommands) {
        copyToTexture(t, buffer->buffer, srcStage, dstStage);
    }

    void memoryBarrier(vk::PipelineStageFlags2 srcStage,
                       vk::AccessFlags2 srcAccess,
                       vk::PipelineStageFlags2 dstStage,
                       vk::AccessFlags2 dstAccess);

    void copyTextureToTexture(Texture* src, Texture* dst);

    void copyBufferToBuffer(StorageBuffer* dst, StorageBuffer* src,
                            uint32_t srcStart, uint32_t dstStart, size_t size) {
        copyBufferToBuffer(dst, src->buffer, srcStart, dstStart, size);
    }

    void copyBufferToBuffer(StorageBuffer* dst, CPUBuffer* src,
                            uint32_t srcStart, uint32_t dstStart, size_t size) {
        copyBufferToBuffer(dst, src->buffer, srcStart, dstStart, size);
    }

    void clearImage(vk::Image image, float r, float g, float b, float a);

    bool isValid() { return cmd != 0; }
};
