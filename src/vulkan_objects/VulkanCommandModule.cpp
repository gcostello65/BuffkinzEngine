//
// Created by Greg Costello on 11/14/24.
//

#include "VulkanCommandModule.h"

void VulkanCommandModule::recordCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};

    // Look into the flags that can show how the command buffer will be used
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanPipelineManager->renderPass;
    renderPassInfo.framebuffer = vulkanBufferManager->swapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vulkanSwapChainManager->swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipelineManager->graphicsPipeline);

    // Need to set the viewport and the scissor since we decided they would be dynamic. This goes for anything you set as dynamic.
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vulkanSwapChainManager->swapChainExtent.width);
    viewport.height = static_cast<float>(vulkanSwapChainManager->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vulkanSwapChainManager->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
