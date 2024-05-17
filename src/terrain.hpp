#pragma once
#include "engine.hpp"
#include "pipelines.hpp"
struct TerrainVertexData {
    glm::vec3 pos;
    glm::vec3 color;
};

struct TerrainPushConstants {
    glm::mat4 proj, view, model;
};

struct TerrainChunk {
    Mesh* mesh{};
    glm::vec2 size{};
    glm::vec3 position{};
};

class TerrainRenderer {
   private:
    Engine& engine;
    BufferWriter& writer;
    GraphicsPipeline pass{};
    TerrainChunk chunk{};

    TerrainChunk generateChunk(const glm::vec3& position);

   public:
    TerrainRenderer(Engine& engine, BufferWriter& writer);

    void renderPass(Texture* depth, Texture* framebuffer, const CameraData& cam,
                    CommandBuffer& cmd);
};
