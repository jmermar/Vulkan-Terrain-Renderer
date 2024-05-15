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
                        .fillTriangles()
                        .build();

    BufferWriter bufferWriter{engine};
    while (!engine.shouldClose()) {
        engine.update();

        auto cmd = engine.initFrame();

        if (cmd.isValid()) {
            bufferWriter.updateWrites(cmd);
            engine.submitFrame(texture);
        }
    }
    return 0;
}
