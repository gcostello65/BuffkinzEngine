//
// Created by Greg Costello on 12/1/24.
//

#include "Scene.h"
#include "assimp/postprocess.h"
#include <iostream>

void Scene::loadScene(std::string filepath) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);

    // TODO: make this more robust and import the whole tree. Our mesh is all in the same node so we don't need to do any recursion for the scene tree
    meshes.resize(scene->mNumMeshes);
    materials.resize(scene->mNumMaterials);

    for (int i = 0; i < scene->mNumMeshes; i++) {
//        std::cout << scene->mMeshes[i]->mVertices[0].x << std::endl;
        meshes[i] = scene->mMeshes[i];
    }

    for (int i = 0; i < scene->mNumMaterials; i++) {
//        std::cout << "Here is the scene: " << scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS) << std::endl;
        materials[i] = scene->mMaterials[i];
    }

    vertices = {
            {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}}
    };

    indices = {
            0, 1, 2, 2, 3, 0
    };

    //uint32_t vertexCount, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory, const std::vector<Vertex>* vertices
    vulkanBufferManager->createVertexBuffer(vertices.size(), &vertexBuffer, &vertexBufferMemory, &vertices);
    vulkanBufferManager->createIndexBuffer(indices.size(), &indexBuffer, &indexBufferMemory, &indices);

}
