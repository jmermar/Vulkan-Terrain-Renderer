#include "terrain.hpp"

#include <fstream>
#include <glm/ext/matrix_transform.hpp>

std::vector<uint8_t> loadShaderBinary(const std::string& filePath) {
    std::ifstream file(std::string(RESPATH) + "shaders/" + filePath,
                       std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot load shader " + filePath);
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);

    file.read((char*)buffer.data(), fileSize);
    return buffer;
}

TerrainRenderer::TerrainRenderer(Engine& engine, BufferWriter& writer)
    : engine(engine), writer(writer) {
    auto vertShader = loadShaderBinary("test.vert.spv");
    auto fragShader = loadShaderBinary("test.frag.spv");

    PipelineBuilder pBuild(engine);
    pass = pBuild.setPushConstant<TerrainPushConstants>()
               .addVertexInputAttribute(0, VertexInputFormat::FLOAT3)
               .addVertexInputAttribute(offsetof(TerrainVertexData, color),
                                        VertexInputFormat::FLOAT3)
               .addColorAttachment(TextureFormat::RGBA16)
               .addStage(std::span(vertShader), ShaderStage::VERTEX)
               .addStage(std::span(fragShader), ShaderStage::FRAGMENT)
               .setCullMode(PolygonCullMode::CW)
               .depthTestReadWrite()
               .fillTriangles()
               .setVertexStride(sizeof(TerrainVertexData))
               .build();

    chunk = generateChunk(glm::vec3(0));
}

TerrainChunk TerrainRenderer::generateChunk(const glm::vec3& p) {
    const size_t resolution = 4096;
    TerrainChunk chunk;
    chunk.position = p;
    std::vector<TerrainVertexData> vertices(resolution * resolution);
    int i = 0;
    for (float x = 0; x < resolution; x += 1) {
        for (float y = 0; y < resolution; y += 1) {
            float ux = (float)x / (resolution - 1);
            float uy = (float)y / (resolution - 1);
            vertices[i].pos = glm::vec3(ux, 0, uy);
            vertices[i].color = glm::vec3(ux, uy, 0);
            i++;
        }
    }
    std::vector<uint32_t> indices((resolution - 1) * (resolution - 1) * 6);
    for (uint32_t x = 0; x < resolution - 1; x++) {
        for (uint32_t y = 0; y < resolution - 1; y++) {
            uint32_t base = (x * (resolution - 1) + y) * 6;
            indices[base] = y * resolution + x;
            indices[base + 1] = (y + 1) * resolution + x + 1;
            indices[base + 2] = y * resolution + x + 1;

            indices[base + 3] = y * resolution + x;
            indices[base + 4] = (y + 1) * resolution + x;
            indices[base + 5] = (y + 1) * resolution + x + 1;
        }
    }

    chunk.mesh = engine.createMesh(vertices.size() * sizeof(TerrainVertexData),
                                   indices.size());
    chunk.size = glm::vec2(200, 200);

    writer.enqueueMeshWrite(chunk.mesh, std::span(vertices),
                            std::span(indices));
    return chunk;
}

void TerrainRenderer::renderPass(Texture* depth, Texture* framebuffer,
                                 const CameraData& cam, CommandBuffer& cmd) {
    cmd.beginPass(std::span(&framebuffer, 1), depth, true);
    // auto& cmdb = cmd.cmd;
    TerrainPushConstants pc;
    pc.proj = cam.proj;
    pc.view = cam.view;
    pc.model =
        glm::translate(glm::mat4(1), chunk.position) *
        glm::scale(glm::mat4(1), glm::vec3(chunk.size.x, 1, chunk.size.y));
    cmd.bindPipeline(pass);
    cmd.pushConstants(pass, pc);
    cmd.setViewport({0, 0, framebuffer->size.w, framebuffer->size.h});

    auto cmdb = cmd.cmd;
    cmd.bindMesh(chunk.mesh);
    cmdb.drawIndexed(chunk.mesh->indicesCount, 1, 0, 0, 0);

    cmd.endPass();
}
