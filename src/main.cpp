#include "engine.hpp"
int main() {
    EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = PresentationFormat::Immediate;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};
    Engine engine(initConfig);
    while (!engine.shouldClose()) {
        engine.update();

        auto cmd = engine.initFrame();

        if (cmd.isValid()) {
            auto image = engine.getFrameImage();

            cmd.transitionImage(image, vk::ImageLayout::eUndefined,
                                vk::ImageLayout::eTransferDstOptimal);

            cmd.clearImage(image, 1, 0, 0, 0);

            engine.submitFrame();
        }
    }
    return 0;
}