#version 450
#extension GL_EXT_nonuniform_qualifier : require

#include "globalData.h"

layout(location = 0) in vec4 pos;

layout(location = 0) out vec3 viewPos;
layout(location = 1) out vec3 worldPos;

layout(push_constant) uniform constants {
    vec4 color;
    uint globalDataBinding;
};

void main() {
    viewPos = (global.view * pos).xyz;
    worldPos = pos.xyz;
    gl_Position = global.proj * global.view * pos;
}