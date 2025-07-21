#include "PlayerControlSystem.hpp"
#include "Coordinator.hpp"

void PlayerControlSystem::init(Coordinator* coordinator, InputSystem* inputSystem) {
    this->coordinator = coordinator;
    this->inputSystem = inputSystem;
}


void PlayerControlSystem::update(float deltaTime) {

    for (Entity const& entity : entitySet) {

        PlayerControlledComponent const& playerControl = coordinator->getComponent<PlayerControlledComponent>(entity);
        TransformComponent& transform = coordinator->getComponent<TransformComponent>(entity);

        if (coordinator->hasComponent<CameraComponent>(entity)) {
            processCameraLook(entity);
        }

        if (coordinator->hasComponent<RigidBodyComponent>(entity)) {
            processMovementBody(transform, playerControl ,coordinator->getComponent<RigidBodyComponent>(entity));
        }
        else {
            processMovementFlight(transform, playerControl ,deltaTime);            
        }
    }
}

void PlayerControlSystem::processCameraLook(Entity entity) {

    auto& camera = coordinator->getComponent<CameraComponent>(entity);
    auto& transform = coordinator->getComponent<TransformComponent>(entity);
    float xoffset = inputSystem->getMouseDeltaX() * camera.sensitivity;
    float yoffset = inputSystem->getMouseDeltaY() * camera.sensitivity;
    glm::vec3 front;

    camera.yaw += xoffset;
    camera.pitch += yoffset;
    
    camera.yaw = fmod(camera.yaw, 360.0f);

    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;

    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    transform.front = glm::normalize(front);
    transform.right = glm::normalize(glm::cross(transform.front, glm::vec3(0.0f, 1.0f, 0.0f)));
    transform.up = glm::normalize(glm::cross(transform.right, transform.front));

}

void PlayerControlSystem::processMovementFlight(TransformComponent& transform, 
                                                PlayerControlledComponent playerControl, 
                                                float deltaTime) {

    float speed = 5.0f * deltaTime;

    if (playerControl.actionState.at(PlayerAction::MOVE_FORWARD)) 
        transform.position += transform.front * speed;
    if (playerControl.actionState.at(PlayerAction::MOVE_BACK)) 
        transform.position -= transform.front * speed;
    if (playerControl.actionState.at(PlayerAction::MOVE_LEFT)) 
        transform.position -= transform.right * speed;
    if (playerControl.actionState.at(PlayerAction::MOVE_RIGHT)) 
        transform.position += transform.right * speed;
}

void PlayerControlSystem::processMovementBody(  TransformComponent& transform, 
                                                PlayerControlledComponent const& playerControl,
                                                RigidBodyComponent& rigidBody
                                                ){


        if (playerControl.actionState.at(PlayerAction::MOVE_FORWARD)) 
            rigidBody.force += glm::vec3(transform.front.x, 0, transform.front.z) * rigidBody.forceStrength;
        if (playerControl.actionState.at(PlayerAction::MOVE_BACK)) 
            rigidBody.force += glm::vec3(-transform.front.x, 0, -transform.front.z) * rigidBody.forceStrength;
        if (playerControl.actionState.at(PlayerAction::MOVE_LEFT)) 
            rigidBody.force += glm::vec3(-transform.right.x, 0, -transform.right.z) * rigidBody.forceStrength;
        if (playerControl.actionState.at(PlayerAction::MOVE_RIGHT)) 
            rigidBody.force += glm::vec3(transform.right.x, 0, transform.right.z) * rigidBody.forceStrength;
}

void PlayerControlSystem::addKeyBinding(Entity entity, std::pair<int, PlayerAction> keyActionPair){

    PlayerControlledComponent& playerControl = coordinator->getComponent<PlayerControlledComponent>(entity);
    playerControl.keyMap.insert(keyActionPair);
}
