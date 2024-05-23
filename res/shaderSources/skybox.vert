#version 450
#extension GL_EXT_nonuniform_qualifier : require

#include "globalData.h"

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 coords;

layout(location = 0) out vec3 ocoords;

layout(push_constant) uniform constants {
    mat4 transform;
    uint globalDataBinding;
    uint skyboxTexture;
};

void main() {
    gl_Position = global.proj * global.view * transform * vec4(pos, 1);
    ocoords = pos;
}