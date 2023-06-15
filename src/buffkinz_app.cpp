#include <buffkinz_app.hpp>
#include "user_input_controller.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <chrono>

#include <glm/common.hpp>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

namespace std {
    template<>
    struct hash<buffkinz::BuffkinzModel::Vertex> {
        size_t operator()(buffkinz::BuffkinzModel::Vertex const &vertex) const {
            size_t seed = 0;
            buffkinz::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCoord);
            return seed;
        }
    };
}

namespace buffkinz {

    BuffkinzApp::BuffkinzApp() {
        vulkan = std::make_unique<VulkanInit>();
        loadGameObjects({"../model/model/armor_2021.obj"});
        createDescriptorSets();
        camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.lookDir = glm::vec3(0.0f, 0.0f, 1.0f);
        camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    BuffkinzApp::~BuffkinzApp() {
        vkDestroyDescriptorPool(vulkan->device.device(), vulkan->descriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(vulkan->device.device(), vulkan->descriptorSetLayout, nullptr);
        for (size_t i = 0; i < vulkan->swapChain->imageCount(); i++) {
            vkDestroyBuffer(vulkan->device.device(), vulkan->uniformBuffers[i], nullptr);
            vkFreeMemory(vulkan->device.device(), vulkan->uniformBuffersMemory[i], nullptr);
        }
    }

    void BuffkinzApp::run() {

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = 0.0f;
        while (!vulkan->window.shouldClose()) {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            glfwSetInputMode(vulkan->window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            controller.keyMovement(vulkan->window.getWindow(), (float &) frameTime, camera);

            drawFrame();
            vkDeviceWaitIdle(vulkan->device.device());
        }
    }

    void BuffkinzApp::loadGameObjects(std::vector<std::string> objFilePaths) {
        std::vector<BuffkinzModel::Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        std::vector<std::string> texturePaths;
        int i = 0;

        for (std::string objFile: objFilePaths) {

//            for (int k = 0; k < 2; k++) {
//                for (int l = 0; l < 2; l++) {
            vertices.clear();
            indices.clear();
            shapes.clear();
            materials.clear();


            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str())) {
                throw std::runtime_error(warn + err);
            }

            std::unordered_map<BuffkinzModel::Vertex, uint32_t> uniqueVertices{};

            for (const auto &shape: shapes) {
                int k = 0;
                for (const auto &index: shape.mesh.indices) {
                    BuffkinzModel::Vertex vertex{};
                    vertex.matId = shape.mesh.material_ids[k];
                    k++;
                    vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = {
                            1.0f, 1.0f, 1.0f
                    };

                    vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]
                    };


                    vertex.texCoord = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0 - attrib.texcoords[2 * index.texcoord_index + 1]
                    };


                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(uniqueVertices[vertex]);

                }
                i++;
            }


            auto object = GameObject::createGameObject();
            for (tinyobj::material_t mat : materials) {
                texturePaths.push_back(mat.diffuse_texname);
            }
            auto buffkinzModel = std::make_shared<BuffkinzModel>(vulkan->device, vertices, indices,
                                                                 texturePaths);
            object.model = buffkinzModel;
//                    object.position = glm::vec3(0.0f, 5.0f * (float)k, 5.0f * (float)l);
            object.position = glm::vec3(0.0f, 0.0f, 5.0f);
            gameObjects.push_back(std::move(object));

        }
//            }
//        }
    }

    void BuffkinzApp::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(vulkan->swapChain->imageCount(), vulkan->descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vulkan->descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkan->swapChain->imageCount());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(vulkan->swapChain->imageCount());
        if (vkAllocateDescriptorSets(vulkan->device.device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        std::cout << "image number: " << vulkan->swapChain->imageCount() << std::endl;
        for (size_t i = 0; i < vulkan->swapChain->imageCount(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = vulkan->uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = vulkan->device.properties.limits.minUniformBufferOffsetAlignment;

            VkDescriptorImageInfo descriptorImageInfos[16];
            for (int j = 0; j < 16; j++) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                if (gameObjects[0].model->getTextures().size() > j) {
                    imageInfo.imageView = gameObjects[0].model->getTextures()[j].textureImageView;
                    imageInfo.sampler = gameObjects[0].model->getTextures()[j].sampler;
                } else {
                    imageInfo.imageView = gameObjects[0].model->getTextures()[0].textureImageView;
                    imageInfo.sampler = gameObjects[0].model->getTextures()[0].sampler;
                }
                descriptorImageInfos[j] = imageInfo;
            }

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;

            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[0].descriptorCount = gameObjects.size();

            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 16;
            descriptorWrites[1].pImageInfo = descriptorImageInfos;

            vkUpdateDescriptorSets(vulkan->device.device(), static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
    }

    void BuffkinzApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(vulkan->commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }


        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkan->swapChain->getRenderPass();
        renderPassInfo.framebuffer = vulkan->swapChain->getFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vulkan->swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {114.f / 255.f, 149.f / 255.f, 183.f / 255.f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(vulkan->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vulkan->swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vulkan->swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vulkan->swapChain->getSwapChainExtent()};
        vkCmdSetViewport(vulkan->commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(vulkan->commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(vulkan->commandBuffers[imageIndex], descriptorSets[imageIndex], imageIndex);

        vkCmdEndRenderPass(vulkan->commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(vulkan->commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void BuffkinzApp::renderGameObjects(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, int imageIndex) {
        vulkan->pipeline->bind(commandBuffer);
        for (auto &obj: gameObjects) {
            updateUniformBuffer(imageIndex, obj);
            uint32_t dynamicOffset = obj.getId() * vulkan->device.properties.limits.minUniformBufferOffsetAlignment;
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->pipelineLayout, 0, 1,
                                    &descriptorSet, 1,
                                    &dynamicOffset);

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer, 0, 0);
        }
    }

    void BuffkinzApp::updateUniformBuffer(int imageIndex, GameObject &object) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        object.ubo.model = glm::scale(glm::translate(glm::mat4(1.0f), object.position), glm::vec3(1.0f, 1.0f, 1.0f)) *
                           glm::rotate(glm::mat4(1.0f), glm::degrees(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        object.ubo.lightTransform = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f),
                                                glm::vec3(0.0f, 1.0f, 0.0f));

        object.ubo.view = glm::lookAt(camera.position, camera.position + camera.lookDir, camera.up);

        object.ubo.proj = glm::perspective(glm::radians(60.0f),
                                           vulkan->swapChain->width() / (float) vulkan->swapChain->height(), 0.1f,
                                           100.0f);

        object.ubo.proj[1][1] *= -1;

        object.ubo.viewDir = camera.lookDir;

        memcpy((char *) vulkan->uniformBuffersMapped[imageIndex] +
               vulkan->device.properties.limits.minUniformBufferOffsetAlignment * object.getId(), &object.ubo,
               sizeof(object.ubo));
    }

    void BuffkinzApp::drawFrame() {
        uint32_t imageIndex;
        auto result = vulkan->swapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            vulkan->recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }


        recordCommandBuffer(imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        result = vulkan->swapChain->submitCommandBuffers(&vulkan->commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkan->window.wasWindowResized()) {
            vulkan->window.resetWindowResizedFlag();
            vulkan->recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}