//
// Created by Greg Costello on 11/9/24.
//

#ifndef BUFFKINZRENDERER_VULKANDEVICEMANAGER_H
#define BUFFKINZRENDERER_VULKANDEVICEMANAGER_H


#include <vulkan/vulkan_core.h>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class VulkanDeviceManager {
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    void pickPhysicalDevice();
    void setInstance(VkInstance &vkInstance);
    void createLogicalDevice();

private:
    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};


#endif //BUFFKINZRENDERER_VULKANDEVICEMANAGER_H
