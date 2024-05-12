#include "engine.hpp"
#include "memory.hpp"
int main() {
    EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = PresentationFormat::Immediate;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};
    Engine engine(initConfig);
    auto texture = engine.createTexture({.w = 2, .h = 2}, TextureFormat::RGBA8);
    while (!engine.shouldClose()) {
        engine.update();

        uint8_t data[4 * 4];
        memset(data, 0, 4 * 4);
        data[0] = 255;
        data[5] = 255;
        data[10] = 255;

        engine.writeImage(texture, data);

        auto cmd = engine.initFrame();

        if (cmd.isValid()) {
            engine.submitFrame(texture);
        }
    }
    return 0;
}
