//
// Created by Greg Costello on 11/11/24.
//

#ifndef BUFFKINZRENDERER_VULKANSWAPCHAINMANAGER_H
#define BUFFKINZRENDERER_VULKANSWAPCHAINMANAGER_H

#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>
#include "VulkanDeviceManager.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChainManager {
public:
    // Members
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> imageViews;

    // Methods
    void createSwapChain();
    void createImageViews();
    SwapChainSupportDetails querySwapChainSupport();
    void setPhysicalDevice(VkPhysicalDevice &device);
    void setDevice(VkDevice &device);
    void setSurface(VkSurfaceKHR &surface);
    void setWindow(GLFWwindow *window);
    void setDeviceManager(VulkanDeviceManager *vulkanDeviceManager);
    VkSurfaceFormatKHR chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
private:
    // Members
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    GLFWwindow *window;
    VulkanDeviceManager *vulkanDeviceManager;

    //Methods
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void gatherSwapChainImages(uint32_t imageCount);
};


#endif //BUFFKINZRENDERER_VULKANSWAPCHAINMANAGER_H
