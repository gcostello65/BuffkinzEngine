#pragma once

#include "buffkinz_window.hpp"
#include "user_input_controller.hpp"
#include "buffkinz_pipeline.hpp"
#include "buffkinz_device.hpp"
#include "buffkinz_swap_chain.hpp"
#include "buffkinz_model.hpp"
#include "scene.hpp"
#include "game_object.hpp"
#include "vulkan_init.h"

#include <vector>
#include <memory>

namespace buffkinz {

    class BuffkinzApp {
        public:

            BuffkinzApp();
            ~BuffkinzApp();

            BuffkinzApp(const BuffkinzApp &) = delete;
            BuffkinzApp &operator = (const BuffkinzApp &) = delete;

            void run();

            VulkanInit vulkan;

        private: 
            void loadGameObjects(std::vector<std::string> objFilePaths);
            void createPipeline();
            void createDescriptorSetLayout();
            void createDescriptorPool();
            void createDescriptorSets();
            void drawFrame();
            void recordCommandBuffer(int imageIndex);
            void createUniformBuffers();
            void updateUniformBuffer(int imageIndex, GameObject &object);
            void renderGameObjects(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, int imageIndex);
            

            std::vector<VkBuffer> uniformBuffers;
            std::vector<VkDeviceMemory> uniformBuffersMemory;
            std::vector<void*> uniformBuffersMapped;

            std::vector<GameObject> gameObjects;
            VkDescriptorSetLayout descriptorSetLayout;
            VkDescriptorPool descriptorPool;

            std::vector<VkDescriptorSet> descriptorSets;
            const std::string MODEL_PATH = "../model/env.obj";
            int32_t vertexOffset;
            uint32_t indexOffset;
            Scene::Camera camera{};

            UserInputController controller;
    };
}