#version 450
#extension GL_EXT_nonuniform_qualifier : require
#include "globalData.h"
#include "terrainGeneration.h"

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 uv;
layout(Location = 2) in vec4 worldPos;
layout(location = 3) in float visibility;

layout(push_constant) uniform constants {
    uint globalDataBinding;
    uint grassBind;
    uint snowBind;
    uint rock1Bind;
    uint rock2Bind;
    uint skybox;
};

layout(binding = 0) uniform sampler2D textures[];
layout(binding = 0) uniform samplerCube skyboxTextures[];

float contribution(float val, float minval, float maxval) {
    return clamp(((val - minval) / (maxval - minval)), 0, 1);
}

vec4 getTextColor() {
    float H = worldPos.y;
    vec4 grass = texture(textures[grassBind], uv);
    vec4 snow = texture(textures[snowBind], uv);
    vec4 rock1 = texture(textures[rock1Bind], uv * 2);
    vec4 rock2 = texture(textures[rock2Bind], uv);

    return mix(rock2,
               mix(grass, rock1,
                   contribution(H, GRASS_MAX_LEVEL - 1, GRASS_MAX_LEVEL)),
               contribution(H, WATER_LEVEL - 1, WATER_LEVEL));
}

void main() {
    outColor =
        mix(texture(skyboxTextures[skybox], worldPos.xyz - global.camPos),
            getTextColor() * clamp(dot(normal, normalize(lightDir)), 0.6, 1),
            visibility);
}
