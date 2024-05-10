#pragma once
#include <SDL3/SDL.h>

#include "gpu_resources.hpp"
#include "raii.hpp"
#include "types.hpp"
class Window {
   private:
    SDL_Window* window{};
    Size size{};

   public:
    void init(const EngineInitConfig& initConfig) {
        free();
        size = initConfig.screenSize;
        window = SDL_CreateWindow(initConfig.appName, initConfig.screenSize.w,
                                  initConfig.screenSize.h, SDL_WINDOW_VULKAN);
    }
    void free() {
        if (window) {
            SDL_DestroyWindow(window);
            window = 0;
        }
    }

    Window() = default;
    ~Window() { free(); }

    Window(const EngineInitConfig& config) : Window() { init(config); }

    operator SDL_Window*() { return window; }
};

class Engine {
   private:
    // types

    struct Swapchain {
        std::vector<vk::Image> images;
        std::vector<vk::raii::ImageView> imageViews;

        vk::raii::SwapchainKHR swapchain{nullptr};
    };

    struct FrameData {
        vk::raii::CommandPool pool{nullptr};
        vk::raii::CommandBuffer commandBuffer{nullptr};

        vk::raii::Semaphore swapchainSemaphore{nullptr},
            renderSemaphore{nullptr};
        vk::raii::Fence renderFence{nullptr};
    };

    // Info variables
    Size windowSize;
    EngineInitConfig initConfig;
    bool _shouldClose = false;
    uint32_t frameCounter{};
    uint32_t swapchainImageIndex = 0;
    uint32_t imageIndex = 0;
    bool shouldRegenerate = false;

    // System
    Window window;

    // Vulkan Components
    vk::raii::Context ctx;
    vk::raii::Instance instance{nullptr};
    vk::raii::Device device{nullptr};
    vk::raii::DebugUtilsMessengerEXT debug_messenger{nullptr};
    vk::raii::PhysicalDevice chosenGPU{nullptr};
    vk::raii::SurfaceKHR surface{nullptr};

    raii::VMA vma;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    uint32_t graphicsQueueFamily;
    uint32_t presentQueueFamily;

    vk::PhysicalDeviceProperties physicalDeviceProperties;

    Swapchain swapchain;
    FrameData frames[FRAMES_IN_FLIGHT];

    void initWindow();
    void initVulkan();
    void reloadSwapchain();
    void initFrameData();

   public:
    Engine(const EngineInitConfig& initConfig);
    ~Engine();

    bool shouldClose() { return _shouldClose; }

    void update();

    vk::CommandBuffer initFrame();

    vk::Image getFrameImage() { return swapchain.images[imageIndex]; }

    void submitFrame();

    void transitionImage(vk::CommandBuffer cmd, vk::Image image,
                         vk::ImageLayout srcLayout,
                         vk::PipelineStageFlagBits2 srcStage,
                         vk::ImageLayout dstLayout,
                         vk::PipelineStageFlagBits2 dstStage);
};