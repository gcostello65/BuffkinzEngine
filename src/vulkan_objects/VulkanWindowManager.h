//
// Created by Greg Costello on 11/11/24.
//

#ifndef BUFFKINZRENDERER_VULKANWINDOWMANAGER_H
#define BUFFKINZRENDERER_VULKANWINDOWMANAGER_H

#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <iostream>

class VulkanWindowManager {
public:
    VkSurfaceKHR surface;

    void createSurface(VkInstance &instance, GLFWwindow *window);
};


#endif //BUFFKINZRENDERER_VULKANWINDOWMANAGER_H
