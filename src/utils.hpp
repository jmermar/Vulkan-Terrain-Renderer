#pragma once
#include "engine/engine.hpp"

const float WATER_LEVEL = 30;
const float MAX_HEIGHT = 60;

const uint32_t OCTAVES = 6;
const float BASE_FREQUENCY = 1 / 256.0;
const float FREQ_FACTOR = 3.5;
const float AMPLITUDE_FACTOR = 0.3;

float getHeight(glm::vec2 pos);