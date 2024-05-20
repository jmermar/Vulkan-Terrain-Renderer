#pragma once
#include "../val/vulkan_abstraction.hpp"

namespace engine {

struct Frustum {
    glm::vec4 left, right, top, bottom, front, back;
};

struct CameraData {
    glm::mat4 proj;
    glm::mat4 view;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
    Frustum frustum;
};

struct RenderState {
    CameraData cam;
    bool frustum;
};
}  // namespace engine
