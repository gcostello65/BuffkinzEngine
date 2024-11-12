project(BuffkinzRenderer)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17" )

#set(VK_ICD_FILENAMES "vulkansdk/macOS/share/vulkan/icd.d/MoltenVK_icd.json")

find_package(Vulkan REQUIRED)

add_subdirectory(lib/glfw-3.4)
SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")
add_subdirectory(lib/glm      EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/vulkan_objects/VulkanInstanceManager.cpp src/vulkan_objects/VulkanInstanceManager.h
        src/vulkan_objects/VulkanDeviceManager.cpp
        src/vulkan_objects/VulkanDeviceManager.h
        src/vulkan_objects/VulkanWindowManager.cpp
        src/vulkan_objects/VulkanWindowManager.h
        src/vulkan_objects/VulkanSwapchainManager.cpp
        src/vulkan_objects/VulkanSwapChainManager.h)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)
