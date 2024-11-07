//
// Created by Greg Costello on 11/6/24.
//

#ifndef BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H
#define BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


class VulkanInstanceManager {
public:
    VkInstance instance;
    void createInstance();
    GLFWwindow* glfWwindow;
};


#endif //BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H
