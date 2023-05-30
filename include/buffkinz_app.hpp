#pragma once

#include "buffkinz_window.hpp"
#include "user_input_controller.hpp"
#include "buffkinz_pipeline.hpp"
#include "buffkinz_device.hpp"
#include "buffkinz_swap_chain.hpp"
#include "buffkinz_model.hpp"
#include "scene.hpp"

#include <vector>
#include <memory>

namespace buffkinz {

    class BuffkinzApp {
        public: 

        struct UniformBufferObject {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
                glm::mat4 lightTransform;
        };
        
            static constexpr int WIDTH = 1920;
            static constexpr int HEIGHT = 1080;

            BuffkinzApp();
            ~BuffkinzApp();

            BuffkinzApp(const BuffkinzApp &) = delete;
            BuffkinzApp &operator = (const BuffkinzApp &) = delete;

            void run();

        private: 
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createDescriptorPool();
            void createDescriptorSets();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);
            void createDescriptorSetLayout();
            void createUniformBuffers();  
            void updateUniformBuffer(int imageIndex);
            

            std::vector<VkBuffer> uniformBuffers;
            std::vector<VkDeviceMemory> uniformBuffersMemory;
            std::vector<void*> uniformBuffersMapped;

            BuffkinzWindow buffkinzWindow{WIDTH, HEIGHT, "BUFFKINZ ENGINE"};
            BuffkinzDevice buffkinzDevice{buffkinzWindow};
            std::unique_ptr<BuffkinzSwapChain> buffkinzSwapChain;
            std::unique_ptr<BuffkinzPipeline> buffkinzPipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<BuffkinzModel> buffkinzModel;
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