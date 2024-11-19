//
// Created by Greg Costello on 11/14/24.
//

#ifndef BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
#define BUFFKINZRENDERER_VULKANCOMMANDMODULE_H

#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include "VulkanPipelineManager.h"
#include "VulkanBufferManager.h"

class VulkanCommandModule {
public:
    void setPipelineManager(VulkanPipelineManager *pipelineManager) {
        vulkanPipelineManager = pipelineManager;
    }

    void setBufferManager(VulkanBufferManager *bufferManager) {
        vulkanBufferManager = bufferManager;
    }

    void setSwapChainManager(VulkanSwapChainManager *swapChainManager) {
        vulkanSwapChainManager = swapChainManager;
    }
    void recordCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex);
private:
    VulkanPipelineManager *vulkanPipelineManager;
    VulkanBufferManager *vulkanBufferManager;
    VulkanSwapChainManager *vulkanSwapChainManager;
};


#endif //BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
