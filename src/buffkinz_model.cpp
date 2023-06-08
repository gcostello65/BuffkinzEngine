#include "buffkinz_model.hpp"
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace buffkinz {
    BuffkinzModel::BuffkinzModel(BuffkinzDevice& device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const char* filePath) : buffkinzDevice{device} {
        createVertexBuffers(vertices);
        createIndexBuffers(indices);

        buffkinzDevice.createTextureImage();
        createTextureImageView(filePath);
        createTextureSampler();
        indicesModel = indices;
    }



    BuffkinzModel::~BuffkinzModel() {
        vkDestroyBuffer(buffkinzDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(buffkinzDevice.device(), vertexBufferMemory, nullptr);
        vkDestroyBuffer(buffkinzDevice.device(), indexBuffer, nullptr);
        vkFreeMemory(buffkinzDevice.device(), indexBufferMemory, nullptr);
        vkDestroyImageView(buffkinzDevice.device(), textureImageView, nullptr);
        vkDestroySampler(buffkinzDevice.device(), textureSampler, nullptr);
        vkDestroyImageView(buffkinzDevice.device(), textureImageView, nullptr);

    }

    void BuffkinzModel::createTextureImageView(char const* filePath) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        memcpy(buffkinzDevice.getTextureData(), pixels, static_cast<size_t>(imageSize));
        buffkinzDevice.transitionImageLayout(buffkinzDevice.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        buffkinzDevice.copyBufferToImage(buffkinzDevice.textureStagingBuffer, buffkinzDevice.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);
        buffkinzDevice.transitionImageLayout(buffkinzDevice.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = buffkinzDevice.getTextureImage();
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(buffkinzDevice.device(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void BuffkinzModel::createTextureSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = buffkinzDevice.properties.limits.maxSamplerAnisotropy;

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(buffkinzDevice.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
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

    std::vector<VkVertexInputBindingDescription> BuffkinzModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }
    
    std::vector<VkVertexInputAttributeDescription> BuffkinzModel::Vertex::getAttribueDescriptions() {
         std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
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

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

         return attributeDescriptions;
    }
}