#include "renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

#include "engine.hpp"
#include "gpu_resources.hpp"
#include "types.hpp"

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

Renderer::Renderer(const RendererConfig& config) {
    EngineInitConfig initConfig;
    initConfig.appName = "Vulkan Terrain";
    initConfig.presentation = PresentationFormat::Mailbox;
    initConfig.useImGUI = false;
    initConfig.screenSize = {.w = 1920, .h = 1080};

    engine = std::make_unique<Engine>(initConfig);
    writer = std::make_unique<BufferWriter>(*engine);
    terrainRenderer = std::make_unique<TerrainRenderer>(*engine, *writer);

    frameBuffer = engine->createTexture({1920, 1080}, TextureFormat::RGBA16);
    depthBuffer = engine->createTexture({1920, 1080}, TextureFormat::DEPTH32);
    engine->createMesh(4, 1);
}

Renderer::~Renderer() { engine->waitFinishAllCommands(); }

void Renderer::updateInput() {
    SDL_Window* window = engine->getWindow();
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glm::vec2 center(w / 2.f, h / 2.f);
    if (mouseCaputure.capture) {
        SDL_SetRelativeMouseMode(true);
        glm::vec2 mousePos;
        SDL_GetMouseState(&mousePos.x, &mousePos.y);

        mouseCaputure.mouseDelta =
            (center - mousePos) / glm::vec2((float)w, (float)h);

        SDL_WarpMouseInWindow(window, center.x, center.y);
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

bool Renderer::shouldClose() { return _shouldClose; }

void Renderer::render(Camera& camera) {
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
