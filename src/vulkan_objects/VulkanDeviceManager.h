//
// Created by Greg Costello on 11/9/24.
//

#ifndef BUFFKINZRENDERER_VULKANDEVICEMANAGER_H
#define BUFFKINZRENDERER_VULKANDEVICEMANAGER_H


#include <vulkan/vulkan_core.h>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanDeviceManager {
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    QueueFamilyIndices indices;

    void pickPhysicalDevice();
    void setInstance(VkInstance &vkInstance);
    void createLogicalDevice();
    void setSurface(VkSurfaceKHR &surface);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

private:
    bool isDeviceSuitable(VkPhysicalDevice device);

    bool checkAvailableExtensions(VkPhysicalDevice &device, std::vector<const char *> const &deviceExtensions);
    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};


#endif //BUFFKINZRENDERER_VULKANDEVICEMANAGER_H
