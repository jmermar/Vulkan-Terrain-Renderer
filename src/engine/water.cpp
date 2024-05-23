#include "water.hpp"

constexpr uint32_t MAX_PATCH = 200;
constexpr uint32_t NUM_PATCHES = MAX_PATCH * MAX_PATCH;

constexpr uint32_t INVOCATION_SIZE = 256;

namespace engine {

struct WaterVertexData {
    glm::vec4 pos;
};

struct DrawIndirectCommand {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
};

struct WaterPushConstants {
    glm::vec4 waterColor;
    val::BindPoint<val::StorageBuffer> globalDataBind;
    val::BindPoint<val::Texture> screenTexture;
    val::BindPoint<val::Texture> depthTexture;
    val::BindPoint<val::Texture> dudv;
    val::BindPoint<val::Texture> skyboxTexture;
};

void WaterRenderer::loadTextures() {
    auto dudvImage = file::loadImage("textures/dudv.png");
    dudvMap = engine.createTexture(dudvImage.size, val::TextureFormat::RGBA8);
    writer.enqueueTextureWrite(dudvMap, dudvImage.data.data());
}

void WaterRenderer::initRenderPass() {
    auto vertShader = file::readBinary("shaders/water.vert.spv");
    auto fragShader = file::readBinary("shaders/water.frag.spv");
    val::PipelineBuilder builder(engine);
    pass = builder.setPushConstant<WaterPushConstants>()
               .addVertexInputAttribute(0, val::VertexInputFormat::FLOAT4)
               .addColorAttachment(val::TextureFormat::RGBA16)
               .addStage(std::span(vertShader), val::ShaderStage::VERTEX)
               .addStage(std::span(fragShader), val::ShaderStage::FRAGMENT)
               .setCullMode(val::PolygonCullMode::CW)
               .fillTriangles()
               .depthTestRead()
               .setVertexStride(sizeof(WaterVertexData))
               .build();

    vertices =
        engine.createStorageBuffer(NUM_PATCHES * 6 * sizeof(WaterVertexData),
                                   vk::BufferUsageFlagBits::eVertexBuffer);

    drawCommand = engine.createStorageBuffer(
        sizeof(DrawIndirectCommand), vk::BufferUsageFlagBits::eIndirectBuffer);
}

WaterRenderer::WaterRenderer(val::Engine& engine, val::BufferWriter& writer)
    : engine(engine), writer(writer) {
    loadTextures();
    initRenderPass();
}

void WaterRenderer::renderPass(val::Texture* depth, val::Texture* framebuffer,
                               val::Texture* screen, const RenderState& rs,
                               val::CommandBuffer& cmd) {
    auto cmdb = cmd.cmd;

    cmd.beginPass(std::span(&framebuffer, 1), depth);
    WaterPushConstants pc;
    pc.globalDataBind = rs.globalData;
    pc.waterColor = rs.skyColor;
    pc.screenTexture = screen->bindPoint;
    pc.depthTexture = depth->bindPoint;
    pc.dudv = dudvMap->bindPoint;
    pc.skyboxTexture = rs.skyboxTexture;
    cmd.bindPipeline(pass);
    cmd.pushConstants(pass, pc);
    cmd.setViewport({0, 0, framebuffer->size.w, framebuffer->size.h});
    cmd.bindVertexBuffer(vertices);
    cmdb.drawIndirect(drawCommand->buffer, 0, 1, sizeof(DrawIndirectCommand));

    cmd.endPass();
}
}  // namespace engine