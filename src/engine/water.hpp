#pragma once
#include "../val/vulkan_abstraction.hpp"
#include "types.hpp"
namespace engine {
class WaterRenderer {
    friend class Engine;

   private:
    val::Engine& engine;
    val::BufferWriter& writer;
    val::GraphicsPipeline pass{};
    val::StorageBuffer* vertices;
    val::StorageBuffer* drawCommand;
    val::Texture* dudvMap;
    val::Texture* normals;

    void loadTextures();
    void initRenderPass();

   public:
    WaterRenderer(val::Engine& engine, val::BufferWriter& writer);

    void renderPass(val::Texture* depth, val::Texture* framebuffer,
                    val::Texture* screen, const RenderState& rs,
                    val::CommandBuffer& cmd);
};
}  // namespace engine
