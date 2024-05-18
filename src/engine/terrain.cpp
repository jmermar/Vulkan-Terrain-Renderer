#include "terrain.hpp"

#include <fstream>
#include <glm/ext/matrix_transform.hpp>

namespace engine {
    constexpr uint32_t INVOCATION_SIZE = 256;
void TerrainRenderer::loadTextures()
{
    auto grassData = file::loadImage("textures/grass.png");
    auto snowData = file::loadImage("textures/snow.png");
    auto rock1Data = file::loadImage("textures/rock1.png");
    auto rock2Data = file::loadImage("textures/rock2.png");

    textures.grass = engine.createTexture(grassData.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR, 8);
    textures.snow = engine.createTexture(snowData.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR, 8);
    textures.rock1 = engine.createTexture(rock1Data.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR, 8);
    textures.rock2 = engine.createTexture(rock2Data.size, val::TextureFormat::RGBA8,
                                 val::TextureSampler::LINEAR, 8);

    writer.enqueueTextureWrite(textures.grass, grassData.data.data());
    writer.enqueueTextureWrite(textures.snow, snowData.data.data());
    writer.enqueueTextureWrite(textures.rock1, rock1Data.data.data());
    writer.enqueueTextureWrite(textures.rock2, rock2Data.data.data());
}
void TerrainRenderer::initRenderPass()
{
    
    auto vertShader = file::readBinary("shaders/test.vert.spv");
    auto fragShader = file::readBinary("shaders/test.frag.spv");
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
}
void TerrainRenderer::initCompute()
{
    auto compShader = file::readBinary("shaders/patchGenerator.comp.spv");

    val::ComputePipelineBuilder cpBuild(engine);
    
    vertexOutput = engine.createStorageBuffer(NUM_PATCHES * 4 * sizeof(TerrainVertexData), vk::BufferUsageFlagBits::eVertexBuffer);
    
    patchGenerator = cpBuild.setShader(compShader).setPushConstant<TerrainComputePushConstants>().build();
    computeGlobalData = engine.createStorageBuffer(sizeof(ComputeGlobalData));
}
TerrainRenderer::TerrainRenderer(val::Engine& engine, val::BufferWriter& writer)
    : engine(engine), writer(writer) {
        loadTextures();
        initRenderPass();
        initCompute();
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
    pc.textures[0] = textures.grass->bindPoint;
    pc.textures[1] = textures.snow->bindPoint;
    pc.textures[2] = textures.rock1->bindPoint;
    pc.textures[3] = textures.rock2->bindPoint;
    cmd.bindPipeline(pass);
    cmd.pushConstants(pass, pc);
    cmd.setViewport({0, 0, framebuffer->size.w, framebuffer->size.h});
    cmd.bindVertexBuffer(vertexOutput);
    cmdb.draw(NUM_PATCHES * 4, 1, 0, 0);

    cmd.endPass();
}
}  // namespace engine
