
#include "Application.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <stdexcept>
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
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    glEnable(GL_DEPTH_TEST);

    // 2. Initialize Managers
    m_coordinator = std::make_unique<Coordinator>();
    m_assetManager = std::make_unique<AssetManager>();
    m_spaceManager = std::make_unique<SpaceManager>();

    // 3. Register components
    m_coordinator->registerComponent<TransformComponent>();
    m_coordinator->registerComponent<MeshComponent>();
    m_coordinator->registerComponent<CameraComponent>();
    m_coordinator->registerComponent<RigidBodyComponent>();
    m_coordinator->registerComponent<CollisionShapeComponent>();

    // 4. Register systems
    // Corrected RenderSystem registration
    {
        Signature signature;
        signature.set(m_coordinator->getComponentTypeID<TransformComponent>());
        signature.set(m_coordinator->getComponentTypeID<MeshComponent>());
        m_coordinator->registerSystem<RenderSystem>(signature);
        m_renderSystem = m_coordinator->getSystem<RenderSystem>();
    }
    
    // Corrected PhysicsSystem registration
    {
        Signature signature;
        signature.set(m_coordinator->getComponentTypeID<TransformComponent>());
        signature.set(m_coordinator->getComponentTypeID<RigidBodyComponent>());
        signature.set(m_coordinator->getComponentTypeID<CollisionShapeComponent>());
        m_coordinator->registerSystem<PhysicsSystem>(signature);
        m_physicsSystem = m_coordinator->getSystem<PhysicsSystem>();
    }
    
    // Initialize the physics system with the managers it needs
    m_physicsSystem->init(m_coordinator.get(), m_spaceManager.get());


    // 5. Load assets
    m_assetManager->loadShader("phong", "assets/shaders/phong.vert", "assets/shaders/phong.frag");
    m_assetManager->loadMesh("cube", "assets/models/cube.obj"); 
    m_assetManager->loadMesh("plane", "assets/models/SnowTerrain.obj");


    // 6. Set up scene
    m_spaceManager->createSpace(); // Create the main physics space

    m_lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    m_cameraEntity = m_coordinator->createEntity();
    m_coordinator->addComponent(m_cameraEntity, TransformComponent{.position = {0.0f, 2.0f, 10.0f}});
    m_coordinator->addComponent(m_cameraEntity, CameraComponent{
        .projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f)
    });

    // Create a dynamic cube entity
    m_cubeEntity = m_coordinator->createEntity();
    m_coordinator->addComponent(m_cubeEntity, TransformComponent{.position = {0.0f, 5.0f, 0.0f}, .scale = {0.1f, 0.1f, 0.1f}});
    m_coordinator->addComponent(m_cubeEntity, MeshComponent{.meshName = "cube"});
    m_coordinator->addComponent(m_cubeEntity, RigidBodyComponent{.mass = 1.0f});
    m_coordinator->addComponent(m_cubeEntity, CollisionShapeComponent{
        .type = ShapeType::BOX,
        .dimensions = {1.0f, 1.0f, 1.0f}
    });

    // Create a static ground plane entity
    m_groundEntity = m_coordinator->createEntity();
    m_coordinator->addComponent(m_groundEntity, TransformComponent{
        .position = {0.0f, -2.0f, 0.0f},
        .scale = {20.0f, 1.0f, 20.0f}
    });
    m_coordinator->addComponent(m_groundEntity, MeshComponent{.meshName = "plane"});
    m_coordinator->addComponent(m_groundEntity, RigidBodyComponent{.mass = 0.0f}); // Mass of 0 makes it static
    m_coordinator->addComponent(m_groundEntity, CollisionShapeComponent{
        .type = ShapeType::BOX,
        .dimensions = {20.0f, 1.0f, 20.0f}
    });
}

void Application::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    float cameraSpeed = 5.0f * m_deltaTime;
    auto& cameraTransform = m_coordinator->getComponent<TransformComponent>(m_cameraEntity);

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        cameraTransform.position.z -= cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        cameraTransform.position.z += cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        cameraTransform.position.x -= cameraSpeed;
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        cameraTransform.position.x += cameraSpeed;
}

void Application::run() {
    while (!glfwWindowShouldClose(m_window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        processInput();
        m_physicsSystem->update(m_deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto const& cameraComponent = m_coordinator->getComponent<CameraComponent>(m_cameraEntity);
        auto const& cameraTransform = m_coordinator->getComponent<TransformComponent>(m_cameraEntity);
        auto shader = m_assetManager->getShader("phong");

        m_renderSystem->draw(*m_coordinator, *m_assetManager, shader, cameraComponent, cameraTransform, m_lightPos, m_lightColor);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(m_window);
    glfwTerminate();
}