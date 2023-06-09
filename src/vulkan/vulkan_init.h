//
// Created by Greg Costello on 6/8/23.
//
#include <buffkinz_device.hpp>
#include <buffkinz_pipeline.hpp>
#include <buffkinz_swap_chain.hpp>
#include <buffkinz_window.hpp>

// This class will create all initial vulkan data structures that are needed to render.
// Pipeline, pipelinelayout, command buffers, descriptor sets, window, device, swapchain.
// Note that these can all have multiple instances, this will only deal with the initial setup of the bare necessity to get the program running.
class VulkanInit {
public:
    VulkanInit();

    ~VulkanInit();
    void createPipelineLayout();
    void recreateSwapChain();
    void createCommandBuffers();
    void createPipeline();
    void freeCommandBuffers();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();

    static constexpr int WIDTH = 1920;
    static constexpr int HEIGHT = 1080;

    buffkinz::BuffkinzWindow window{WIDTH, HEIGHT, "BUFFKINZ ENGINE"};
    buffkinz::BuffkinzDevice device{window};
    std::unique_ptr<buffkinz::BuffkinzSwapChain> swapChain;
    std::unique_ptr<buffkinz::BuffkinzPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
};

