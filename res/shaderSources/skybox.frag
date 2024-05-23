#version 450
#extension GL_EXT_nonuniform_qualifier : require

#include "globalData.h"

layout(location = 0) in vec3 coords;

layout(location = 0) out vec4 color;

layout(push_constant) uniform constants {
    mat4 transform;
    uint globalDataBinding;
    uint skyboxTexture;
};

layout(binding = 0) uniform samplerCube textures[];

void main() { color = texture(textures[skyboxTexture], coords); }