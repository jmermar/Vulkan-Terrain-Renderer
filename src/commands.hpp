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

    void copyToTexture(Texture* t, vk::Buffer origin,
                       vk::PipelineStageFlagBits2 srcStage =
                           vk::PipelineStageFlagBits2::eAllCommands,
                       vk::PipelineStageFlagBits2 dstStage =
                           vk::PipelineStageFlagBits2::eAllCommands);

    void copyTextureToTexture(Texture* src, Texture* dst);

    void clearImage(vk::Image image, float r, float g, float b, float a);

    bool isValid() { return cmd != 0; }
};
