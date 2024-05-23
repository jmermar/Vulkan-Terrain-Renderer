#version 450
#extension GL_EXT_nonuniform_qualifier : require
#include "globalData.h"

layout(location = 0) in vec3 viewPos;
layout(location = 1) in vec3 worldPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D textures[];
layout(binding = 0) uniform samplerCube arrayTextures[];

layout(push_constant) uniform constants {
    vec4 color;
    uint globalDataBinding;
    uint screenTexture;
    uint depthTexture;
    uint dudv;
    uint skybox;
};

const float stepF = 0.5;
const float minRayStep = 0.1;
const int maxSteps = 60;
const float searchDist = 5;
const int numBinarySearchSteps = 5;
const float biased = 0.005;

const vec3 planes[6] = {
    vec3(0, 1, 0), vec3(0, -1, 0), vec3(0, 0, -1),
    vec3(0, 0, 1), vec3(-1, 0, 0), vec3(1, 0, 0),
};

vec2 rayCast(vec3 position, vec3 reflection);

vec4 getSSRColor(vec3 dir, vec4 defColor) {
    vec3 hitPos = viewPos;
    float dDepth;
    vec4 coords = vec4(rayCast(hitPos, dir), 0, 0);

    vec2 dudvOff =
        (texture(textures[dudv],
                 vec2(global.time) * 0.05 + vec2(worldPos.x, worldPos.z) * 0.1)
                 .rg *
             2 -
         1) *
        0.01;

    coords += vec4(dudvOff, 0, 0);

    float useTex = step(0, coords.x) * step(-1, -coords.x) * step(0, coords.y) *
                   step(-1, -coords.y);

    return texture(textures[screenTexture], coords.xy) * useTex +
           defColor * (1 - useTex);
}

vec4 getSSRColor(vec3 dir, vec3 worldDir) {
    vec3 hitPos = viewPos;
    float dDepth;
    vec4 coords = vec4(rayCast(hitPos, dir), 0, 0);

    vec2 dudvOff =
        (texture(textures[dudv],
                 vec2(global.time) * 0.05 + vec2(worldPos.x, worldPos.z) * 0.1)
                 .rg *
             2 -
         1) *
        0.01;

    coords += vec4(dudvOff, 0, 0);

    float useTex = step(0, coords.x) * step(-1, -coords.x) * step(0, coords.y) *
                   step(-1, -coords.y);

    vec4 defColor = texture(arrayTextures[skybox], worldDir);

    return texture(textures[screenTexture], coords.xy) * useTex +
           defColor * (1 - useTex);
}

void main() {
    vec3 normal = (global.view * vec4(0, 1, 0, 0)).xyz;

    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(normal)));
    vec3 refracted = normalize(viewPos);

    vec3 worldRef =
        normalize(reflect(normalize(worldPos - global.camPos), vec3(0, 1, 0)));

    float refractFactor = abs(dot(normalize(viewPos), normalize(normal)));

    outColor = getSSRColor(reflected, worldRef) * (1 - refractFactor) +
               getSSRColor(refracted, vec4(0)) * refractFactor;
}

vec2 generateProjectedPosition(vec3 pos) {
    vec4 samplePosition = global.proj * vec4(pos, 1.f);
    samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
    return samplePosition.xy;
}

vec3 generatePositionFromDepth(vec2 texturePos, float depth) {
    vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
    vec4 inversed = global.invP * ndc;
    inversed /= inversed.w;
    return inversed.xyz;
}

vec2 rayCast(vec3 position, vec3 reflection) {
    vec3 step = stepF * reflection;
    vec3 marchingPosition = position + step;
    float delta;
    float depthFromScreen;
    vec2 screenPosition;

    int i = 0;
    for (; i < maxSteps; i++) {
        screenPosition = generateProjectedPosition(marchingPosition);
        depthFromScreen =
            abs(generatePositionFromDepth(
                    screenPosition,
                    texture(textures[depthTexture], screenPosition).x)
                    .z);
        delta = abs(marchingPosition.z) - depthFromScreen;
        if (abs(delta) < biased) {
            return screenPosition.xy;
        }

        if (delta > 0) {
            break;
        }
        marchingPosition += step;
    }

    for (; i < maxSteps; i++) {
        step *= 0.5;
        marchingPosition = marchingPosition - step * sign(delta);

        screenPosition = generateProjectedPosition(marchingPosition);
        depthFromScreen =
            abs(generatePositionFromDepth(
                    screenPosition,
                    texture(textures[depthTexture], screenPosition).x)
                    .z);
        delta = abs(marchingPosition.z) - depthFromScreen;

        if (abs(delta) < biased) {
            vec3 color = vec3(1);
            return screenPosition.xy;
        }
    }

    screenPosition = generateProjectedPosition(marchingPosition);
    return vec2(-1);
}