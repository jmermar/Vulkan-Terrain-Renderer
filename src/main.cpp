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
    }
    return 0;
}