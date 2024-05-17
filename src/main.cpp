#include "renderer.hpp"

int main() {
    Renderer renderer({1920, 1080});
    Camera cam;
    cam.position.y = 1;
    float deltaTime = 60 / 1000.f;
    float moveSpeed = 3.f;
    float sen = 20.f;
    while (!renderer.shouldClose()) {
        renderer.captureMouse();

        cam.rotateX(renderer.getMouseDelta().x * sen);
        cam.rotateY(renderer.getMouseDelta().y * sen);

        if (renderer.isKeyDown(SDL_SCANCODE_W)) {
            cam.position += cam.dir * deltaTime * moveSpeed;
        }

        if (renderer.isKeyDown(SDL_SCANCODE_S)) {
            cam.position -= cam.dir * deltaTime * moveSpeed;
        }

        if (renderer.isKeyDown(SDL_SCANCODE_A)) {
            cam.position +=
                glm::cross(glm::vec3(0, 1, 0), cam.dir) * deltaTime * moveSpeed;
        }

        if (renderer.isKeyDown(SDL_SCANCODE_D)) {
            cam.position -=
                glm::cross(glm::vec3(0, 1, 0), cam.dir) * deltaTime * moveSpeed;
        }

        renderer.updateInput();
        renderer.render(cam);
    }
    return 0;
}
