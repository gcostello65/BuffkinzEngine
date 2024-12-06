//
// Created by Greg Costello on 12/1/24.
//

#ifndef BUFFKINZRENDERER_SCENE_H
#define BUFFKINZRENDERER_SCENE_H

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <vulkan/vulkan_core.h>

#include "../vulkan_objects/VulkanBufferManager.h"

class Scene {
public:
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VulkanBufferManager* vulkanBufferManager;

    // TEMPORARY VARIABLE
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::vector<aiMesh*> meshes;
    std::vector<aiMaterial*> materials;

    void loadScene(std::string filepath);
};


#endif //BUFFKINZRENDERER_SCENE_H
