//
// Created by Greg Costello on 11/9/24.
//

#include <stdexcept>
#include <vector>
#include <set>
#include "VulkanDeviceManager.h"

void VulkanDeviceManager::pickPhysicalDevice() {
    // Enumerate the available devices: this is just like when we enumerate the extensions. TODO: Read up more on extensions
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If no devices, fail out since nothing can be done without a gpu
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // Otherwise, make an array to hold all of the available devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Iterate through all of the devices and see which ones are suitable. Pick the first suitable one
    for (const auto &device: devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }
}

void VulkanDeviceManager::setInstance(VkInstance &vkInstance) {
    this->instance = vkInstance;
}

// TODO: Make a more robust method for picking the gpu to use in the physical device
bool VulkanDeviceManager::isDeviceSuitable(VkPhysicalDevice device) {

    // In the future this could hold more extension requirements so add here when needed
    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkAvailableExtensions(device, deviceExtensions);

    return indices.isComplete() && extensionsSupported;
}

bool VulkanDeviceManager::checkAvailableExtensions(VkPhysicalDevice &device, std::vector<const char*> const &deviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    // NOTE: Remember that .data() method for a vector gives a pointer to the beginning of the contiguous memory of the array so you can do c style indexing
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionProperties.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // LEFT OFF HERE: about to iterate through all of the device extensions and make sure swapchain is supported.
}

QueueFamilyIndices VulkanDeviceManager::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkBool32 presentQueueEnabled;

    int i = 0;
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // In the future this might be removed since we can query for support of presentation and graphics in the same queue and not have to manage two queues here
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentQueueEnabled);
        if (presentQueueEnabled) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanDeviceManager::createLogicalDevice() {
    // TODO: Consider making indices a class member variable so that it is not instantiated twice. No need for two calls to this method for same device
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueues = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    for (uint32_t queueIndex : uniqueQueues) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfo.queueCount = 1;

        // TODO: Lookup more about the queue priority and when you would want multiple queues in a queue family
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Seting up the struct to hold all the info for creating a physical device
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();

    // Adding the portability extension since this is needed for macOS support
    std::vector<const char *> extensions;
    extensions.push_back("VK_KHR_portability_subset"); // Example additional extension

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = extensions.size();

    // TODO: tutorial says it is best practice to enable the validaiton layers but not needed in the newer versions of vulkan. Determine if I want to leave this or remove
//    if (enableValidationLayers) {
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//    } else {
//        createInfo.enabledLayerCount = 0;
//    }

    // Create the physical device
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // Allocating the device queues
    // NOTE: May be a better way to set these queues up since they may be the same queue, in that case this is not optimal
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

}

void VulkanDeviceManager::setSurface(VkSurfaceKHR &surface) {
    this->surface = surface;
}
