#pragma once
#include <vector>

#include "gpu_resources.hpp"
class GlobalBinding {
    friend class Engine;

   private:
    vk::Device device{nullptr};
    vk::raii::DescriptorPool pool{nullptr};
    vk::raii::DescriptorSetLayout layout{nullptr};
    vk::raii::DescriptorSet descriptorSet{nullptr};

    vk::raii::Sampler linearSampler{nullptr}, nearestSampler{nullptr};

    std::vector<bool> textureBinds;
    std::vector<bool> storageBinds;

    GlobalBinding() = default;

    void init(const vk::raii::Device& device,
              const vk::PhysicalDeviceProperties& properties);

   public:
    BindPoint<Texture> bindTexture(vk::ImageView texture);
    BindPoint<StorageBuffer> bindStorageBuffer(vk::Buffer* storageBuffer);

    void removeBind(BindPoint<Texture> bindPoint) {
        if (!bindPoint.bind) return;
        textureBinds[bindPoint.bind - 1] = false;
    }
    void removeBind(BindPoint<StorageBuffer> bindPoint) {
        if (!bindPoint.bind) return;
        storageBinds[bindPoint.bind - 1] = false;
    }

    void clearBounds() {
        storageBinds.clear();
        textureBinds.clear();
    }
};