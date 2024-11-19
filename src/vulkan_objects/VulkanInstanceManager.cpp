//
// Created by Greg Costello on 11/6/24.
//

#include "VulkanInstanceManager.h"
#include <stdexcept>
#include <iostream>

void VulkanInstanceManager::manageValidationLayers(VkInstanceCreateInfo &createInfo) {
    static const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    if (enableValidationLayers && !checkValidationLayerSupport(validationLayers)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
}

bool VulkanInstanceManager::checkValidationLayerSupport(const std::vector<const char *> &validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName: validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            std::cout << layerProperties.layerName << std::endl;
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanInstanceManager::createInstance() {

    // Setting the application info, this may be configured further in the future
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Rendering Playground";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Render Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Instance create info which will hold the information to instantiate the instance
    // TODO: Look into what vulkan extensions are and how they are used. Also look up custom allocator callbacks
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Specify your additional extensions
    // Look into why this works and the specific extensions in vulkan
    // This is needed in order to allow vulkan to use the moltenvk driver which is now portable? Look into this
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back("VK_KHR_portability_enumeration"); // Example additional extension
    extensions.push_back("VK_KHR_get_physical_device_properties2"); // Example additional extension

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    manageValidationLayers(createInfo);

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    // This is for debugging purposes
    uint32_t apiVersion;

    vkEnumerateInstanceVersion(&apiVersion);
    std::cout << "Supported Vulkan API version: "
              << VK_VERSION_MAJOR(apiVersion) << "."
              << VK_VERSION_MINOR(apiVersion) << "."
              << VK_VERSION_PATCH(apiVersion) << std::endl;

    std::cout << "Here is the code for result: " << result << std::endl;
    // End debug

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanInstanceManager::handleMessageCallbacks() {
    // TODO: Go back to the Message Callback handling in the vulkan tutorial to make the validation layers more specific rather than spam
}