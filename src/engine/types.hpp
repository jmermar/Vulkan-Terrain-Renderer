#pragma once
#include "../val/vulkan_abstraction.hpp"

namespace engine {
struct CameraData {
    glm::mat4 proj;
    glm::mat4 view;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
};
}  // namespace engine
