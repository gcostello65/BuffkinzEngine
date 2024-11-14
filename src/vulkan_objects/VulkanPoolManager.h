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
    VulkanDeviceManager *vulkanDeviceManager;

    void createCommandPool();
};


#endif //BUFFKINZRENDERER_VULKANPOOLMANAGER_H
