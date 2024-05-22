#pragma once
#include "../val/vulkan_abstraction.hpp"
#include "types.hpp"
namespace engine {
class TerrainRenderer {
   private:
    val::Engine& engine;
    val::BufferWriter& writer;
    val::GraphicsPipeline pass{};
    val::GraphicsPipeline depthPrepass{};
    val::ComputePipeline patchGenerator{};
    val::StorageBuffer* vertexOutput;
    val::StorageBuffer* drawIndirectCommand;

    struct {
        val::Texture* grass;
        val::Texture* snow;
        val::Texture* rock1;
        val::Texture* rock2;
    } textures;

    void loadTextures();
    void initRenderPass();
    void initCompute();

   public:
    TerrainRenderer(val::Engine& engine, val::BufferWriter& writer);

    void renderComputePass(const RenderState& rs, val::CommandBuffer& cmd,
                           val::StorageBuffer* waterVertices,
                           val::StorageBuffer* waterDraw);

    void renderDepthPrepass(val::Texture* depth, const RenderState& rs,
                            val::CommandBuffer& cmd);

    void renderPass(val::Texture* depth, val::Texture* framebuffer,
                    const RenderState& rs, val::CommandBuffer& cmd);
};
}  // namespace engine
