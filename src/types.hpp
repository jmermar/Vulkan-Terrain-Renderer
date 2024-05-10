#pragma once
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

constexpr uint32_t FRAMES_IN_FLIGHT = 2;

struct Size {
    uint32_t w{}, h{};
};

struct Point {
    int32_t x{}, y{};
};

struct Region2D {
    uint32_t w{}, h{};
    uint32_t x{}, y{};
};

struct Rect {
    uint32_t w{}, h{};
    int32_t x{}, y{};
};

enum class PresentationFormat : uint32_t {
    Immediate = VK_PRESENT_MODE_IMMEDIATE_KHR
};

struct EngineInitConfig {
    Size screenSize{};
    PresentationFormat presentation;
    const char* appName{};
    bool useImGUI{};
};

enum class TextureFormat : uint32_t {
    RGBA8 = VK_FORMAT_R8G8B8A8_UNORM,
    RGBA16 = VK_FORMAT_R16G16B16A16_UNORM
};

template <typename T>
using BindPoint = uint32_t;

enum class TextureSampler { NEAREST, LINEAR };