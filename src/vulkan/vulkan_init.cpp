//
// Created by Greg Costello on 6/8/23.
//

#include <vulkan/vulkan_core.h>
#include "vulkan_init.h"

VulkanInit::VulkanInit() {}
VulkanInit::~VulkanInit() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}
void VulkanInit::createPipelineLayout() {}
void VulkanInit::recreateSwapChain() {}
void VulkanInit::createCommandBuffers() {}

void VulkanInit::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void VulkanInit::createPipeline() {
    assert(buffkinzSwapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};

    BuffkinzPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = buffkinzSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    buffkinzPipeline = std::make_unique<BuffkinzPipeline>(
            device,
            "./shaders/simple_shader.vert.spv",
            "./shaders/simple_shader.frag.spv",
            pipelineConfig);
}

void VulkanInit::recreateSwapChain() {
    auto extent = buffkinzWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = buffkinzWindow.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.device());
    if (buffkinzSwapChain == nullptr) {
        buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(device, extent);
    } else {
        buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(device, extent,
                                                                std::move(buffkinzSwapChain));
        if (buffkinzSwapChain->imageCount() != commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
    }
    createPipeline();


}

void VulkanInit::freeCommandBuffers() {
    vkFreeCommandBuffers(device.device(), device.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
}

void VulkanInit::createCommandBuffers() {
    commandBuffers.resize(buffkinzSwapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }
}

