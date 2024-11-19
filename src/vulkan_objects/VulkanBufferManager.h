//
// Created by Greg Costello on 11/14/24.
//

#ifndef BUFFKINZRENDERER_VULKANBUFFERMANAGER_H
#define BUFFKINZRENDERER_VULKANBUFFERMANAGER_H

#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapChainManager.h"
#include "VulkanPipelineManager.h"
#include "VulkanPoolManager.h"

class VulkanBufferManager {
public:
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VulkanSwapChainManager *vulkanSwapChainManager;
    // Probably best practice to make all of these object members pointers since there will be a copy everytime... Might need to refactor
    VulkanDeviceManager *vulkanDeviceManager;
    VulkanPipelineManager *vulkanPipelineManager;
    VulkanPoolManager *vulkanPoolManager;
    VkCommandBuffer commandBuffer;

    void createFrameBuffers();
    void createCommandBuffer();
    void setSwapChainManager(VulkanSwapChainManager *swapChainManager) {
        vulkanSwapChainManager = swapChainManager;
    }

    void setDeviceManager(VulkanDeviceManager *vulkanDeviceManager) {
        this->vulkanDeviceManager = vulkanDeviceManager;
    }

    void setPipelineManager(VulkanPipelineManager *vulkanPipelineManager) {
        this->vulkanPipelineManager = vulkanPipelineManager;
    }

    void setPoolManager(VulkanPoolManager *vulkanPoolManager) {
        this->vulkanPoolManager = vulkanPoolManager;
    }
};


#endif //BUFFKINZRENDERER_VULKANBUFFERMANAGER_H
