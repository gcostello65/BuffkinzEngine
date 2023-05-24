#include <buffkinz_window.hpp>
#include <stdexcept>

namespace buffkinz {

    BuffkinzWindow::BuffkinzWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    BuffkinzWindow::~BuffkinzWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void BuffkinzWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void BuffkinzWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void BuffkinzWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto buffkinzWindow = reinterpret_cast<BuffkinzWindow *>(glfwGetWindowUserPointer(window));
        buffkinzWindow->framebufferResized = true;
        buffkinzWindow->width = width;
        buffkinzWindow->height = height;
    }

} 