#include "buffkinz_model.hpp"
#include <cassert>

namespace buffkinz {
    BuffkinzModel::BuffkinzModel(BuffkinzDevice& device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : buffkinzDevice{device} {
        createVertexBuffers(vertices);
        createIndexBuffers(indices);
        indicesModel = indices;
    }

    BuffkinzModel::~BuffkinzModel() {
        vkDestroyBuffer(buffkinzDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(buffkinzDevice.device(), vertexBufferMemory, nullptr);
        vkDestroyBuffer(buffkinzDevice.device(), indexBuffer, nullptr);
        vkFreeMemory(buffkinzDevice.device(), indexBufferMemory, nullptr);
    }

    void BuffkinzModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        buffkinzDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);
        
        void *data;
        vkMapMemory(buffkinzDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(buffkinzDevice.device(), vertexBufferMemory);
    }

    void BuffkinzModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    buffkinzDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(buffkinzDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(buffkinzDevice.device(), stagingBufferMemory);

    buffkinzDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    buffkinzDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(buffkinzDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(buffkinzDevice.device(), stagingBufferMemory, nullptr);
    }

    void BuffkinzModel::draw(VkCommandBuffer commandBuffer) {
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesModel.size()), 1, 0, 0, 0);
    }
    void BuffkinzModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    std::vector<VkVertexInputBindingDescription> BuffkinzModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    
    std::vector<VkVertexInputAttributeDescription> BuffkinzModel::Vertex::getAttribueDescriptions() {
         std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
         attributeDescriptions[0].binding = 0;
         attributeDescriptions[0].location = 0;
         attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
         attributeDescriptions[0].offset = offsetof(Vertex, position);;

         attributeDescriptions[1].binding = 0;
         attributeDescriptions[1].location = 1;
         attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
         attributeDescriptions[1].offset = offsetof(Vertex, color);
         return attributeDescriptions;
    }
}