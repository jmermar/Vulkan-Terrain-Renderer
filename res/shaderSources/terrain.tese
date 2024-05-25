#version 450
#extension GL_EXT_nonuniform_qualifier : require
#include "globalData.h"
#include "terrainGeneration.h"

layout(quads, equal_spacing, ccw) in;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec4 worldPos;
layout(location = 3) out float visibility;

const float density = 0.00035;
const float gradient = 4.5;

layout(push_constant) uniform constants {
    uint globalDataBinding;
    uint grassBind;
    uint snowBind;
    uint rock1Bind;
    uint rock2Bind;
};

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    worldPos = (p1 - p0) * v + p0;

    worldPos.y = getHeight(vec2(worldPos.x, worldPos.z));
    vec3 right = vec3(worldPos.x + 0.05, 0, worldPos.z);
    right.y = getHeight(vec2(right.x, right.z));
    vec3 down = vec3(worldPos.x, 0, worldPos.z + 0.05);
    down.y = getHeight(vec2(down.x, down.z));

    normal =
        cross(normalize(right - worldPos.xyz), normalize(down - worldPos.xyz));

    uv.x = worldPos.x / 8;
    uv.y = worldPos.z / 8;

    float disToCamera = length(global.camPos - worldPos.xyz);
    visibility =
        exp(-pow((disToCamera * global.fogDensity), global.fogGradient));

    gl_Position = global.proj * global.view * worldPos;
}
