//
// Created by Greg on 11/2/2024.
//

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

#include "vulkan_objects/VulkanInstanceManager.h"
#include "vulkan_objects/VulkanDeviceManager.h"
#include "vulkan_objects/VulkanWindowManager.h"
#include "vulkan_objects/VulkanSwapChainManager.h"
#include "vulkan_objects/VulkanPipelineManager.h"

class HelloTriangleApplication {
public:
    //TODO: Update all of the public members in the managers to have proper abstraction and getters and setters. Not great to have everything publicly exposed.
    VulkanInstanceManager vulkanInstanceManager;
    VulkanDeviceManager vulkanDeviceManager;
    VulkanWindowManager vulkanWindowManager;
    VulkanSwapChainManager vulkanSwapChainManager;
    VulkanPipelineManager vulkanPipelineManager;

    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initVulkan() {
        // TODO: make all of the public handles to member objects getters
        vulkanInstanceManager.createInstance();
        vulkanInstanceManager.handleMessageCallbacks();
        vulkanWindowManager.createSurface(vulkanInstanceManager.instance, window);
        vulkanDeviceManager.setInstance(vulkanInstanceManager.instance);
        vulkanDeviceManager.setSurface(vulkanWindowManager.surface);
        vulkanDeviceManager.pickPhysicalDevice();
        vulkanDeviceManager.createLogicalDevice();
        vulkanSwapChainManager.setPhysicalDevice(vulkanDeviceManager.physicalDevice);
        vulkanSwapChainManager.setDevice(vulkanDeviceManager.device);
        vulkanSwapChainManager.setSurface(vulkanWindowManager.surface);
        vulkanSwapChainManager.setWindow(window);
        vulkanSwapChainManager.setDeviceManager(&vulkanDeviceManager);
        vulkanSwapChainManager.createSwapChain();
        vulkanSwapChainManager.createImageViews();
        vulkanPipelineManager.createGraphicsPipeline();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();
        }
    }

    void cleanup() {
        for (auto imageView : vulkanSwapChainManager.imageViews) {
            vkDestroyImageView(vulkanDeviceManager.device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(vulkanDeviceManager.device, vulkanSwapChainManager.swapChain, nullptr);
        vkDestroySurfaceKHR(vulkanInstanceManager.instance, vulkanWindowManager.surface, nullptr);
        vkDestroyDevice(vulkanDeviceManager.device, nullptr);
        vkDestroyInstance(vulkanInstanceManager.instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}