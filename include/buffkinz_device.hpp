#pragma once

#include <buffkinz_window.hpp>

// std lib headers
#include <string>
#include <vector>

namespace buffkinz {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;

        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class BuffkinzDevice {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        BuffkinzDevice(BuffkinzWindow &window);

        ~BuffkinzDevice();

        // Not copyable or movable
        BuffkinzDevice(const BuffkinzDevice &) = delete;

        BuffkinzDevice &operator=(const BuffkinzDevice &) = delete;

        BuffkinzDevice(BuffkinzDevice &&) = delete;

        BuffkinzDevice &operator=(BuffkinzDevice &&) = delete;

        VkImage getTextureImage() {
            return textureImage;
        }

        VkCommandPool getCommandPool() { return commandPool; }

        VkDevice device() { return device_; }

        VkSurfaceKHR surface() { return surface_; }

        VkQueue graphicsQueue() { return graphicsQueue_; }

        VkQueue presentQueue() { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

        VkFormat findSupportedFormat(
                const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void copyBufferToImage(
                VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createTextureImage();

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        void
        createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);


        void createImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);

        void* getTextureData() {return textureData;}

        VkPhysicalDeviceProperties properties;

        VkPhysicalDevice getPhysicalDevice() {return physicalDevice;}

        VkBuffer textureStagingBuffer;
        VkDeviceMemory textureStagingBufferMemory;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;


    private:
        void createInstance();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);

        std::vector<const char *> getRequiredExtensions();

        bool checkValidationLayerSupport();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        void hasGflwRequiredInstanceExtensions();

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        BuffkinzWindow &window;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        void* textureData;

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                            "VK_KHR_portability_subset"};
    };

}  // namespace buffkinz