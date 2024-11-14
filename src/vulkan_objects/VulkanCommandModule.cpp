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
}
