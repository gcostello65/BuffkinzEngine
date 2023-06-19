#pragma once

#include "buffkinz_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

#include <vector>

namespace buffkinz {

    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };

    class BuffkinzModel {
    public:

        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal;
            glm::vec2 texCoord;
            glm::uint32_t matId;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();

            static std::vector<VkVertexInputAttributeDescription> getAttribueDescriptions();



            bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
            }

        };

        struct Texture {
            VkImage textureImage;
            VkImageView textureImageView;
            VkSampler sampler;
            std::string filePath;
            int width;
            int height;
            int size;
        };

        std::vector<Texture> getTextures() {return textures;}
        BuffkinzModel(BuffkinzDevice &buffkinzDevice, const std::vector<Vertex> &vertices,
                      const std::vector<uint32_t> &indices, std::vector<std::string> filePaths);

        ~BuffkinzModel();

        void bind(VkCommandBuffer commandBuffer);

        void draw(VkCommandBuffer commandBuffer, uint32_t firstIndex, int32_t vertexOffset);

        VkImageView getTextureImageView() { return textureImageView; }

    private:

        void createVertexBuffers(const std::vector<Vertex> &vertices);

        void createIndexBuffers(const std::vector<uint32_t> &indices);

        void createTextureImageView(std::vector<std::string> filePaths);

        void createTextureSampler(Texture &texture);

        BuffkinzDevice &buffkinzDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t vertexCount;
        std::vector<uint32_t> indicesModel;
        std::vector<Texture> textures;

        VkImageView textureImageView;

    };
}