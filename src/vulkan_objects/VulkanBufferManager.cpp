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

// Helper method for buffer creation. Doing everything from creating the buffer to mapping it to memory
void VulkanBufferManager::createBuffer(VkDeviceSize size, VkDeviceMemory &bufferMemory, VkBufferUsageFlags usage,
                                       VkMemoryPropertyFlags properties, VkBuffer *bufferHandle) {
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkanDeviceManager->device, &bufferInfo, nullptr, bufferHandle) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate the vertex buffer");
    }

    // Now allocating the memory for the buffer here
    VkMemoryRequirements memoryRequirements;
    // The de-reference for the buffer here is because we are holding the vertex buffer in the scene right now
    vkGetBufferMemoryRequirements(vulkanDeviceManager->device, *bufferHandle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkanDeviceManager->device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Could not allocate the memory for the vertex buffer");
    }

    // The memory was allocated now we need to bind the buffer to the memory
    vkBindBufferMemory(vulkanDeviceManager->device, *bufferHandle, bufferMemory, 0);
}

void VulkanBufferManager::createVertexBuffer(uint32_t vertexCount, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory, const std::vector<Vertex>* vertices) {
    VkDeviceSize bufferSize = sizeof(Vertex) * vertexCount;

    // THIS IS IMPORTANT, ITS HOW YOU TRANSFER MEMORY FROM HOST LOCAL TO DEVICE LOCAL!
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer);

    void* data; // Pointer to where the memory will be on the cpu after the memory map
    vkMapMemory(vulkanDeviceManager->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices->data(), bufferSize);
    vkUnmapMemory(vulkanDeviceManager->device, stagingBufferMemory);

    // TODO: make it so that we have one large vertex buffer instead of just the vertices we need and use an offset baked into the mesh data
    createBuffer(bufferSize, *vertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer);

    copyBuffer(stagingBuffer, *vertexBuffer, bufferSize);

    // Important thing to remember here: a buffer always has memory that needs to be allocated to it so they come as a pair, you need to destroy both when you are cleaning them up
    vkDestroyBuffer(vulkanDeviceManager->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDeviceManager->device, stagingBufferMemory, nullptr);
}

void VulkanBufferManager::copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size) {
    // Create the command buffer
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = vulkanPoolManager->commandPool;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkanDeviceManager->device, &allocateInfo, &commandBuffer);

    // Record the command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // Immediately record the command buffer
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    // Submit the command buffer now TODO: SHould probably create a helper method to create all of these structs for recording and then submitting. Gets tedious to copy them
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(vulkanDeviceManager->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkanDeviceManager->graphicsQueue);

    vkFreeCommandBuffers(vulkanDeviceManager->device, vulkanPoolManager->commandPool, 1, &commandBuffer);
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

void
VulkanBufferManager::createIndexBuffer(uint32_t indexCount, VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory,
                                       const std::vector<uint32_t> *indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

    // THIS IS IMPORTANT, ITS HOW YOU TRANSFER MEMORY FROM HOST LOCAL TO DEVICE LOCAL!
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, stagingBufferMemory, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer);

    void* data; // Pointer to where the memory will be on the cpu after the memory map
    vkMapMemory(vulkanDeviceManager->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices->data(), bufferSize);
    vkUnmapMemory(vulkanDeviceManager->device, stagingBufferMemory);

    // TODO: make it so that we have one large vertex buffer instead of just the vertices we need and use an offset baked into the mesh data
    createBuffer(bufferSize, *indexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer);

    copyBuffer(stagingBuffer, *indexBuffer, bufferSize);

    // Important thing to remember here: a buffer always has memory that needs to be allocated to it so they come as a pair, you need to destroy both when you are cleaning them up
    vkDestroyBuffer(vulkanDeviceManager->device, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDeviceManager->device, stagingBufferMemory, nullptr);
}


