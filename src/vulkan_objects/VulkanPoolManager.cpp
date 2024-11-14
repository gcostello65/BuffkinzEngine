//
// Created by Greg Costello on 11/14/24.
//

#include "VulkanPoolManager.h"

void VulkanPoolManager::createCommandPool() {

    // Creating a command pool from which command buffers will be allocated
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vulkanDeviceManager->indices.graphicsFamily.value();

    if (vkCreateCommandPool(vulkanDeviceManager->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
