#pragma once

#include "gpu_resources.hpp"

class CommandBuffer {
    friend class Engine;

   private:
    vk::CommandBuffer cmd;
    CommandBuffer(vk::CommandBuffer cmd) : cmd(cmd) {}

    void begin();

   public:
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

    void clearImage(vk::Image image, float r, float g, float b, float a);

    bool isValid() { return cmd != 0; }
};