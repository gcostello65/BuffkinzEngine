project(BuffkinzRenderer)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17" )

find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/bin)

file(GLOB SHADERS
        ${SHADER_SOURCE_DIR}/*.vert
        ${SHADER_SOURCE_DIR}/*.frag)

add_executable(${PROJECT_NAME} ${VERT_SHADERS}
        src/main.cpp
        src/vulkan_objects/VulkanInstanceManager.cpp src/vulkan_objects/VulkanInstanceManager.h
        src/vulkan_objects/VulkanDeviceManager.cpp
        src/vulkan_objects/VulkanDeviceManager.h
        src/vulkan_objects/VulkanWindowManager.cpp
        src/vulkan_objects/VulkanWindowManager.h
        src/vulkan_objects/VulkanSwapchainManager.cpp
        src/vulkan_objects/VulkanSwapChainManager.h
        src/vulkan_objects/VulkanPipelineManager.cpp
        src/vulkan_objects/VulkanPipelineManager.h)

find_package(Vulkan REQUIRED)

add_subdirectory(lib/glfw-3.4)
SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")
add_subdirectory(lib/glm      EXCLUDE_FROM_ALL)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} glfw)
endif (VULKAN_FOUND)

execute_process(
        COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        OUTPUT ${SHADER_BINARY_DIR}
        COMMENT "Creating ${SHADER_BINARY_DIR}"
)

foreach(source IN LISTS SHADERS)
    message(STATUS "FOUND SHADER")
    get_filename_component(FILENAME ${source} NAME)
    message(STATUS "FILE NAME: ${FILENAME}, ${SHADER_BINARY_DIR}")
    execute_process(
            COMMAND ${glslc_executable} -c ${SHADER_SOURCE_DIR}/${FILENAME} -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})