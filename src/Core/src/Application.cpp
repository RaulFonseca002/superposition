#include <glad/glad.h>

#include "Application.hpp"
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
    if (!glfwInit()) { throw std::runtime_error("Failed to initialize GLFW"); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Superposition Engine", nullptr, nullptr);
    if (!window) { glfwTerminate(); throw std::runtime_error("Failed to create GLFW window"); }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { throw std::runtime_error("Failed to initialize GLAD"); }
    
    glEnable(GL_DEPTH_TEST);

    coordinator = std::make_unique<Coordinator>();
    assetManager = std::make_unique<AssetManager>();
    spaceManager = std::make_unique<SpaceManager>();

    coordinator->registerComponent<TransformComponent>();
    coordinator->registerComponent<MeshComponent>();
    coordinator->registerComponent<CameraComponent>();
    coordinator->registerComponent<RigidBodyComponent>();
    coordinator->registerComponent<CollisionShapeComponent>();
    coordinator->registerComponent<PlayerControlledComponent>();

    // Register systems
    {
        Signature signature;
        signature.set(coordinator->getComponentTypeID<TransformComponent>());
        signature.set(coordinator->getComponentTypeID<MeshComponent>());
        coordinator->registerSystem<RenderSystem>(signature);
    }
    renderSystem = coordinator->getSystem<RenderSystem>();

    {
        Signature signature;
        signature.set(coordinator->getComponentTypeID<TransformComponent>());
        signature.set(coordinator->getComponentTypeID<RigidBodyComponent>());
        signature.set(coordinator->getComponentTypeID<CollisionShapeComponent>());
        coordinator->registerSystem<PhysicsSystem>(signature);
    }
    physicsSystem = coordinator->getSystem<PhysicsSystem>();

    {
        Signature signature;
        signature.set(coordinator->getComponentTypeID<PlayerControlledComponent>());
        signature.set(coordinator->getComponentTypeID<TransformComponent>());
        signature.set(coordinator->getComponentTypeID<CameraComponent>());
        coordinator->registerSystem<InputSystem>(signature);
    }
    inputSystem = coordinator->getSystem<InputSystem>();
    
    // Initialize systems with the pointers they need
    physicsSystem->init(coordinator.get(), spaceManager.get());
    inputSystem->init(coordinator.get(), window);

    assetManager->loadShader("phong", "assets/shaders/phong.vert", "assets/shaders/phong.frag");
    assetManager->loadMesh("cube", "assets/models/cube.obj"); 
    assetManager->loadMesh("plane", "assets/models/SnowTerrain.obj");

    spaceManager->createSpace();

    lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    cameraEntity = coordinator->createEntity();
    coordinator->addComponent(cameraEntity, TransformComponent{.position = {0.0f, 2.0f, 10.0f}});
    coordinator->addComponent(cameraEntity, CameraComponent{
        .projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f)
    });
    coordinator->addComponent(cameraEntity, PlayerControlledComponent{});

    cubeEntity = coordinator->createEntity();
    coordinator->addComponent(cubeEntity, TransformComponent{.position = {0.0f, 5.0f, 0.0f}});
    coordinator->addComponent(cubeEntity, MeshComponent{.meshName = "cube"});
    coordinator->addComponent(cubeEntity, RigidBodyComponent{.mass = 1.0f});
    coordinator->addComponent(cubeEntity, CollisionShapeComponent{.type = ShapeType::BOX, .dimensions = {0.5f, 0.5f, 0.5f}});

    groundEntity = coordinator->createEntity();
    coordinator->addComponent(groundEntity, TransformComponent{.position = {0.0f, -0.5f, 0.0f}, .scale = {20.0f, 0.5f, 20.0f}});
    coordinator->addComponent(groundEntity, MeshComponent{.meshName = "plane"});
    coordinator->addComponent(groundEntity, RigidBodyComponent{.mass = 0.0f});
    coordinator->addComponent(groundEntity, CollisionShapeComponent{.type = ShapeType::BOX, .dimensions = {20.0f, 0.5f, 20.0f}});
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // The InputSystem now handles all input processing
        inputSystem->update(deltaTime);
        physicsSystem->update(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto const& cameraComponent = coordinator->getComponent<CameraComponent>(cameraEntity);
        auto const& cameraTransform = coordinator->getComponent<TransformComponent>(cameraEntity);
        auto shader = assetManager->getShader("phong");

        renderSystem->draw(*coordinator, *assetManager, shader, cameraComponent, cameraTransform, lightPos, lightColor);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
