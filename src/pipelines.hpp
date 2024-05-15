#pragma once

#include "engine.hpp"
#include "gpu_resources.hpp"
#include "raii.hpp"
#include "types.hpp"
class PipelineBuilder {
   private:
    Engine& engine;
    struct CustomShaderStage {
        vk::raii::ShaderModule module;
        vk::ShaderStageFlags stage;
    };

    std::vector<vk::VertexInputAttributeDescription> attributes;

    std::vector<CustomShaderStage> stages;

    vk::PushConstantRange pushConstant{};

    vk::PipelineInputAssemblyStateCreateInfo assembly;
    vk::PipelineRasterizationStateCreateInfo rasterizer;

   public:
    PipelineBuilder(Engine& engine) : engine(engine) {
        drawLines();
        setCullMode(PolygonCullMode::NONE);
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    }
    template <typename T>
    PipelineBuilder& setPushConstant() {
        pushConstant.size = sizeof(T);
        pushConstant.offset = 0;

        return *this;
    }

    PipelineBuilder& addStage(const std::span<uint8_t> shaderData,
                              ShaderStage stage);
    PipelineBuilder& clearStages() {
        stages.clear();
        return *this;
    }
    PipelineBuilder& addVertexInputAttribute(uint32_t offset,
                                             VertexInputFormat format);
    PipelineBuilder& clearVertexInputAttributes() {
        attributes.clear();
        return *this;
    }

    PipelineBuilder& fillTriangles();
    PipelineBuilder& drawTriangleLines();
    PipelineBuilder& drawLines();

    PipelineBuilder& setCullMode(PolygonCullMode mode) {
        rasterizer.polygonMode = vk::PolygonMode(mode);
        rasterizer.lineWidth = 1.f;
        return *this;
    }
};
