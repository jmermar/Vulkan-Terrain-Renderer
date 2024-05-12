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

void CommandBuffer::copyToTexture(Texture* t, vk::Buffer origin,
                                  vk::PipelineStageFlagBits2 srcStage,
                                  vk::PipelineStageFlagBits2 dstStage) {
    vk::BufferImageCopy copyRegion;
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;

    copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageExtent.width = t->size.w;
    copyRegion.imageExtent.height = t->size.h;
    copyRegion.imageExtent.depth = 1;

    cmd.copyBufferToImage(origin, t->image,
                          vk::ImageLayout::eTransferDstOptimal, {copyRegion});
}

void CommandBuffer::copyTextureToTexture(Texture* src, Texture* dst) {
    vk::ImageBlit2 region;
    region.srcSubresource.layerCount = 1;
    region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.srcOffsets[0] = {.x = 0, .y = 0, .z = 0};
    region.srcOffsets[1] = {
        .x = (int32_t)src->size.w, .y = (int32_t)src->size.h, .z = (int32_t)1};

    region.dstSubresource.layerCount = 1;
    region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.dstOffsets[0] = {.x = 0, .y = 0, .z = 0};
    region.dstOffsets[1] = {
        .x = (int32_t)dst->size.w, .y = (int32_t)dst->size.h, .z = (int32_t)1};

    vk::BlitImageInfo2 blitInfo;
    blitInfo.srcImage = src->image;
    blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
    blitInfo.filter = vk::Filter::eNearest;
    blitInfo.regionCount = 1;
    blitInfo.pRegions = &region;
    blitInfo.dstImage = dst->image;
    blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;

    cmd.blitImage2(blitInfo);
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
