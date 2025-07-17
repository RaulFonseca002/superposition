#include "InputSystem.hpp"
#include "Coordinator.hpp"
#include <glm/gtc/matrix_transform.hpp>

void InputSystem::init(Coordinator* coordinator, GLFWwindow* window) {
    
    this->coordinator = coordinator;
    this->window = window;
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    m_lastX = static_cast<float>(xpos);
    m_lastY = static_cast<float>(ypos);
}

void InputSystem::update(float deltaTime) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    for (auto const& entity : entitySet) {
        
        if(coordinator->hasComponent<CameraComponent>(entity)){
            processCameraInput(entity, deltaTime);
        }
        
    }
}

void InputSystem::processCameraInput(Entity entity, float deltaTime) {
    // --- Mouse Look ---
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouseMovement) {
        m_lastX = xpos;
        m_lastY = ypos;
        firstMouseMovement = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    auto& camera = coordinator->getComponent<CameraComponent>(entity);
    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;

    // --- Keyboard Movement ---
    float cameraSpeed = 5.0f * deltaTime;
    auto& transform = coordinator->getComponent<TransformComponent>(entity);
    
    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    glm::vec3 cameraFront = glm::normalize(front);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        transform.position += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        transform.position -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        transform.position -= cameraRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        transform.position += cameraRight * cameraSpeed;
}