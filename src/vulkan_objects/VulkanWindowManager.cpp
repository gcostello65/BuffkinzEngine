//
// Created by Greg Costello on 11/11/24.
//

#include "VulkanWindowManager.h"

void VulkanWindowManager::createSurface(VkInstance &instance, GLFWwindow *window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vulkan window surface");
    }
}
