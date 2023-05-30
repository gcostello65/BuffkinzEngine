#pragma once

#include "buffkinz_window.hpp"
#include "buffkinz_model.hpp"
#include "scene.hpp"

namespace buffkinz {
    class UserInputController {
        public:
        struct InputTypes {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveBack = GLFW_KEY_S;
            int moveForward = GLFW_KEY_W;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
        };

        float moveSpeed = 0.1f;

        InputTypes inputs{};
        void keyMovement(GLFWwindow* window, Scene::Camera& camera);
        };
        
}