//
// Created by Greg Costello on 11/14/24.
//

#ifndef BUFFKINZRENDERER_VULKANPOOLMANAGER_H
#define BUFFKINZRENDERER_VULKANPOOLMANAGER_H

#include <vulkan/vulkan_core.h>
#include "VulkanDeviceManager.h"

class VulkanPoolManager {
public:
    VkCommandPool commandPool;

    void setDeviceManager(VulkanDeviceManager *deviceManager) {
        vulkanDeviceManager = deviceManager;
    }
    void createCommandPool();
private:
    VulkanDeviceManager *vulkanDeviceManager;
};


#endif //BUFFKINZRENDERER_VULKANPOOLMANAGER_H
