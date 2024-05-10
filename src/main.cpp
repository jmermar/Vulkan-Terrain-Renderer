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

        if (cmd != nullptr) {
            auto image = engine.getFrameImage();

            engine.transitionImage(cmd, image, vk::ImageLayout::eUndefined,
                                   vk::PipelineStageFlagBits2::eAllCommands,
                                   vk::ImageLayout::eTransferDstOptimal,
                                   vk::PipelineStageFlagBits2KHR::eAllCommands);
            vk::ImageSubresourceRange range;
            range.levelCount = vk::RemainingMipLevels;
            range.layerCount = vk::RemainingArrayLayers;
            range.aspectMask = vk::ImageAspectFlagBits::eColor;

            vk::ClearColorValue color;
            color.setFloat32({1.f, 0.f, 0.f, 0.f});
            cmd.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal,
                                color, range);

            engine.submitFrame();
        }
    }
    return 0;
}