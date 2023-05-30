// #include <renderer.hpp>

// #include <vector>
// #include <memory>

// namespace buffkinz {
//     void CreateRenderSystem::loadModels() {
//         std::vector<BuffkinzModel::Vertex> vertices;
//         std::vector<uint32_t> indices;

//         tinyobj::attrib_t attrib;
//         std::vector<tinyobj::shape_t> shapes;
//         std::vector<tinyobj::material_t> materials;
//         std::string warn, err;

//         if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
//             throw std::runtime_error(warn + err);
//         }

//         int i = 0;
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
//                 i++;
//                 vertices.push_back(vertex);
//                 indices.push_back(indices.size());
//             }
//         }

//         vertexOffset = i;
//         indexOffset = i;


//         buffkinzModel = std::make_unique<BuffkinzModel>(buffkinzDevice, vertices, indices);
//     }   

//     void CreateRenderSystem::createDescriptorSetLayout() {
//         VkDescriptorSetLayoutBinding uboLayoutBinding{};
//         uboLayoutBinding.binding = 0;
//         uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         uboLayoutBinding.descriptorCount = 1;
//         uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//         uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

//         VkDescriptorSetLayoutCreateInfo layoutInfo{};
//         layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//         layoutInfo.bindingCount = 1;
//         layoutInfo.pBindings = &uboLayoutBinding;

//         if (vkCreateDescriptorSetLayout(buffkinzDevice.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create descriptor set layout!");
//         }
//     }

//     void CreateRenderSystem::createUniformBuffers() {
//         VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//         std::cout << "Image count: " + std::to_string(static_cast<int>(buffkinzSwapChain->imageCount())) + "\n";
//         uniformBuffers.resize(buffkinzSwapChain->imageCount());
//         uniformBuffersMemory.resize(buffkinzSwapChain->imageCount());
//         uniformBuffersMapped.resize(buffkinzSwapChain->imageCount());

//         for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
//         buffkinzDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

//         vkMapMemory(buffkinzDevice.device(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
//         }
//     }

//     void CreateRenderSystem::createPipelineLayout() {
//         VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//         pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//         pipelineLayoutInfo.setLayoutCount = 1;
//         pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
//         pipelineLayoutInfo.pushConstantRangeCount = 0;
//         pipelineLayoutInfo.pPushConstantRanges = nullptr;
//         if (vkCreatePipelineLayout(buffkinzDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create pipeline layout!");
//         }
//     }

//     void CreateRenderSystem::createPipeline() {
//         assert(buffkinzSwapChain != nullptr && "Cannot create pipeline before swap chain");
//         assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

//         PipelineConfigInfo pipelineConfig{};

//         BuffkinzPipeline::defaultPipelineConfigInfo(pipelineConfig);
//         pipelineConfig.renderPass = buffkinzSwapChain->getRenderPass();
//         pipelineConfig.pipelineLayout = pipelineLayout;
//         buffkinzPipeline = std::make_unique<BuffkinzPipeline>(
//             buffkinzDevice,
//             "./shaders/simple_shader.vert.spv",
//             "./shaders/simple_shader.frag.spv",
//             pipelineConfig);
//     }

//     void CreateRenderSystem::recreateSwapChain() {
//         auto extent = buffkinzWindow.getExtent();
//         while (extent.width == 0 || extent.height == 0) {
//             extent = buffkinzWindow.getExtent();
//             glfwWaitEvents();
//         }

//         vkDeviceWaitIdle(buffkinzDevice.device());
//         if (buffkinzSwapChain == nullptr) {
//             buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(buffkinzDevice, extent);
//         } else {
//             buffkinzSwapChain = std::make_unique<BuffkinzSwapChain>(buffkinzDevice, extent, std::move(buffkinzSwapChain));
//             if (buffkinzSwapChain->imageCount() != commandBuffers.size()) {
//                 freeCommandBuffers();
//                 createCommandBuffers();
//             }
//         }  
//         createPipeline();

    
//     }

//     void CreateRenderSystem::freeCommandBuffers() {
//         vkFreeCommandBuffers(buffkinzDevice.device(), buffkinzDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
//         commandBuffers.clear();
//     }

//     void CreateRenderSystem::createCommandBuffers() {
//         commandBuffers.resize(buffkinzSwapChain->imageCount());

//         VkCommandBufferAllocateInfo allocInfo{};
//         allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//         allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//         allocInfo.commandPool = buffkinzDevice.getCommandPool();
//         allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

//         if (vkAllocateCommandBuffers(buffkinzDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
//             throw std::runtime_error("failed to allocate command buffers");
//         }
//     }

//     void CreateRenderSystem::createDescriptorPool() {
//         VkDescriptorPoolSize poolSize{};
//         poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         poolSize.descriptorCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());     

//         VkDescriptorPoolCreateInfo poolInfo{};
//         poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//         poolInfo.poolSizeCount = 1;
//         poolInfo.pPoolSizes = &poolSize;
//         poolInfo.maxSets = static_cast<uint32_t>(buffkinzSwapChain->imageCount());

//         if (vkCreateDescriptorPool(buffkinzDevice.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create descriptor pool!");
//         }

//     }

//     void CreateRenderSystem::createDescriptorSets() {
//         std::vector<VkDescriptorSetLayout> layouts(buffkinzSwapChain->imageCount(), descriptorSetLayout);
//         VkDescriptorSetAllocateInfo allocInfo{};
//         allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//         allocInfo.descriptorPool = descriptorPool;
//         allocInfo.descriptorSetCount = static_cast<uint32_t>(buffkinzSwapChain->imageCount());
//         allocInfo.pSetLayouts = layouts.data();

//         descriptorSets.resize(buffkinzSwapChain->imageCount());
//         if (vkAllocateDescriptorSets(buffkinzDevice.device(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//             throw std::runtime_error("failed to allocate descriptor sets!");
//         }

//         for (size_t i = 0; i < buffkinzSwapChain->imageCount(); i++) {
//             VkDescriptorBufferInfo bufferInfo{};
//             bufferInfo.buffer = uniformBuffers[i];
//             bufferInfo.offset = 0;
//             bufferInfo.range = sizeof(UniformBufferObject);

//             VkWriteDescriptorSet descriptorWrite{};
//             descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//             descriptorWrite.dstSet = descriptorSets[i];
//             descriptorWrite.dstBinding = 0;
//             descriptorWrite.dstArrayElement = 0;

//             descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//             descriptorWrite.descriptorCount = 1;

//             descriptorWrite.pBufferInfo = &bufferInfo;
//             descriptorWrite.pImageInfo = nullptr; // Optional
//             descriptorWrite.pTexelBufferView = nullptr; // Optional

//             vkUpdateDescriptorSets(buffkinzDevice.device(), 1, &descriptorWrite, 0, nullptr);
//         }


//     }
// }