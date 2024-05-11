#include "commands.hpp"

void CommandBuffer::begin() {
    cmd.reset();
    vk::CommandBufferBeginInfo cmdBeginInfo;
    cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmd.begin(cmdBeginInfo);
}

void CommandBuffer::transitionImage(vk::Image image, vk::ImageLayout srcLayout,
                                    vk::PipelineStageFlagBits2 srcStage,
                                    vk::ImageLayout dstLayout,
                                    vk::PipelineStageFlagBits2 dstStage) {
    vk::ImageMemoryBarrier2KHR imageBarrier;
    imageBarrier.srcAccessMask = vk::AccessFlagBits2::eMemoryWrite;
    imageBarrier.srcStageMask = srcStage;

    imageBarrier.dstAccessMask =
        vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
    imageBarrier.dstStageMask = dstStage;

    imageBarrier.oldLayout = srcLayout;
    imageBarrier.newLayout = dstLayout;

    vk::ImageSubresourceRange range;
    range.levelCount = vk::RemainingMipLevels;
    range.layerCount = vk::RemainingArrayLayers;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;

    imageBarrier.subresourceRange = range;

    imageBarrier.image = image;

    vk::DependencyInfo dependencyInfo;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &imageBarrier;
    cmd.pipelineBarrier2(dependencyInfo);
}

void CommandBuffer::clearImage(vk::Image image, float r, float g, float b,
                               float a) {
    vk::ImageSubresourceRange range;
    range.levelCount = vk::RemainingMipLevels;
    range.layerCount = vk::RemainingArrayLayers;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;

    vk::ClearColorValue color;
    color.setFloat32({r, g, b, a});
    cmd.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal, color,
                        range);
}
