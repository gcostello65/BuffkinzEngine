//
// Created by Greg Costello on 5/30/23.
//
#pragma once

#include "buffkinz_model.hpp"
#include <memory>
namespace buffkinz {

    class GameObject {
    public:
        struct UniformBufferObject {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
            glm::mat4 lightTransform;
            glm::vec3 viewDir;
        };
        using id_t = unsigned int;

        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        id_t getId() {return id;}

        std::shared_ptr<BuffkinzModel> model{};
        glm::vec3 position{};
        glm::vec3 color{};
        UniformBufferObject ubo;


        GameObject(const GameObject &) = delete;
        GameObject &operator = (const GameObject &) = delete;
        GameObject(GameObject &&) = default;
        GameObject &operator = (GameObject &&) = default;

    private:
        GameObject(id_t objId) :id{objId} {}
        id_t id;
    };

} // buffkinz

