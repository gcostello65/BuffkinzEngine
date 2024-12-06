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

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static const VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    // This method will need to change as we add more vertext attributes
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class VulkanBufferManager {
public:
    //TODO: Make a constants file for stuff like this, this is bad practice and I know but just trying to make things work right now
    const static int MAX_FRAMES_IN_FLIGHT = 2;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VulkanSwapChainManager *vulkanSwapChainManager;
    // Probably best practice to make all of these object members pointers since there will be a copy everytime... Might need to refactor
    VulkanDeviceManager *vulkanDeviceManager;
    VulkanPipelineManager *vulkanPipelineManager;
    VulkanPoolManager *vulkanPoolManager;
    std::vector<VkCommandBuffer> commandBuffers;

    void createFrameBuffers();
    void createCommandBuffers();
    void createVertexBuffer(uint32_t vertexCount, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory, const std::vector<Vertex>* vertices);
    void createIndexBuffer(uint32_t indexCount, VkBuffer *indexBuffer, VkDeviceMemory *indexBufferMemory, const std::vector<uint32_t>* indices);


    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
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

    void createBuffer(VkDeviceSize size, VkDeviceMemory &bufferMemory, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer *bufferHandle);

    void copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size);
};


#endif //BUFFKINZRENDERER_VULKANBUFFERMANAGER_H
