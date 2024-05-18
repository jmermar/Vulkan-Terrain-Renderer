#pragma once
#include "../val/vulkan_abstraction.hpp"
#include "types.hpp"
namespace engine {
struct TerrainVertexData {
    glm::vec4 pos;
};

struct TerrainPushConstants {
    glm::mat4 proj, view, model;
    val::BindPoint<val::Texture> grassBind;
};

struct ComputeGlobalData {
    uint32_t numVertices;
    uint32_t patchIndex;
    uint32_t pad[2];
};

struct TerrainComputePushConstants {
    glm::vec3 camPos;
    val::BindPoint<val::StorageBuffer> patchesBind;
    val::BindPoint<val::StorageBuffer> globalBind;
};

struct TerrainChunk {
    val::Mesh* mesh{};
    glm::vec2 size{};
    glm::vec3 position{};
};

constexpr float PATCH_LENGTH = 128;
constexpr uint32_t MAX_PATCH = 32;
constexpr uint32_t NUM_PATCHES = MAX_PATCH * MAX_PATCH;

class TerrainRenderer {
   private:
    val::Engine& engine;
    val::BufferWriter& writer;
    val::GraphicsPipeline pass{};
    val::ComputePipeline patchGenerator{};
    val::Texture* grass;
    val::StorageBuffer* vertexOutput;
    val::StorageBuffer* computeGlobalData;


   public:
    TerrainRenderer(val::Engine& engine, val::BufferWriter& writer);

    void renderPass(val::Texture* depth, val::Texture* framebuffer,
                    const CameraData& cam, val::CommandBuffer& cmd);
};
}  // namespace engine
