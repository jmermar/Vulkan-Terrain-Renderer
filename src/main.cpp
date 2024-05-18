#include "engine/engine.hpp"

int main() {
    engine::Engine engine({1920, 1080});
    engine::Camera cam;
    cam.position.y = 1;
    float deltaTime = 60 / 1000.f;
    float moveSpeed = 4.f;
    float sen = 20.f;
    bool capture = false;
    while (!engine.shouldClose()) {
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

        engine.updateInput();
        engine.render(cam);
    }
    return 0;
}
