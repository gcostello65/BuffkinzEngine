#include "user_input_controller.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include<glm/common.hpp>
#define GLM_FORCE_RADIANS



namespace buffkinz {

    void UserInputController::keyMovement(GLFWwindow* window, Scene::Camera& camera) {

        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
                // std::cout << "start: " << camera.lookDir.x << " : " << camera.lookDir.y << " : " << camera.lookDir.z << " : " <<std::endl;

        std::cout << "xpos: " << xpos << "ypos: " << ypos <<std::endl;

        if ((float)xpos != 0.0f || (float)ypos != 0.0f) {

        glm::quat currentLook = glm::quat(0.0f,  camera.lookDir.x,  camera.lookDir.y, camera.lookDir.z);

        glm::vec3 displacement = -glm::normalize(glm::cross(camera.lookDir, camera.up)) * (float)xpos +
                glm::normalize(glm::cross(glm::cross(camera.lookDir, camera.up), camera.lookDir)) * (float)ypos;

        glm::vec3 rotationAxis = glm::normalize(glm::cross(camera.lookDir, displacement));

        float rotationAngle = 0.02 * glm::dot(displacement + camera.lookDir, camera.lookDir);

        glm::quat rotationQuat = glm::angleAxis(rotationAngle, rotationAxis);

        glm::quat result = glm::inverse(rotationQuat) * currentLook * rotationQuat;

        camera.lookDir = glm::vec3(result.x, result.y, result.z);
        }
        glfwSetCursorPos(window, 0, 0);
        
        if (glfwGetKey(window, inputs.moveLeft)) {
            camera.position = camera.position - glm::normalize(glm::cross(camera.lookDir, camera.up)) * moveSpeed;
        }

        if (glfwGetKey(window, inputs.moveRight)) {
            camera.position = camera.position + glm::normalize(glm::cross(camera.lookDir, camera.up)) * moveSpeed;
        }

        if (glfwGetKey(window, inputs.moveForward)) {
            camera.position = camera.position + camera.lookDir * moveSpeed;
        }

        if (glfwGetKey(window, inputs.moveBack)) {
            camera.position = camera.position - camera.lookDir * moveSpeed;
        }

        if (glfwGetKey(window, inputs.moveUp)) {
            camera.position = camera.position + glm::normalize(glm::cross(glm::cross(camera.lookDir, camera.up), camera.lookDir)) * moveSpeed;
        }

        if (glfwGetKey(window, inputs.moveDown)) {
            camera.position = camera.position - glm::normalize(glm::cross(glm::cross(camera.lookDir, camera.up), camera.lookDir)) * moveSpeed;
        }
    }
}