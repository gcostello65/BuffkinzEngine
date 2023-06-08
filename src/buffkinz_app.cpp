#include <buffkinz_app.hpp>
#include "user_input_controller.hpp"

#include <stdexcept>
#include<array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <chrono>
#include <glm/gtc/matrix_inverse.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include<glm/common.hpp>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define GLM_FORCE_RADIANS

namespace std {
    template <>
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

        loadGameObjects({"../model/sir_buffkinz2_more_geometry.obj"});
        camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
        camera.lookDir = glm::vec3(0.0f, 0.0f, 1.0f);
        camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
        createDescriptorSetLayout();
        createPipelineLayout();
        recreateSwapChain();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
    }

    BuffkinzApp::~BuffkinzApp() {
        vkDestroyPipelineLayout(buffkinzDevice.device(), pipelineLayout, nullptr);

        vkDestroyDescriptorPool(buffkinzDevice.device(), descriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(buffkinzDevice.device(), descriptorSetLayout, nullptr);

        for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
            vkDestroyBuffer(buffkinzDevice.device(), uniformBuffers[i], nullptr);
            vkFreeMemory(buffkinzDevice.device(), uniformBuffersMemory[i], nullptr);
        }
    }

    void BuffkinzApp::run() {

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = 0.0f;
        while (!buffkinzWindow.shouldClose()) {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
             frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
//            std::cout << "Frame time: " << frameTime << std::endl;
            currentTime = newTime;

            glfwSetInputMode(buffkinzWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            controller.keyMovement(buffkinzWindow.getWindow(), (float &) frameTime, camera);

            drawFrame();
            vkDeviceWaitIdle(buffkinzDevice.device());
        }
    }

    void BuffkinzApp::loadGameObjects(std::vector<std::string> objFilePaths) {
        std::vector<BuffkinzModel::Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        int i = 0;

        for (std::string objFile : objFilePaths) {

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
                        for (const auto &index: shape.mesh.indices) {
                            BuffkinzModel::Vertex vertex{};
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
                    }



                    auto object = GameObject::createGameObject();
                    auto buffkinzModel = std::make_shared<BuffkinzModel>(buffkinzDevice, vertices, indices,
                                                                         "../model/sir_buff_2.png");
                    object.model = buffkinzModel;
//                    object.position = glm::vec3(0.0f, 5.0f * (float)k, 5.0f * (float)l);
                    object.position = glm::vec3(0.0f, i * 20.0f, 5.0f);
                    gameObjects.push_back(std::move(object));
                    i++;
                }
//            }
//        }
    }

    void BuffkinzApp::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(buffkinzDevice.device(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void BuffkinzApp::createUniformBuffers() {
        VkDeviceSize bufferSize = buffkinzDevice.properties.limits.minUniformBufferOffsetAlignment * 100;
        std::cout << "Image count: " + std::to_string(static_cast<int>(buffkinzSwapChain->imageCount())) + "\n";
        uniformBuffers.resize(buffkinzSwapChain->imageCount());
        uniformBuffersMemory.resize(buffkinzSwapChain->imageCount());
        uniformBuffersMapped.resize(buffkinzSwapChain->imageCount());

        for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
            buffkinzDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        uniformBuffers[i], uniformBuffersMemory[i]);

            vkMapMemory(buffkinzDevice.device(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
        }
    }

    void BuffkinzApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(buffkinzDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void BuffkinzApp::createPipeline() {
        assert(buffkinzSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};

        BuffkinzPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = buffkinzSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        buffkinzPipeline = std::make_unique<BuffkinzPipeline>(
                buffkinzDevice,
                "./shaders/simple_shader.vert.spv",
                "./shaders/simple_shader.frag.spv",
                pipelineConfig);
    }

    void BuffkinzApp::recreateSwapChain() {
        auto extent = buffkinzWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = buffkinzWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(buffkinzDevice.device());
        if (buffkinzSwapChain == nullptr) {
            buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(buffkinzDevice, extent);
        } else {
            buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(buffkinzDevice, extent,
                                                                    std::move(buffkinzSwapChain));
            if (buffkinzSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        createPipeline();


    }

    void BuffkinzApp::freeCommandBuffers() {
        vkFreeCommandBuffers(buffkinzDevice.device(), buffkinzDevice.getCommandPool(),
                             static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void BuffkinzApp::createCommandBuffers() {
        commandBuffers.resize(buffkinzSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = buffkinzDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(buffkinzDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void BuffkinzApp::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(buffkinzSwapChain->imageCount());

        if (vkCreateDescriptorPool(buffkinzDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

    }

    void BuffkinzApp::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(buffkinzSwapChain->imageCount(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(buffkinzSwapChain->imageCount());
        if (vkAllocateDescriptorSets(buffkinzDevice.device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        std::cout << "image number: " << buffkinzSwapChain->imageCount() << std::endl;
        for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = buffkinzDevice.properties.limits.minUniformBufferOffsetAlignment;

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = gameObjects[0].model->getTextureImageView();
            imageInfo.sampler = gameObjects[0].model->textureSampler;

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
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(buffkinzDevice.device(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }


    }

    void BuffkinzApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }


        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = buffkinzSwapChain->getRenderPass();
        renderPassInfo.framebuffer = buffkinzSwapChain->getFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = buffkinzSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {114.f/255.f, 149.f/255.f, 183.f/255.f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(buffkinzSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(buffkinzSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, buffkinzSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

//        buffkinzPipeline->bind(commandBuffers[imageIndex]);

//        buffkinzModel->bind(commandBuffers[imageIndex]);
//        buffkinzModel->draw(commandBuffers[imageIndex], 0, 0);
        renderGameObjects(commandBuffers[imageIndex], descriptorSets[imageIndex], imageIndex);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void BuffkinzApp::renderGameObjects(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, int imageIndex) {
        buffkinzPipeline->bind(commandBuffer);
        for (auto &obj: gameObjects) {
            updateUniformBuffer(imageIndex, obj);
            uint32_t dynamicOffset = obj.getId() * buffkinzDevice.properties.limits.minUniformBufferOffsetAlignment;
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
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

//        if (object.getId() == 1) {
            object.ubo.model = glm::scale(glm::translate(glm::mat4(1.0f), object.position), glm::vec3(1.0f, 1.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::degrees(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        } else {
//            object.ubo.model = glm::translate(glm::mat4(1.0f), object.position);
//                    * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        }

        object.ubo.lightTransform = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        object.ubo.view = glm::lookAt(camera.position, camera.position + camera.lookDir, camera.up);

        object.ubo.proj = glm::perspective(glm::radians(60.0f),
                                    buffkinzSwapChain->width() / (float) buffkinzSwapChain->height(), 0.1f, 100.0f);

        object.ubo.proj[1][1] *= -1;

//        std::cout << "rotation angle: " << camera.rotAngle << std::endl;
//        glm::quat rotationQuat = glm::angleAxis(camera.rotAngle, camera.rotationAxis);
//
//        glm::quat result = glm::inverse(rotationQuat) * camera.lookDir * rotationQuat;
//
//        camera.lookDir = glm::vec3(result.x, result.y, result.z);
        object.ubo.viewDir = camera.lookDir;

        memcpy((char*)uniformBuffersMapped[imageIndex] + buffkinzDevice.properties.limits.minUniformBufferOffsetAlignment * object.getId(), &object.ubo, sizeof(object.ubo));
    }

    void BuffkinzApp::drawFrame() {
        uint32_t imageIndex;
        auto result = buffkinzSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }


        recordCommandBuffer(imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        result = buffkinzSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || buffkinzWindow.wasWindowResized()) {
            buffkinzWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}