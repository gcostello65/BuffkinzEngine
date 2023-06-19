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

            std::unique_ptr<VulkanInit> vulkan;

        private: 
            void loadGameObjects(std::vector<std::string> objFilePaths);
            void drawFrame();
            void recordCommandBuffer(int imageIndex);
            void updateUniformBuffer(int imageIndex, GameObject &object);
            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VkDescriptorSet> descriptorSets, int imageIndex);
            void createDescriptorSets();

            std::vector<GameObject> gameObjects;
            const std::string MODEL_PATH = "../model/env.obj";
            Scene::Camera camera{};
            std::vector<VkDescriptorSet> descriptorSets;
            UserInputController controller;
    };
}