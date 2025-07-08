#pragma once

#include "Coordinator.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "RenderSystem.hpp"
#include "PhysicsSystem.hpp"
#include <memory>
#include <map>

// Forward declare GLFWwindow
struct GLFWwindow;

// Temporary global asset manager. In a real engine, this would be a dedicated class.
extern std::map<AssetID, std::shared_ptr<Mesh>> g_meshCache;

class Application {
public:
    Application();
    void run();

private:
    void init(); // All setup logic will go here

    GLFWwindow* m_window;
    std::unique_ptr<Coordinator> m_coordinator;
    std::shared_ptr<RenderSystem> m_renderSystem;
    std::shared_ptr<PhysicsSystem> m_physicsSystem;
    std::shared_ptr<Shader> m_shader;

    // Add a member to store the camera's entity ID
    Entity m_cameraEntity;

    // Timing
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
};