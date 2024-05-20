#include <iostream>

#include "engine/engine.hpp"

void drawGUI(engine::Engine& engine) {
    auto isTrue = true;
    static auto frustum = false;
    ImGui::SetNextWindowPos(ImVec2(16, 16));
    ImGui::Begin(
        "vkRaster", &isTrue,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration);

    ImGui::Text("FPS: %f\n", 1.f / engine.getDeltaTime());

    ImGui::Checkbox("Frustum culling", &frustum);
    engine.state.frustum = frustum;

    ImGui::End();
}

int main() {
    engine::Engine engine({1920, 1080}, drawGUI);
    engine::Camera cam;
    cam.position.y = 120;
    cam.dir = {0, 0, 1};
    float moveSpeed = 100.f;
    float sen = 20.f;
    bool capture = false;
    while (!engine.shouldClose()) {
        engine.updateInput();

        auto deltaTime = engine.getDeltaTime();

        if (engine.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
            capture ? engine.captureMouse() : engine.stopCaptureMouse();
            capture = !capture;
        }
        cam.rotateX(engine.getMouseDelta().x * sen);
        cam.rotateY(engine.getMouseDelta().y * sen);

        if (engine.isKeyDown(SDL_SCANCODE_W)) {
            cam.position += cam.dir * deltaTime * moveSpeed;
        }

        if (engine.isKeyDown(SDL_SCANCODE_S)) {
            cam.position -= cam.dir * deltaTime * moveSpeed;
        }

        if (engine.isKeyDown(SDL_SCANCODE_A)) {
            cam.position +=
                glm::cross(glm::vec3(0, 1, 0), cam.dir) * deltaTime * moveSpeed;
        }

        if (engine.isKeyDown(SDL_SCANCODE_D)) {
            cam.position -=
                glm::cross(glm::vec3(0, 1, 0), cam.dir) * deltaTime * moveSpeed;
        }
        engine.render(cam);
    }
    return 0;
}
