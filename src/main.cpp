#include "engine/engine.hpp"
#include <iostream>
int main() {
    engine::Engine engine({1920, 1080});
    engine::Camera cam;
    cam.position.y = 120;
    cam.dir = {0, 0, 1};
    float moveSpeed = 100.f;
    float sen = 20.f;
    bool capture = false;
    auto ticks = SDL_GetTicks();
    while (!engine.shouldClose()) {
        engine.updateInput();

        auto deltaTime = (SDL_GetTicks() - ticks) / 1000.f;

        std::cout << "FPS: " << 1 / deltaTime << std::endl;

        ticks = SDL_GetTicks();



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
