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
#include "vulkan_objects/VulkanBufferManager.h"
#include "vulkan_objects/VulkanCommandModule.h"
#include "engine_objects/Scene.h"

class VulkanInit {
public:
    //TODO: Update all of the public members in the managers to have proper abstraction and getters and setters. Not great to have everything publicly exposed.
    VulkanInstanceManager vulkanInstanceManager;
    VulkanDeviceManager vulkanDeviceManager;
    VulkanWindowManager vulkanWindowManager;
    VulkanSwapChainManager vulkanSwapChainManager;
    VulkanPipelineManager vulkanPipelineManager;
    VulkanBufferManager vulkanBufferManager;
    VulkanPoolManager vulkanPoolManager;
    VulkanCommandModule vulkanCommandModule;

    Scene scene;


    void run() {
        initWindow();
        initVulkan();

        scene.loadScene("./assets/glTF/Sponza.gltf");
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initVulkan() {
        // TODO: make all of the public handles to member objects getters
        // TODO: I should really just make one common config class that has all of these handles that I can add to each class and reference pointers
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
        vulkanPipelineManager.setDevice(vulkanDeviceManager.device);
        vulkanPipelineManager.setSwapChainHandle(vulkanSwapChainManager);
        vulkanPipelineManager.createRenderPass();
        vulkanPipelineManager.createGraphicsPipeline();
        vulkanBufferManager.setSwapChainManager(&vulkanSwapChainManager);
        vulkanBufferManager.setDeviceManager(&vulkanDeviceManager);
        vulkanBufferManager.setPipelineManager(&vulkanPipelineManager);
        vulkanPoolManager.setDeviceManager(&vulkanDeviceManager);
        vulkanPoolManager.createCommandPool();
        vulkanBufferManager.setPoolManager(&vulkanPoolManager);
        vulkanBufferManager.createFrameBuffers();
        vulkanBufferManager.createCommandBuffers();
        vulkanCommandModule.setPipelineManager(&vulkanPipelineManager);
        vulkanCommandModule.setSwapChainManager(&vulkanSwapChainManager);
        vulkanCommandModule.setBufferManager(&vulkanBufferManager);
        vulkanCommandModule.setDeviceManager(&vulkanDeviceManager);
        vulkanCommandModule.createSyncObjects();

        scene.vulkanBufferManager = &vulkanBufferManager;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            vulkanCommandModule.drawFrame(&scene);
        }

        vkDeviceWaitIdle(vulkanDeviceManager.device);
    }

    void cleanup() {
        for (auto imageView : vulkanSwapChainManager.imageViews) {
            vkDestroyImageView(vulkanDeviceManager.device, imageView, nullptr);
        }
        for (auto framebuffer : vulkanBufferManager.swapChainFramebuffers) {
            vkDestroyFramebuffer(vulkanDeviceManager.device, framebuffer, nullptr);
        }

        for (int i = 0; i < vulkanBufferManager.MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(vulkanDeviceManager.device, vulkanCommandModule.imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(vulkanDeviceManager.device, vulkanCommandModule.renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(vulkanDeviceManager.device, vulkanCommandModule.inFlightFences[i], nullptr);
        }
        vkDestroyBuffer(vulkanDeviceManager.device, scene.vertexBuffer, nullptr);
        vkFreeMemory(vulkanDeviceManager.device, scene.vertexBufferMemory, nullptr);
        vkDestroyCommandPool(vulkanDeviceManager.device, vulkanPoolManager.commandPool, nullptr);
        vkDestroyPipeline(vulkanDeviceManager.device, vulkanPipelineManager.graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(vulkanDeviceManager.device, vulkanPipelineManager.pipelineLayout, nullptr);
        vkDestroyRenderPass(vulkanDeviceManager.device, vulkanPipelineManager.renderPass, nullptr);
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
        window = glfwCreateWindow(1920, 1080, "Vulkan", nullptr, nullptr);
    }
};

int main() {
    VulkanInit app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}