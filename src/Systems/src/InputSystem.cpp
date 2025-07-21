#include "InputSystem.hpp"
#include "Coordinator.hpp"
#include <glm/gtc/matrix_transform.hpp>

void InputSystem::init(Coordinator* coordinator, GLFWwindow* window) {

    this->coordinator = coordinator;
    this->window = window;

    double xpos, ypos;
    glfwGetCursorPos(this->window, &xpos, &ypos);
    m_lastX = xpos;
    m_lastY = ypos;
}

void InputSystem::checkPressedKeys(Entity entity){

    auto& playerControl = coordinator->getComponent<PlayerControlledComponent>(entity);
    for (auto const& pair : playerControl.keyMap) {
        playerControl.actionState[pair.second] = (glfwGetKey(window, pair.first) == GLFW_PRESS);
    }
}


void InputSystem::update(float deltaTime) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouseMovement) {
        m_lastX = xpos;
        m_lastY = ypos;
        firstMouseMovement = false;
    }

    m_deltaX = xpos - m_lastX;
    m_deltaY = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    for(const Entity entity : entitySet){

        //set all keys to false so it can be saved for the player input system
        PlayerControlledComponent playerControl = coordinator->getComponent<PlayerControlledComponent>(entity);
        for(auto& it : playerControl.actionState){
            it.second = false;
        }

        //lopp the mapped keys and check if they are pressed, save it for the control system
        for(auto const& it : playerControl.keyMap){
            checkPressedKeys(entity);  
        }
    }
}
        
