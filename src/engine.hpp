#pragma once
#include <SDL3/SDL.h>

#include "binding.hpp"
#include "commands.hpp"
#include "gpu_resources.hpp"
#include "memory.hpp"
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
    friend class BufferWriter;
    // types

    struct DeletionQueue {
        std::vector<Texture> textures;
        std::vector<StorageBuffer> buffers;
        std::vector<Mesh> meshes;
        std::vector<raii::Buffer> rawBuffers;

        void clear() {
            textures.clear();
            buffers.clear();
            meshes.clear();
            rawBuffers.clear();
        }
    };

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

        DeletionQueue deletionQueue;
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

    GlobalBinding bindings;

    Pool<StorageBuffer, 4096> bufferPool;
    Pool<Texture, 4096> texturePool;
    Pool<Mesh, 4096> meshPool;

    BufferWriter bufferWriter{*this};

    DeletionQueue deletionQueue;

    void initWindow();
    void initVulkan();
    void reloadSwapchain();
    void initFrameData();

   public:
    Engine(const EngineInitConfig& initConfig);
    ~Engine();

    bool shouldClose() { return _shouldClose; }

    void update();

    CommandBuffer initFrame();

    void submitFrame(Texture* backbuffer);

    Texture* createTexture(Size size, TextureFormat format,
                           TextureSampler sampling = TextureSampler::NEAREST,
                           VkImageUsageFlags usage = 0);
    void freeTexture(Texture* t) {
        deletionQueue.textures.push_back(std::move(*t));
        texturePool.destroy(t);
    }

    void writeImage(Texture* texture, void* data) {
        int size = texture->size.w * texture->size.h *
                   (texture->format == TextureFormat::RGBA16 ? 8 : 4);

        bufferWriter.enqueueTextureWrite(texture, data, size);
    }
};
