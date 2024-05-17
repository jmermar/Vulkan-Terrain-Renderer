#pragma once
#include <cstdint>
#include <unordered_map>

#include "../val/vulkan_abstraction.hpp"
#include "terrain.hpp"

namespace engine {

class Window;
struct RendererConfig {
    uint32_t width;
    uint32_t height;
};
struct Camera {
    float w{1}, h{1};
    float fov{90};
    glm::vec3 position{};
    glm::vec3 dir{0, 0, 1};

    void rotateX(float degrees);
    void rotateY(float degreess);

    glm::mat4 getProjection();
    glm::mat4 getView();
};

enum class KeyState { PRESSED, RELEASED, HOLD };

class Engine {
   private:
    std::unique_ptr<Window> presentation;
    std::unique_ptr<val::Engine> engine;
    std::unique_ptr<val::BufferWriter> writer;
    std::unique_ptr<TerrainRenderer> terrainRenderer;

    std::unordered_map<SDL_Scancode, KeyState> keyStates;

    val::Texture* frameBuffer;
    val::Texture* depthBuffer;

    bool _shouldClose = false;

    struct {
        glm::vec2 mouseDelta;
        bool capture = false;
    } mouseCaputure;

   public:
    Engine(const RendererConfig& config);
    ~Engine();

    KeyState getKeyState(SDL_Scancode key) {
        if (keyStates.contains(key)) return keyStates[key];
        return KeyState::RELEASED;
    }

    bool isKeyDown(SDL_Scancode key) {
        return getKeyState(key) != KeyState::RELEASED;
    }

    bool isKeyPressed(SDL_Scancode key) {
        return getKeyState(key) == KeyState::PRESSED;
    }

    void captureMouse() { mouseCaputure.capture = true; }
    void stopCaptureMouse() { mouseCaputure.capture = false; }

    glm::vec2 getMouseDelta() { return mouseCaputure.mouseDelta; }

    void updateInput();

    bool shouldClose();
    void render(Camera& camera);
};
}  // namespace engine