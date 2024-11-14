//
// Created by Greg Costello on 11/14/24.
//

#ifndef BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
#define BUFFKINZRENDERER_VULKANCOMMANDMODULE_H

#include <vulkan/vulkan_core.h>
#include <stdexcept>

class VulkanCommandModule {
public:
    void recordCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex);
private:
};


#endif //BUFFKINZRENDERER_VULKANCOMMANDMODULE_H
