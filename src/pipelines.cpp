#include "pipelines.hpp"

#include <cstdint>
#include <vulkan/vulkan_enums.hpp>

#include "types.hpp"

PipelineBuilder& PipelineBuilder::addStage(const std::span<uint8_t> shaderData,
                                           ShaderStage stage) {
    vk::ShaderModuleCreateInfo moduleCreate;
    moduleCreate.pCode = (uint32_t*)shaderData.data();
    moduleCreate.codeSize = shaderData.size();

    stages.push_back({.module = engine.device.createShaderModule(moduleCreate),
                      .stage = vk::ShaderStageFlagBits(stage)});

    return *this;
}

PipelineBuilder& PipelineBuilder::addVertexInputAttribute(
    uint32_t offset, VertexInputFormat format) {
    vk::VertexInputAttributeDescription attr;
    attr.format = vk::Format(format);
    attr.location = attributes.size();
    attr.offset = offset;
    attributes.push_back(attr);
    return *this;
}

PipelineBuilder& PipelineBuilder::fillTriangles() {
    assembly.topology = vk::PrimitiveTopology::eTriangleList;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    return *this;
}

PipelineBuilder& PipelineBuilder::drawTriangleLines() {
    assembly.topology = vk::PrimitiveTopology::eTriangleList;
    rasterizer.polygonMode = vk::PolygonMode::eLine;
    return *this;
}

PipelineBuilder& PipelineBuilder::drawLines() {
    assembly.topology = vk::PrimitiveTopology::eLineList;
    rasterizer.polygonMode = vk::PolygonMode::eLine;
    return *this;
}
