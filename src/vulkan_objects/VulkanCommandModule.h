//
// Created by Greg Costello on 11/14/24.
//

#ifndef BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
#define BUFFKINZRENDERER_VULKANCOMMANDMODULE_H

#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include "VulkanPipelineManager.h"
#include "VulkanBufferManager.h"
#include "../engine_objects/Scene.h"

class VulkanCommandModule {
public:
    const static int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;

    void createSyncObjects();
    void drawFrame(Scene *scene);
    void setPipelineManager(VulkanPipelineManager *pipelineManager) {
        vulkanPipelineManager = pipelineManager;
    }

    void setBufferManager(VulkanBufferManager *bufferManager) {
        vulkanBufferManager = bufferManager;
    }

    void setSwapChainManager(VulkanSwapChainManager *swapChainManager) {
        vulkanSwapChainManager = swapChainManager;
    }

    void setDeviceManager(VulkanDeviceManager *deviceManager) {
        vulkanDeviceManager = deviceManager;
    }
private:
    VulkanPipelineManager *vulkanPipelineManager;
    VulkanBufferManager *vulkanBufferManager;
    VulkanSwapChainManager *vulkanSwapChainManager;
    VulkanDeviceManager *vulkanDeviceManager;

    void recordCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex, Scene *scene);
};


#endif //BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
