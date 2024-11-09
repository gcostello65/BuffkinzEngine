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

class HelloTriangleApplication {
public:
    VulkanInstanceManager vulkanInstanceManager;
    VulkanDeviceManager vulkanDeviceManager;
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initVulkan() {
        vulkanInstanceManager.createInstance();
        vulkanInstanceManager.handleMessageCallbacks();
        vulkanDeviceManager.setInstance(vulkanInstanceManager.instance);
        vulkanDeviceManager.pickPhysicalDevice();
        vulkanDeviceManager.createLogicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();
        }
    }

    void cleanup() {
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