#pragma once
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>

constexpr uint32_t FRAMES_IN_FLIGHT = 2;

enum class PresentationFormat : uint32_t {
    Immediate = VK_PRESENT_MODE_IMMEDIATE_KHR,
    Mailbox = VK_PRESENT_MODE_MAILBOX_KHR
};

enum class TextureFormat : uint32_t {
    RGBA8 = VK_FORMAT_R8G8B8A8_UNORM,
    RGBA16 = VK_FORMAT_R16G16B16A16_UNORM,
    DEPTH32 = VK_FORMAT_D32_SFLOAT
};

enum class TextureSampler : uint32_t {
    NEAREST = VK_FILTER_NEAREST,
    LINEAR = VK_FILTER_LINEAR
};

enum class ShaderStage : uint32_t {
    VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
    FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT,
    TESSELATION_CONTROL = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TESSELATION_EVALUATION = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
};

enum class VertexInputFormat : uint32_t {
    FLOAT = VK_FORMAT_D32_SFLOAT,
    FLOAT2 = VK_FORMAT_R32G32_SFLOAT,
    FLOAT3 = VK_FORMAT_R32G32B32_SFLOAT,
    FLOAT4 = VK_FORMAT_R32G32B32A32_SFLOAT
};

enum class PolygonCullMode : uint32_t {
    NONE = VK_CULL_MODE_NONE,
    CCW = VK_CULL_MODE_FRONT_BIT,
    CW = VK_CULL_MODE_BACK_BIT
};

struct Size {
    uint32_t w{}, h{};
};

struct Point {
    int32_t x{}, y{};
};

struct Region2D {
    uint32_t x{}, y{};
    uint32_t w{}, h{};
};

struct Rect {
    int32_t x{}, y{};
    uint32_t w{}, h{};
};

struct EngineInitConfig {
    Size screenSize{};
    PresentationFormat presentation;
    const char* appName{};
    bool useImGUI{};
};

template <typename T>
struct BindPoint {
    uint32_t bind;
};
