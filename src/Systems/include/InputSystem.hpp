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

        /* i prefear for data to be saved as components
        but mouse pos is equal to every entity for each frame 
        so it would be redundant to store it as a component 
        and inputSystem is the sole responsable for listening to the hardwere
        so i am putthing this here becuse i am a slave of the principals i choose to apply*/

        float m_lastX = 0.0f;
        float m_lastY = 0.0f;
        float m_deltaX = 0.0f;
        float m_deltaY = 0.0f;
        bool firstMouseMovement = true;

        void checkPressedKeys(Entity entity);


    public:
        
        void onEntityAdded(Entity entity) override {};
        void onEntityRemoved(Entity entity) override {};
        void update(float deltaTime);
        void init(Coordinator* coordinator, GLFWwindow* window);
        float getMouseDeltaX() { return m_deltaX; };
        float getMouseDeltaY() { return m_deltaY; };
};
