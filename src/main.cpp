#include <fstream>
#include <glm/glm.hpp>
#include <stdexcept>

#include "engine.hpp"
#include "pipelines.hpp"
#include "types.hpp"
struct PushConstants {
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 model;
};
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

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
int main() {
    EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = PresentationFormat::Mailbox;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};
    Engine engine(initConfig);
    auto texture = engine.createTexture(
        {.w = 256, .h = 256}, TextureFormat::RGBA16, TextureSampler::LINEAR, 4);

    // Setup pipeline
    auto vertShader = loadShaderBinary("test.vert.spv");
    auto fragShader = loadShaderBinary("test.frag.spv");
    PipelineBuilder builder(engine);
    auto pipeline = builder.setPushConstant<PushConstants>()
                        .addVertexInputAttribute(0, VertexInputFormat::FLOAT3)
                        .addVertexInputAttribute(offsetof(Vertex, color),
                                                 VertexInputFormat::FLOAT3)
                        .addColorAttachment(TextureFormat::RGBA16)
                        .addStage(std::span(vertShader), ShaderStage::VERTEX)
                        .addStage(std::span(fragShader), ShaderStage::FRAGMENT)
                        .setCullMode(PolygonCullMode::NONE)
                        .fillTriangles()
                        .setVertexStride(sizeof(Vertex))
                        .build();

    BufferWriter bufferWriter{engine};

    Vertex vertices[3];
    uint32_t indices[3] = {0, 1, 2};

    vertices[0].pos = {0, -1, 0};
    vertices[1].pos = {-1, 1, 0};
    vertices[2].pos = {1, 1, 0};

    vertices[0].color = {1, 0, 0};
    vertices[1].color = {0, 1, 0};
    vertices[2].color = {0, 0, 1};

    auto mesh = engine.createMesh(sizeof(Vertex) * 3, 3);
    bufferWriter.enqueueMeshWrite(mesh, std::span(vertices, 3),
                                  std::span(indices, 3));
    while (!engine.shouldClose()) {
        engine.update();

        auto cmd = engine.initFrame();

        if (cmd.isValid()) {
            bufferWriter.updateWrites(cmd);

            cmd.beginPass(std::span(&texture, 1));
            auto& cmdb = cmd.cmd;
            PushConstants pc;
            pc.proj = pc.model = pc.view = glm::mat4(1);
            cmd.bindPipeline(pipeline, pc);
            cmd.setViewport({0, 0, 256, 256});

            cmd.bindMesh(mesh);

            cmdb.drawIndexed(3, 1, 0, 0, 0);
            cmd.endPass();

            engine.submitFrame(texture);
        }
    }
    engine.waitFinishAllCommands();
    return 0;
}
