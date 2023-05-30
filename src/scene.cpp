// #include "scene.hpp"
// #include <vector>
// #include <iostream>

// #define GLM_FORCE_RADIANS
// #define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

// namespace buffkinz {
//     BuffkinzScene::BuffkinzScene() {
//         camera = BuffkinzCamera::Camera {

//         };
//     }

//     BuffkinzScene::~BuffkinzScene() {

//     }

//     void BuffkinzScene::loadScene(std::vector<std::string> objectFiles) {
//         std::cout << "Object File Count" + std::to_string(objectFiles.size()) + "\n";

//         objects.resize(objectFiles.size());
//         std::vector<BuffkinzModel::Vertex> vertices;
//         std::vector<uint32_t> indices;

//         tinyobj::attrib_t attrib;
//         std::vector<tinyobj::shape_t> shapes;
//         std::vector<tinyobj::material_t> materials;
//         std::string warn, err;

//         for (int i = 0; i < objectFiles.size(); i++) {
//         vertices.clear();
//         indices.clear();
//         shapes.clear();
//         materials.clear();

//         if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objectFiles[i].c_str())) {
//             throw std::runtime_error(warn + err);
//         }

//         for (const auto& shape : shapes) {
//             for (const auto& index : shape.mesh.indices) {
//                 BuffkinzModel::Vertex vertex{};
//                 vertex.position  = {
//                     attrib.vertices[3 * index.vertex_index + 0],
//                     attrib.vertices[3 * index.vertex_index + 1],
//                     attrib.vertices[3 * index.vertex_index + 2]
//                 };

//                 vertex.normal = {
//                     attrib.normals[3 * index.normal_index + 0],
//                     attrib.normals[3 * index.normal_index + 1],
//                     attrib.normals[3 * index.normal_index + 2]
//                 };


//                 // vertex.texCoord = {
//                 //     attrib.texcoords[2 * index.texcoord_index + 0],
//                 //     attrib.texcoords[2 * index.texcoord_index + 1]
//                 // };
                
//                 vertex.color = {(i % 3) * 1.0f, (i % 3) * 1.0f, (i % 3) * 1.0f};
//                 vertices.push_back(vertex);
//                 indices.push_back(i);
//             }
//         }
//         GameObject obj = GameObject();
//                 obj.setMesh(vertices);
//                 obj.setIndices(indices); 
//                 obj.setWorldPosition(glm::vec4(0.0f, 0.0f, (float)i * 10, 1.0)); 
//                 objects[i] = obj;
//                 std::cout << "Object vertex Count" + std::to_string(objects[i].getMesh().size()) + "\n";
//                 std::cout << "Vertex Count" + std::to_string(obj.getMesh().size()) + "\n";
//     }
//   }
// }