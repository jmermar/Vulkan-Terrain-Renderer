#include "terrain.hpp"

#include <fstream>
#include <glm/ext/matrix_transform.hpp>

namespace engine {
    constexpr uint32_t INVOCATION_SIZE = 256;
TerrainRenderer::TerrainRenderer(val::Engine& engine, val::BufferWriter& writer)
    : engine(engine), writer(writer) {
    auto vertShader = file::readBinary("shaders/test.vert.spv");
    auto fragShader = file::readBinary("shaders/test.frag.spv");
    auto compShader = file::readBinary("shaders/patchGenerator.comp.spv");
    auto teseShader = file::readBinary("shaders/terrain.tese.spv");
    auto tescShader = file::readBinary("shaders/terrain.tesc.spv");

    val::PipelineBuilder pBuild(engine);
    pass = pBuild.setPushConstant<TerrainPushConstants>()
               .addVertexInputAttribute(0, val::VertexInputFormat::FLOAT4)
               .addColorAttachment(val::TextureFormat::RGBA16)
               .addStage(std::span(vertShader), val::ShaderStage::VERTEX)
               .addStage(std::span(fragShader), val::ShaderStage::FRAGMENT)
               .addStage(std::span(teseShader), val::ShaderStage::TESSELATION_EVALUATION)
               .addStage(std::span(tescShader), val::ShaderStage::TESSELATION_CONTROL)
               .setCullMode(val::PolygonCullMode::CW)
               .setTessellation(4)
               .depthTestReadWrite()
               .tessellationFill()
               .setVertexStride(sizeof(TerrainVertexData))
               .build();

    val::ComputePipelineBuilder cpBuild(engine);
     patchGenerator = cpBuild.setShader(compShader).setPushConstant<TerrainComputePushConstants>().build();

    auto texData = file::loadImage("textures/grass.png");
    grass = engine.createTexture(texData.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR, 8);

    vertexOutput = engine.createStorageBuffer(NUM_PATCHES * 4 * sizeof(TerrainVertexData), vk::BufferUsageFlagBits::eVertexBuffer);
    writer.enqueueTextureWrite(grass, texData.data.data());

    computeGlobalData = engine.createStorageBuffer(sizeof(ComputeGlobalData));
}

void TerrainRenderer::renderPass(val::Texture* depth, val::Texture* framebuffer,
                                 const CameraData& cam,
                                 val::CommandBuffer& cmd) {
    cmd.bindPipeline(patchGenerator);

    TerrainComputePushConstants computePushConstants;
    computePushConstants.globalBind = computeGlobalData->bindPoint;
    computePushConstants.patchesBind = vertexOutput->bindPoint;
    computePushConstants.camPos = cam.pos;

    cmd.pushConstants(patchGenerator, computePushConstants);

    auto cmdb = cmd.cmd;

    cmdb.dispatch(NUM_PATCHES / INVOCATION_SIZE, 1, 1);

    cmd.memoryBarrier(vk::PipelineStageFlagBits2::eComputeShader, vk::AccessFlagBits2::eMemoryRead|vk::AccessFlagBits2::eMemoryWrite, vk::PipelineStageFlagBits2::eAllCommands, vk::AccessFlagBits2::eMemoryRead);

    cmd.beginPass(std::span(&framebuffer, 1), depth, true);
    TerrainPushConstants pc;
    pc.proj = cam.proj;
    pc.view = cam.view;
    pc.grassBind = grass->bindPoint;
    cmd.bindPipeline(pass);
    cmd.pushConstants(pass, pc);
    cmd.setViewport({0, 0, framebuffer->size.w, framebuffer->size.h});
    cmd.bindVertexBuffer(vertexOutput);
    cmdb.draw(NUM_PATCHES * 4, 1, 0, 0);

    cmd.endPass();
}
}  // namespace engine
