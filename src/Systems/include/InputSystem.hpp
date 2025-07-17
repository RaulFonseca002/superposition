#pragma once

#include "System.hpp"
#include "Types.hpp"
#include "Coordinator.hpp"
#include <GLFW/glfw3.h>

class Coordinator;
struct GLFWwindow;

class InputSystem : public System {

    private:

        Coordinator* coordinator = nullptr;
        GLFWwindow* window = nullptr;

        // State for mouse-look
        float m_lastX = 0.0f;
        float m_lastY = 0.0f;
        bool firstMouseMovement = true;

        void processCameraInput(Entity entity, float deltaTime);


    public:
        InputSystem() = default;
        void onEntityAdded(Entity entity) override {};
        void onEntityRemoved(Entity entity) override {};
        void update(float deltaTime);
        void init(Coordinator* coordinator, GLFWwindow* window);
};
