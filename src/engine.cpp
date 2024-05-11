#include "engine.hpp"

#include <VkBootstrap.h>

#include "SDL3/SDL_vulkan.h"
void Engine::initWindow() { window.init(initConfig); }

void Engine::initVulkan() {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
                        .request_validation_layers(true)
                        .use_default_debug_messenger()
                        .require_api_version(1, 3, 0)
                        .build();

    vkb::Instance vkb_inst = inst_ret.value();
    instance = vk::raii::Instance(ctx, vkb_inst.instance, nullptr);
    debug_messenger =
        vk::raii::DebugUtilsMessengerEXT(instance, vkb_inst.debug_messenger);

    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window, vkb_inst.instance, nullptr, &surface);
    this->surface = vk::raii::SurfaceKHR(instance, surface);

    vk::PhysicalDeviceVulkan13Features features{};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    vk::PhysicalDeviceVulkan12Features features12{};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;
    features12.runtimeDescriptorArray = true;
    features12.descriptorBindingPartiallyBound = true;
    features12.shaderSampledImageArrayNonUniformIndexing = true;
    features12.descriptorBindingSampledImageUpdateAfterBind = true;
    features12.shaderUniformBufferArrayNonUniformIndexing = true;
    features12.descriptorBindingUniformBufferUpdateAfterBind = true;
    features12.shaderStorageBufferArrayNonUniformIndexing = true;
    features12.descriptorBindingStorageBufferUpdateAfterBind = true;
    features12.drawIndirectCount = true;

    vk::PhysicalDeviceFeatures features10{};
    features10.multiDrawIndirect = true;

    vkb::PhysicalDeviceSelector selector{vkb_inst};
    vkb::PhysicalDevice physicalDevice =
        selector.set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_required_features(features10)
            .set_surface(surface)
            .select()
            .value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};

    vkb::Device vkbDevice = deviceBuilder.build().value();

    chosenGPU = vk::raii::PhysicalDevice(instance, physicalDevice);

    device = vk::raii::Device(chosenGPU, vkbDevice.device);

    graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    presentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();

    graphicsQueueFamily =
        vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    presentQueueFamily =
        vkbDevice.get_queue_index(vkb::QueueType::present).value();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = *chosenGPU;
    allocatorInfo.device = *device;
    allocatorInfo.instance = *instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vma.init(allocatorInfo);

    physicalDeviceProperties = chosenGPU.getProperties();
}

void Engine::reloadSwapchain() {
    swapchain.images.clear();
    swapchain.imageViews.clear();
    auto vkbSwapchain =
        vkb::SwapchainBuilder(*chosenGPU, *device, *surface)
            .set_desired_format(VkSurfaceFormatKHR{
                .format = VK_FORMAT_B8G8R8A8_UNORM,
                .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_present_mode((VkPresentModeKHR)initConfig.presentation)
            .set_desired_extent(windowSize.w, windowSize.h)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

    windowSize.w = vkbSwapchain.extent.width;
    windowSize.h = vkbSwapchain.extent.height;

    swapchain.swapchain =
        vk::raii::SwapchainKHR(device, vkbSwapchain.swapchain);

    swapchain.images.reserve(vkbSwapchain.image_count);
    swapchain.imageViews.reserve(vkbSwapchain.image_count);

    auto images = vkbSwapchain.get_images().value();
    auto imageViews = vkbSwapchain.get_image_views().value();

    for (size_t i = 0; i < vkbSwapchain.image_count; i++) {
        swapchain.images.push_back(images[i]);
        swapchain.imageViews.push_back(
            vk::raii::ImageView(device, imageViews[i]));
    }
}

void Engine::initFrameData() {
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        auto& frame = frames[i];

        vk::CommandPoolCreateInfo commandPoolInfo{};

        commandPoolInfo.flags =
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolInfo.queueFamilyIndex = graphicsQueueFamily;

        frame.pool = vk::raii::CommandPool(device, commandPoolInfo);

        vk::CommandBufferAllocateInfo cmdAllocInfo;
        cmdAllocInfo.commandPool = frame.pool;
        cmdAllocInfo.commandBufferCount = 1;
        cmdAllocInfo.level = vk::CommandBufferLevel::ePrimary;

        frame.commandBuffer =
            std::move(device.allocateCommandBuffers(cmdAllocInfo)[0]);

        vk::FenceCreateInfo fenceCreate;
        fenceCreate.flags = vk::FenceCreateFlagBits::eSignaled;
        frame.renderFence = device.createFence(fenceCreate);

        vk::SemaphoreCreateInfo semaphoreCreate;
        frame.swapchainSemaphore = device.createSemaphore(semaphoreCreate);
        frame.renderSemaphore = device.createSemaphore(semaphoreCreate);
    }
}

Engine::Engine(const EngineInitConfig& initConfig) {
    windowSize = initConfig.screenSize;
    this->initConfig = initConfig;
    initWindow();
    initVulkan();
    reloadSwapchain();
    initFrameData();
    bindings.init(device, physicalDeviceProperties);
}

Engine::~Engine() { device.waitIdle(); }

void Engine::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                _shouldClose = true;
                break;
        }
    }
}

CommandBuffer Engine::initFrame() {
    auto& frame = frames[frameCounter % FRAMES_IN_FLIGHT];
    static_cast<void>(
        device.waitForFences({frame.renderFence}, true, 10000000000000));
    device.resetFences({frame.renderFence});

    auto result = swapchain.swapchain.acquireNextImage(
        10000000000000, frame.swapchainSemaphore);

    if (result.first == vk::Result::eErrorOutOfDateKHR) {
        shouldRegenerate = true;
        frameCounter++;
        return vk::CommandBuffer(nullptr);
    }

    imageIndex = result.second;

    auto cmd = CommandBuffer(frame.commandBuffer);
    cmd.begin();
    return cmd;
}

void Engine::submitFrame() {
    auto& frame = frames[frameCounter % FRAMES_IN_FLIGHT];

    auto cmd = CommandBuffer(frame.commandBuffer);

    cmd.transitionImage(swapchain.images[imageIndex],
                        vk::ImageLayout::eUndefined,
                        vk::PipelineStageFlagBits2::eAllCommands,
                        vk::ImageLayout::ePresentSrcKHR,
                        vk::PipelineStageFlagBits2KHR::eAllCommands);

    frame.commandBuffer.end();

    vk::CommandBufferSubmitInfo commandBufferSubmitInfo;
    commandBufferSubmitInfo.commandBuffer = frame.commandBuffer;

    vk::SemaphoreSubmitInfo waitInfo, signalInfo;

    waitInfo.semaphore = frame.swapchainSemaphore;
    waitInfo.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

    signalInfo.semaphore = frame.renderSemaphore;
    signalInfo.stageMask = vk::PipelineStageFlagBits2::eAllGraphics;

    vk::SubmitInfo2 submitInfo;
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitInfo;
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signalInfo;
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

    graphicsQueue.submit2({submitInfo}, frame.renderFence);

    auto sw = *swapchain.swapchain;

    vk::PresentInfoKHR presentInfo;
    presentInfo.swapchainCount = 1;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pSwapchains = &sw;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &(*frame.renderSemaphore);

    auto presentResult = graphicsQueue.presentKHR(presentInfo);

    if (presentResult == vk::Result::eErrorOutOfDateKHR) {
        shouldRegenerate = true;
    }

    frameCounter++;
}
