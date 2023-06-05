#pragma once
#include <glm/glm.hpp>

namespace buffkinz {
    class Scene {
        public: 
            struct Camera {
                glm::vec3 position;
                glm::vec3 lookDir;
                glm::vec3 up;
                glm::mat4 viewMat;
                float rotAngle;
                glm::vec3 rotationAxis;
            };
    };
}