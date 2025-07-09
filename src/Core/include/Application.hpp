#pragma once

#include "Coordinator.hpp"
#include "AssetManager.hpp"
#include "RenderSystem.hpp"
#include "PhysicsSystem.hpp"
#include <memory>
#include <glm/glm.hpp>

struct GLFWwindow;

class Application {
public:
    Application();
    void run();

private:
    void init();
    void processInput();

    GLFWwindow* m_window;
    std::unique_ptr<Coordinator> m_coordinator;
    std::unique_ptr<AssetManager> m_assetManager;

    std::shared_ptr<RenderSystem> m_renderSystem;
    std::shared_ptr<PhysicsSystem> m_physicsSystem;
    
    Entity m_cameraEntity;
    Entity m_cubeEntity;

    glm::vec3 m_lightPos;
    glm::vec3 m_lightColor;

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
};