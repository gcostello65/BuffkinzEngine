//
// Created by Greg Costello on 11/11/24.
//

#include "VulkanSwapChainManager.h"

void VulkanSwapChainManager::createSwapChain() {
    SwapChainSupportDetails details = querySwapChainSupport();

    // Pick the relevant present mode, format, and extent from the details queried above
    VkPresentModeKHR presentModeKhr = choosePresentMode(details.presentModes);
    VkSurfaceFormatKHR formatKhr = chooseSwapChainFormat(details.formats);
    VkExtent2D extent = chooseExtent(details.capabilities);

    // Set the details for use later
    swapChainImageFormat = formatKhr.format;
    swapChainExtent = extent;

    uint32_t imageCount = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = formatKhr.format;
    createInfo.imageColorSpace = formatKhr.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;

    // TODO: Look into using VK_IMAGE_USAGE_TRANSFER_DST_BIT in order to do some post processing rather than writing directly to the image
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = vulkanDeviceManager->findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        // Lookup why we need to specify the index count and the indices here
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = details.capabilities.currentTransform;
    // If we need to use the alpha channel, look at this again
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentModeKhr;

    // TODO: check if I need clipping here
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    gatherSwapChainImages(imageCount);
}

void VulkanSwapChainManager::gatherSwapChainImages(uint32_t imageCount) {
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}

SwapChainSupportDetails VulkanSwapChainManager::querySwapChainSupport() {
    SwapChainSupportDetails details;

    // Query for the basic surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    //vkGetPhysicalDeviceSurfaceFormatsKHR
    // Querying for the surface formats. This follows the typical struct init flow
    uint32_t surfaceFormatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, nullptr);

    if (surfaceFormatsCount != 0) {
        details.formats.resize(surfaceFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatsCount, details.formats.data());
    }
    //vkGetPhysicalDeviceSurfacePresentModesKHR
    // Querying for the surface formats. This follows the typical struct init flow
    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);

    if (surfaceFormatsCount != 0) {
        details.presentModes.resize(surfaceFormatsCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, details.presentModes.data());
    }

    if (details.formats.empty() || details.presentModes.empty()) {
        throw std::runtime_error("The physicalDevice cannot handle swapchain, cannot move forward for window rendering");
    }

    return details;
}

VkSurfaceFormatKHR VulkanSwapChainManager::chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const VkSurfaceFormatKHR format : availableFormats) {
        // Pick the ideal format which is most widely used...
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    // ... or just pick the first available format and call it a day for POC
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChainManager::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    // Up here we can pick our preferred present mode which may change in the future but this is fine right now
    for (const VkPresentModeKHR &presentMode : availablePresentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    // This is the only present mode that is guaranteed
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChainManager::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    //TODO: Study this method, I should learn more about the window sizing and how it plays with the swapchain. For now just copying from vulkan tutorial
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanSwapChainManager::setPhysicalDevice(VkPhysicalDevice &device) {
    this->physicalDevice = device;
}
void VulkanSwapChainManager::setDevice(VkDevice &device) {
    this->device = device;
}
void VulkanSwapChainManager::setSurface(VkSurfaceKHR &surface) {
    this->surface = surface;
}

void VulkanSwapChainManager::setWindow(GLFWwindow *window) {
    this->window = window;
}

void VulkanSwapChainManager::setDeviceManager(VulkanDeviceManager *vulkanDeviceManager) {
    this->vulkanDeviceManager = vulkanDeviceManager;
}

void VulkanSwapChainManager::createImageViews() {
    imageViews.resize(swapChainImages.size());

    int i = 0;
    for (const auto& image : swapChainImages) {
        VkImageViewCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;

        // The viewtype tells what the image looks like. 1D, 2D, 3D, cubemap, etc
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        // These fields allow us to swizzle the colors, we probably don't want to do this now. Might be worth looking into later
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Here is where we take care of things like how the image is used (depth buffering and stencil buffers will be flags here)
        // Also this is where mipmapping is setup so this will be updated later
        //TODO: Look into this struct and what are possible options for it
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Could not create the image views for the swapchain images");
        }
        i++;
    }
}
