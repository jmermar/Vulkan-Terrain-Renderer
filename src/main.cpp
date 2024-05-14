#include "engine.hpp"
#include "memory.hpp"
int main() {
    EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = PresentationFormat::Mailbox;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};
    Engine engine(initConfig);
    auto texture = engine.createTexture(
        {.w = 256, .h = 256}, TextureFormat::RGBA16, TextureSampler::LINEAR, 4);

    auto buffer = engine.createStorageBuffer(256 * 256 * 4 * 2);
    auto mesh = engine.createMesh(sizeof(float) * 3, 3);

    BufferWriter bufferWriter{engine};
    int i = 0;
    while (!engine.shouldClose()) {
        float a = ((i++) % 100) / 100.f;
        engine.update();

        uint16_t data[4 * 4];
        memset(data, 0, 4 * 4 * sizeof(uint16_t));
        data[0] = UINT16_MAX * a;
        data[5] = UINT16_MAX * (1 - a);
        data[10] = UINT16_MAX * (0.5 + 0.5 * a);

        bufferWriter.enqueueBufferWrite(buffer, data, 0, 4 * 4 * 2);
        float v[] = {0.f, 1.f, 0.f};
        uint32_t i[] = {0, 1, 2};
        bufferWriter.enqueueMeshWrite(mesh, std::span<float>(v),
                                      std::span<uint32_t>(i));

        bufferWriter.enqueueTextureWrite(texture, data);

        auto cmd = engine.initFrame();

        if (cmd.isValid()) {
            bufferWriter.updateWrites(cmd);
            cmd.transitionTexture(texture, vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eTransferDstOptimal);
            cmd.copyToTexture(texture, buffer);
            engine.submitFrame(texture);
        }
    }
    return 0;
}
