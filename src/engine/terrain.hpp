#pragma once
#include "../val/vulkan_abstraction.hpp"
#include "types.hpp"
namespace engine {
struct TerrainVertexData {
    glm::vec3 pos;
    glm::vec3 color;
};

struct TerrainPushConstants {
    glm::mat4 proj, view, model;
    val::BindPoint<val::Texture> grassBind;
};

struct TerrainChunk {
    val::Mesh* mesh{};
    glm::vec2 size{};
    glm::vec3 position{};
};

class TerrainRenderer {
   private:
    val::Engine& engine;
    val::BufferWriter& writer;
    val::GraphicsPipeline pass{};
    val::Texture* grass;
    TerrainChunk chunk{};

    TerrainChunk generateChunk(const glm::vec3& position);

   public:
    TerrainRenderer(val::Engine& engine, val::BufferWriter& writer);

    void renderPass(val::Texture* depth, val::Texture* framebuffer,
                    const CameraData& cam, val::CommandBuffer& cmd);
};
}  // namespace engine
