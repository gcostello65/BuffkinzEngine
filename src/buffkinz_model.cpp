#include "buffkinz_model.hpp"
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace buffkinz {
    BuffkinzModel::BuffkinzModel(BuffkinzDevice& device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : buffkinzDevice{device} {
        createVertexBuffers(vertices);
        createIndexBuffers(indices);
        createTextureImage();
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

    void BuffkinzModel::draw(VkCommandBuffer commandBuffer, uint32_t firstIndex, int32_t vertexOffset) {
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indicesModel.size()), 1, firstIndex, vertexOffset, 0);
    }
    void BuffkinzModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void BuffkinzModel::createTextureImage() {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("../textures/Samus_Varia_High512_C.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        buffkinzDevice.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(buffkinzDevice.device(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(buffkinzDevice.device(), stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
    }

    void BuffkinzModel::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(buffkinzDevice.device(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(buffkinzDevice.device(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = buffkinzDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(buffkinzDevice.device(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(buffkinzDevice.device(), image, imageMemory, 0);
    }

    std::vector<VkVertexInputBindingDescription> BuffkinzModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    
    std::vector<VkVertexInputAttributeDescription> BuffkinzModel::Vertex::getAttribueDescriptions() {
         std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
         attributeDescriptions[0].binding = 0;
         attributeDescriptions[0].location = 0;
         attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
         attributeDescriptions[0].offset = offsetof(Vertex, position);;

         attributeDescriptions[1].binding = 0;
         attributeDescriptions[1].location = 1;
         attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
         attributeDescriptions[1].offset = offsetof(Vertex, color);

         attributeDescriptions[2].binding = 0;
         attributeDescriptions[2].location = 2;
         attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
         attributeDescriptions[2].offset = offsetof(Vertex, normal);

         return attributeDescriptions;
    }
}