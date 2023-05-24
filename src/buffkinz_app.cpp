#include <buffkinz_app.hpp>

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

namespace buffkinz {

    BuffkinzApp::BuffkinzApp() {

        loadModels();
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
        while (!buffkinzWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();
            vkDeviceWaitIdle(buffkinzDevice.device());
        }
    }

    void BuffkinzApp::loadModels() {
        std::vector<BuffkinzModel::Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
            throw std::runtime_error(warn + err);
        }

        int i = 0;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                BuffkinzModel::Vertex vertex{};
                vertex.position  = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                // vertex.texCoord = {
                //     attrib.texcoords[2 * index.texcoord_index + 0],
                //     attrib.texcoords[2 * index.texcoord_index + 1]
                // };
                
                vertex.color = {(i % 3) * 1.0f, (i % 3) * 1.0f, (i % 3) * 1.0f};
                i++;
                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }


        buffkinzModel = std::make_unique<BuffkinzModel>(buffkinzDevice, vertices, indices);
}

    void BuffkinzApp::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(buffkinzDevice.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void BuffkinzApp::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        std::cout << "Image count: " + std::to_string(static_cast<int>(buffkinzSwapChain->imageCount())) + "\n";
        uniformBuffers.resize(buffkinzSwapChain->imageCount());
        uniformBuffersMemory.resize(buffkinzSwapChain->imageCount());
        uniformBuffersMapped.resize(buffkinzSwapChain->imageCount());

        for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
        buffkinzDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

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
        if (vkCreatePipelineLayout(buffkinzDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
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
            buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(buffkinzDevice, extent, std::move(buffkinzSwapChain));
            if (buffkinzSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }  
        createPipeline();

    
    }

    void BuffkinzApp::freeCommandBuffers() {
        vkFreeCommandBuffers(buffkinzDevice.device(), buffkinzDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
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
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());     

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
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

        for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(buffkinzDevice.device(), 1, &descriptorWrite, 0, nullptr);
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
            renderPassInfo.renderArea.offset = {0,0};
            renderPassInfo.renderArea.extent = buffkinzSwapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 0.0f};
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

            buffkinzPipeline->bind(commandBuffers[imageIndex]);
            vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, nullptr);    

            buffkinzModel->bind(commandBuffers[imageIndex]);
            buffkinzModel->draw(commandBuffers[imageIndex]);

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
    }

    void BuffkinzApp::updateUniformBuffer(int imageIndex) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        BuffkinzApp::UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.0f, 0.0f));


        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        ubo.proj = glm::perspective(glm::radians(60.0f), buffkinzSwapChain->width() / (float) buffkinzSwapChain->height(), 0.1f, 100.0f);
        ubo.proj[1][1] *= -1;


        memcpy(uniformBuffersMapped[imageIndex], &ubo, sizeof(ubo));
    }

    void BuffkinzApp::drawFrame() {
        uint32_t imageIndex;
        auto result = buffkinzSwapChain->acquireNextImage(&imageIndex);
        updateUniformBuffer(imageIndex);

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