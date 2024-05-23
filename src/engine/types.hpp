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
    val::BindPoint<val::StorageBuffer> globalData;
    glm::vec4 skyColor = glm::vec4(0, 0, 1, 1);
    float fogDensity = 2.5;
    float fogGradient = 0.0004;
    val::BindPoint<val::Texture> skyboxTexture;
};

struct GlobalData {
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 projView;
    glm::mat4 invP;
    glm::vec3 camPos;
    float pad;
    Frustum frustum;
    glm::vec4 skyColor;
    float fogDensity;
    float fogGradient;
    float time;
};
}  // namespace engine
