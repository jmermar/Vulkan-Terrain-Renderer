#include "utils.hpp"

glm::vec3 permute(glm::vec3 x) {
    return glm::mod(((x * 34.f) + 1.f) * x, 289.f);
}

float snoise(glm::vec2 v) {
    const glm::vec4 C = glm::vec4(0.211324865405187, 0.366025403784439,
                                  -0.577350269189626, 0.024390243902439);
    glm::vec2 i = glm::floor(v + glm::dot(v, glm::vec2(C.y)));
    glm::vec2 x0 = v - i + glm::dot(i, glm::vec2(C.x));
    glm::vec2 i1;
    i1 = (x0.x > x0.y) ? glm::vec2(1.0, 0.0) : glm::vec2(0.0, 1.0);
    glm::vec4 x12 =
        glm::vec4(x0.x, x0.y, x0.x, x0.y) + glm::vec4(C.x, C.x, C.z, C.z);
    x12.x -= i1.x;
    x12.y -= i1.y;
    i = glm::mod(i, 289.f);
    glm::vec3 p = permute(permute(i.y + glm::vec3(0.f, i1.y, 1.f)) + i.x +
                          glm::vec3(0.f, i1.x, 1.f));
    glm::vec3 m = glm::max(
        0.5f - glm::vec3(
                   glm::dot(x0, x0),
                   glm::dot(glm::vec2(x12.x, x12.y), glm::vec2(x12.x, x12.y)),
                   glm::dot(glm::vec2(x12.z, x12.w), glm::vec2(x12.z, x12.w))),
        0.f);
    m = m * m;
    m = m * m;
    glm::vec3 x = 2.f * glm::fract(p * glm::vec3(C.w)) - 1.f;
    glm::vec3 h = glm::abs(x) - 0.5f;
    glm::vec3 ox = glm::floor(x + 0.5f);
    glm::vec3 a0 = x - ox;
    m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);
    glm::vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.y = a0.y * x12.x + h.y * x12.y;
    g.z = a0.z * x12.z + h.z * x12.w;
    return 130.0 * dot(m, g);
}

float getHeight(glm::vec2 v) {
    float f = BASE_FREQUENCY;
    float amplitude = 1;
    float ampAcc = 0;
    float noiseAcc = 0;
    for (int i = 0; i < OCTAVES; i++) {
        noiseAcc += (snoise(v * f) * 0.5 + 0.5) * amplitude;
        ampAcc += amplitude;
        f *= FREQ_FACTOR;
        amplitude *= AMPLITUDE_FACTOR;
    }

    float ret = (noiseAcc / ampAcc) * MAX_HEIGHT;

    return ret;
}