#pragma once

#include "Coordinator.hpp"
#include "AssetManager.hpp"
#include "SpaceManager.hpp"
#include "RenderSystem.hpp"
#include "PhysicsSystem.hpp"
#include "InputSystem.hpp"
#include "PlayerControlSystem.hpp"
#include <memory>
#include <glm/glm.hpp>

struct GLFWwindow;

class Application {
public:
    Application();
    void run();

private:
    void init();

    GLFWwindow* window;
    std::unique_ptr<Coordinator> coordinator;
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<SpaceManager> spaceManager;

    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<PlayerControlSystem> playerControlSystem;
    
    Entity cameraEntity;
    Entity cubeEntity;
    Entity groundEntity;

    glm::vec3 lightPos;
    glm::vec3 lightColor;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};