//
// Created by Greg Costello on 11/12/24.
//

#ifndef BUFFKINZRENDERER_VULKANPIPELINEMANAGER_H
#define BUFFKINZRENDERER_VULKANPIPELINEMANAGER_H

#include <vulkan/vulkan_core.h>
#include <vector>
#include "VulkanSwapChainManager.h"

class VulkanPipelineManager {
public:
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;

    void createRenderPass();

    void createGraphicsPipeline();

    static std::vector<char> readFile(const std::basic_string<char> &filename);

    VkShaderModule createShaderModule(const std::vector<char> &);

    void setDevice(VkDevice &device) {
        this->device = device;
    }

    void setSwapChainHandle(VulkanSwapChainManager &handle) {
        vulkanSwapchainManager = handle;
    }

private:
    VkDevice device;
    VulkanSwapChainManager vulkanSwapchainManager;
};


#endif //BUFFKINZRENDERER_VULKANPIPELINEMANAGER_H
