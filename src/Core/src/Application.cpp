#include "Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>

// Define the global mesh cache
std::map<AssetID, std::shared_ptr<Mesh>> g_meshCache;


Application::Application() {
    init();
}

void Application::init() {
    // 1. Initialize GLFW and create a window
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1280, 720, "Superposition Engine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);

    // 2. Initialize the Coordinator and register everything
    m_coordinator = std::make_unique<Coordinator>();

    // Register components
    m_coordinator->registerComponent<TransformComponent>();
    m_coordinator->registerComponent<MeshComponent>();
    m_coordinator->registerComponent<CameraComponent>();
    m_coordinator->registerComponent<RigidBodyComponent>();

    // Register RenderSystem
    {
        Signature signature;
        signature.set(m_coordinator->getComponentTypeID<TransformComponent>());
        signature.set(m_coordinator->getComponentTypeID<MeshComponent>());
        m_coordinator->registerSystem<RenderSystem>(signature);
        m_renderSystem = m_coordinator->getSystem<RenderSystem>();
    }

    // Register PhysicsSystem
    {
        Signature signature;
        signature.set(m_coordinator->getComponentTypeID<TransformComponent>());
        signature.set(m_coordinator->getComponentTypeID<RigidBodyComponent>());
        m_coordinator->registerSystem<PhysicsSystem>(signature);
        m_physicsSystem = m_coordinator->getSystem<PhysicsSystem>();
    }

    // 3. Load assets (shaders and meshes)
    m_shader = std::make_shared<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    // Create a cube mesh and add it to our cache
    std::vector<Vertex> cubeVertices = {
        // positions          // normals           // texture coords
		{{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},

		{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

		{{ 0.5f,  0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},

		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},

		{{-0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
    };
    std::vector<unsigned int> cubeIndices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
    g_meshCache[0] = std::make_shared<Mesh>(cubeVertices, cubeIndices);


    // 4. Create entities and add components
    
    // Create a camera entity and store its ID
    m_cameraEntity = m_coordinator->createEntity();
    m_coordinator->addComponent(m_cameraEntity, TransformComponent{
        .position = {0.0f, 0.0f, 3.0f}
    });
    m_coordinator->addComponent(m_cameraEntity, CameraComponent{
        .projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f),
        .primary = true // Corrected member name
    });

    // Create a renderable entity
    Entity cube = m_coordinator->createEntity();
    m_coordinator->addComponent(cube, TransformComponent{
        .position = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f}
    });
    m_coordinator->addComponent(cube, MeshComponent{
        .meshID = 0, // ID of the cube mesh in our cache
        .materialID = 0
    });
}

void Application::run() {
    while (!glfwWindowShouldClose(m_window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // --- Update ---
        m_physicsSystem->update(m_deltaTime);

        // --- Render ---
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get camera data using the stored entity ID
        auto const& cameraComponent = m_coordinator->getComponent<CameraComponent>(m_cameraEntity);
        auto const& cameraTransform = m_coordinator->getComponent<TransformComponent>(m_cameraEntity);

        m_renderSystem->draw(*m_coordinator, m_shader, cameraComponent, cameraTransform);

        // Swap buffers and poll events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
