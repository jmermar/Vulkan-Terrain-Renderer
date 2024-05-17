#include "engine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

#include "types.hpp"
namespace engine {
class Window : public val::PresentationProvider {
   private:
    SDL_Window* window{};

   public:
    Window(Size size, const char* name) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        window = SDL_CreateWindow(name, size.w, size.h, SDL_WINDOW_VULKAN);
    }
    ~Window() { SDL_DestroyWindow(window); }

    operator SDL_Window*() { return window; }

    VkSurfaceKHR getSurface(VkInstance instance) override {
        VkSurfaceKHR sur;
        SDL_Vulkan_CreateSurface(window, instance, nullptr, &sur);
        return sur;
    }

    Size getSize() override {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return {(uint32_t)w, (uint32_t)h};
    }
};

void Camera::rotateX(float degrees) {
    auto angle = glm::radians(degrees / 2.f);
    glm::quat rotation(glm::cos(angle), glm::vec3(0, 1, 0) * glm::sin(angle));
    glm::quat rotationC = glm::conjugate(rotation);

    dir = rotation * dir * rotationC;
}
void Camera::rotateY(float degrees) {
    auto angle = glm::radians(degrees / 2.f);
    glm::quat rotation(glm::cos(angle),
                       glm::cross(dir, glm::vec3(0, 1, 0)) * glm::sin(angle));
    glm::quat rotationC = glm::conjugate(rotation);

    dir = rotation * dir * rotationC;
}

glm::mat4 Camera::getView() {
    return glm::lookAt(position, dir + position, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjection() {
    auto ret = glm::perspective(glm::radians(fov), w / h, 0.1f, 100000.f);
    ret[1][1] *= -1;
    return ret;
}

Engine::Engine(const RendererConfig& config) {
    Size winSize = {.w = config.width, .h = config.height};
    presentation = std::make_unique<Window>(winSize, "Vulkan Terrain");
    val::EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = val::PresentationFormat::Mailbox;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};

    engine = std::make_unique<val::Engine>(initConfig, presentation.get());
    writer = std::make_unique<val::BufferWriter>(*engine);
    terrainRenderer = std::make_unique<TerrainRenderer>(*engine, *writer);

    frameBuffer =
        engine->createTexture({1920, 1080}, val::TextureFormat::RGBA16);
    depthBuffer =
        engine->createTexture({1920, 1080}, val::TextureFormat::DEPTH32);
    engine->createMesh(4, 1);
}

Engine::~Engine() { engine->waitFinishAllCommands(); }

void Engine::updateInput() {
    auto winSize = presentation->getSize();
    glm::vec2 center(winSize.w / 2.f, winSize.h / 2.f);
    if (mouseCaputure.capture) {
        SDL_SetRelativeMouseMode(true);
        glm::vec2 mousePos;
        SDL_GetMouseState(&mousePos.x, &mousePos.y);

        mouseCaputure.mouseDelta =
            (center - mousePos) / glm::vec2((float)winSize.w, (float)winSize.h);

        SDL_WarpMouseInWindow(*presentation, center.x, center.y);
    } else {
        SDL_SetRelativeMouseMode(false);
        mouseCaputure.mouseDelta = {};
    }
    for (auto& [scan, key] : keyStates) {
        if (key == KeyState::PRESSED) key = KeyState::HOLD;
    }
    SDL_Event event;
    SDL_Scancode key;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                _shouldClose = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                key = event.key.keysym.scancode;
                if (getKeyState(key) == KeyState::RELEASED) {
                    keyStates[key] = KeyState::PRESSED;
                }
                break;

            case SDL_EVENT_KEY_UP:
                key = event.key.keysym.scancode;
                keyStates.erase(key);
                break;
        }
    }
}

bool Engine::shouldClose() { return _shouldClose; }

void Engine::render(Camera& camera) {
    engine->update();

    auto cmd = engine->initFrame();

    CameraData cd;
    cd.pos = camera.position;
    cd.up = glm::vec3(0, 1, 0);
    cd.dir = camera.dir;
    cd.proj = camera.getProjection();
    cd.view = camera.getView();

    if (cmd.isValid()) {
        writer->updateWrites(cmd);

        cmd.transitionTexture(frameBuffer, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        cmd.clearImage(frameBuffer->image, 1, 0.5, 0, 1);
        cmd.transitionTexture(frameBuffer, vk::ImageLayout::eTransferDstOptimal,
                              vk::ImageLayout::eColorAttachmentOptimal);

        terrainRenderer->renderPass(depthBuffer, frameBuffer, cd, cmd);

        engine->submitFrame(frameBuffer);
    }
}
}  // namespace engine
