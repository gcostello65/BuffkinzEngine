//
// Created by Greg Costello on 6/8/23.
//
#include <buffkinz_device.hpp>
#include <buffkinz_pipeline.hpp>
#include <buffkinz_swap_chain.hpp>
#include <buffkinz_window.hpp>

class VulkanInit {
public:
    VulkanInit();
    ~VulkanInit();
    void createPipelineLayout();
    void recreateSwapChain();
    void createCommandBuffers();

    static constexpr int WIDTH = 1920;
    static constexpr int HEIGHT = 1080;

    buffkinz::BuffkinzWindow window{WIDTH, HEIGHT, "BUFFKINZ ENGINE"};
    buffkinz::BuffkinzDevice device{window};
    std::unique_ptr<buffkinz::BuffkinzSwapChain> swapChain;
    std::unique_ptr<buffkinz::BuffkinzPipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};

