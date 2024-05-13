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

void CommandBuffer::memoryBarrier(vk::PipelineStageFlags2 srcStage,
                                  vk::AccessFlags2 srcAccess,
                                  vk::PipelineStageFlags2 dstStage,
                                  vk::AccessFlags2 dstAccess) {
    vk::MemoryBarrier2 memoryBarrier;
    memoryBarrier.srcAccessMask = srcAccess;
    memoryBarrier.dstAccessMask = dstAccess;
    memoryBarrier.srcStageMask = srcStage;
    memoryBarrier.dstStageMask = dstStage;
    vk::DependencyInfo dependencyInfo;
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &memoryBarrier;
    cmd.pipelineBarrier2(dependencyInfo);
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

void CommandBuffer::copyBufferToBuffer(StorageBuffer* dst, vk::Buffer src,
                                       uint32_t srcStart, uint32_t dstStart,
                                       size_t size) {
    vk::CopyBufferInfo2 copyInfo;
    copyInfo.srcBuffer = src;
    copyInfo.dstBuffer = dst->buffer;

    vk::BufferCopy2 region;
    region.srcOffset = srcStart;
    region.dstOffset = dstStart;
    region.size = size;
    copyInfo.regionCount = 1;
    copyInfo.pRegions = &region;
    cmd.copyBuffer2(copyInfo);
}

void CommandBuffer::copyToMesh(Mesh* mesh, CPUBuffer* vertices,
                               CPUBuffer* indices) {
    vk::CopyBufferInfo2 copyInfo;
    copyInfo.srcBuffer = vertices->buffer;
    copyInfo.dstBuffer = mesh->vertices;

    vk::BufferCopy2 region;
    region.size = vertices->size;
    copyInfo.regionCount = 1;
    copyInfo.pRegions = &region;
    cmd.copyBuffer2(copyInfo);

    copyInfo.srcBuffer = indices->buffer;
    copyInfo.dstBuffer = mesh->indices;

    region.size = indices->size;
    cmd.copyBuffer2(copyInfo);
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
