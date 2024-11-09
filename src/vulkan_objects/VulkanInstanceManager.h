//
// Created by Greg Costello on 11/6/24.
//

#ifndef BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H
#define BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>


class VulkanInstanceManager {
public:
    VkInstance instance;

    void createInstance();
    void handleMessageCallbacks();

    GLFWwindow *glfWwindow;


private:
    static void manageValidationLayers(VkInstanceCreateInfo &createInfo);

    static bool checkValidationLayerSupport(const std::vector<const char*> &validationLayers);
};


#endif //BUFFKINZRENDERER_VULKANINSTANCEMANAGER_H
