#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace buffkinz {

    class BuffkinzWindow {
        public:
            BuffkinzWindow(int w, int h, std::string name);
            ~BuffkinzWindow();

            BuffkinzWindow(const BuffkinzWindow &) = delete;
            BuffkinzWindow &operator = (const BuffkinzWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); };
            VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

            bool wasWindowResized() {return framebufferResized;}

            void resetWindowResizedFlag() {framebufferResized = false;}

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
            GLFWwindow* getWindow() {
                return window;
            }

        private:
            static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
            void initWindow();

            int width;
            int height;
            bool framebufferResized = false;

            std::string windowName;
            
            GLFWwindow *window;
    };
}