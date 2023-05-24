#pragma once

#include "buffkinz_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace buffkinz {
    class BuffkinzModel {
        public: 

            struct Vertex {
                glm::vec3 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttribueDescriptions();
            };

            BuffkinzModel(BuffkinzDevice& buffkinzDevice, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
            ~BuffkinzModel();

            BuffkinzModel(const BuffkinzModel &) = delete;
            BuffkinzModel &operator = (const BuffkinzModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);
            void loadModels();
            
        private: 

            void createVertexBuffers(const std::vector<Vertex> &vertices);
            void createIndexBuffers(const std::vector<uint32_t> &indices);

            BuffkinzDevice& buffkinzDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
            uint32_t vertexCount;
            std::vector<uint32_t> indicesModel;
            const std::string MODEL_PATH = "model/DolBarriersuit.obj";
    };
}