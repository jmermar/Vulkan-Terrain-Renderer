#pragma once
#include "engine/engine.hpp"

const float WATER_LEVEL = 15;
const float MAX_HEIGHT = 45;

const uint32_t OCTAVES = 4;
const float BASE_FREQUENCY = 1 / 1024.0;
const float FREQ_FACTOR = 3;
const float AMPLITUDE_FACTOR = 0.65;

float getHeight(glm::vec2 pos);