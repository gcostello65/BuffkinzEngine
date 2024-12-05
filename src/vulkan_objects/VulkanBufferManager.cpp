//
// Created by Greg Costello on 11/14/24.
//

#include "VulkanBufferManager.h"
#include "../engine_objects/Scene.h"

void VulkanBufferManager::createFrameBuffers() {
    // This note goes for all public member references. I know this is not the best practice, but I am trying to iterate quickly to learn vulkan
    // Not working on writing the best c++ code at this point. That will come in a large refactoring pass.
    swapChainFramebuffers.resize(vulkanSwapChainManager->imageViews.size());

    // Creating one frame buffer per swapchain image since we can be rendering to any of the swapchain images and need a frame buffer for each
    // We then pick the frame buffer that has been rendered to in order to display
    for (size_t i = 0; i < vulkanSwapChainManager->imageViews.size(); i++) {
        VkImageView attachments[] = {
                vulkanSwapChainManager->imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vulkanPipelineManager->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = vulkanSwapChainManager->swapChainExtent.width;
        framebufferInfo.height = vulkanSwapChainManager->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkanDeviceManager->device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanBufferManager::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    // Creating a command buffer from the command pool so that we can record draw commands
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vulkanPoolManager->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBuffers.size();

    if (vkAllocateCommandBuffers(vulkanDeviceManager->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanBufferManager::createVertexBuffer(uint32_t vertexCount, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory, const std::vector<Vertex>* vertices) {
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * vertexCount;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanDeviceManager->device, &bufferInfo, nullptr, vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate the vertex buffer");
    }

    // Now allocating the memory for the buffer here
    VkMemoryRequirements memoryRequirements;
    // The de-reference for the buffer here is because we are holding the vertex buffer in the scene right now
    vkGetBufferMemoryRequirements(vulkanDeviceManager->device, *vertexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(vulkanDeviceManager->device, &allocateInfo, nullptr, vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate the memory for the vertex buffer");
    }

    // The memory was allocated now we need to bind the buffer to the memory
    vkBindBufferMemory(vulkanDeviceManager->device, *vertexBuffer, *vertexBufferMemory, 0);

    void* data; // Pointer to where the memory will be on the cpu after the memory map
    vkMapMemory(vulkanDeviceManager->device, *vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices->data(), (size_t) bufferInfo.size);
    vkUnmapMemory(vulkanDeviceManager->device, *vertexBufferMemory);
}

uint32_t VulkanBufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanDeviceManager->physicalDevice, &memoryProperties);

    // According to the tutorial we are only worrying about the type of memory and not the heap but this might be something to do later to optimize
    for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
        // This if statement is taking the typeFilter mask which we are passing in and checking the current memory property
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}


