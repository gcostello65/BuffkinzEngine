//
// Created by Greg Costello on 11/14/24.
//

#include "VulkanBufferManager.h"

void VulkanBufferManager::createFrameBuffers() {
    // This note goes for all public member references. I know this is not the best practice, but I am trying to iterate quickly to learn vulkan
    // Not working on writing the best c++ code at this point. That will come in a large refactoring pass.
    swapChainFramebuffers.resize(vulkanSwapChainManager.imageViews.size());

    // Creating one frame buffer per swapchain image since we can be rendering to any of the swapchain images and need a frame buffer for each
    // We then pick the frame buffer that has been rendered to in order to display
    for (size_t i = 0; i < vulkanSwapChainManager.imageViews.size(); i++) {
        VkImageView attachments[] = {
                vulkanSwapChainManager.imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vulkanPipelineManager->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = vulkanSwapChainManager.swapChainExtent.width;
        framebufferInfo.height = vulkanSwapChainManager.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkanDeviceManager->device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanBufferManager::createCommandBuffer() {
    // Creating a command buffer from the command pool so that we can record draw commands
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vulkanPoolManager->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vulkanDeviceManager->device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}


