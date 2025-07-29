
#include "Application.hpp"
#include "Mesh.hpp"
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
        coordinator->registerSystem<InputSystem>(signature);
    }
    inputSystem = coordinator->getSystem<InputSystem>();

    {
        Signature signature;
        signature.set(coordinator->getComponentTypeID<PlayerControlledComponent>());
        signature.set(coordinator->getComponentTypeID<TransformComponent>());
        coordinator->registerSystem<PlayerControlSystem>(signature);
    }
    playerControlSystem = coordinator->getSystem<PlayerControlSystem>();

    // Initialize systems
    physicsSystem->init(coordinator.get(), spaceManager.get());
    inputSystem->init(coordinator.get(), window);
    playerControlSystem->init(coordinator.get(), inputSystem.get());

    // Load the new PBR shader
    assetManager->loadShader("pbr", "assets/shaders/pbr.vert", "assets/shaders/pbr.frag");

    // Load the scenes from your .gltf files
    assetManager->loadScene("platform", "assets/models/Platform_2x2_Empty.gltf", *coordinator);
    assetManager->loadScene("platform", "assets/models/Light_Square.gltf", *coordinator);

    // Set up the physics world
    spaceManager->createSpace();

    // Set up a light source
    lightPos = glm::vec3(0.0f, 10.0f, 10.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // Create the camera entity
    cameraEntity = coordinator->createEntity();
    coordinator->addComponent(cameraEntity, TransformComponent{.position = {0.0f, 5.0f, 15.0f}});
    coordinator->addComponent(cameraEntity, CameraComponent{
        .projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f),
        .primary = true
    });
    
    // Add player controls to the camera
    PlayerControlledComponent cameraController;
    cameraController.keyMap[GLFW_KEY_W] = PlayerAction::MOVE_FORWARD;
    cameraController.keyMap[GLFW_KEY_S] = PlayerAction::MOVE_BACK;
    cameraController.keyMap[GLFW_KEY_A] = PlayerAction::MOVE_LEFT;
    cameraController.keyMap[GLFW_KEY_D] = PlayerAction::MOVE_RIGHT;
    coordinator->addComponent(cameraEntity, cameraController);

    Entity lightSquare = assetManager->getEntityFromScene("lightSquareScene", "Light_Square");
    if (lightSquare != -1) {
        auto& lightTransform = coordinator->getComponent<TransformComponent>(lightSquare);
        lightTransform.position = {0.0f, 10.0f, 0.0f};
        
        coordinator->addComponent(lightSquare, RigidBodyComponent{.mass = 5.0f});
        coordinator->addComponent(lightSquare, CollisionShapeComponent{.type = ShapeType::BOX, .dimensions = {0.5f, 0.5f, 0.5f}});
    }
        
    Entity groundEntity = assetManager->getEntityFromScene("platformScene", "Platform_2x2_Empty");
    if (groundEntity != -1) {

        auto& groundTransform = coordinator->getComponent<TransformComponent>(groundEntity);
        groundTransform.position = {0.0f, 0.0f, 0.0f};
        groundTransform.scale = {10.0f, 10.0f, 10.0f};

        coordinator->addComponent(groundEntity, RigidBodyComponent{.mass = 0.0f});
        coordinator->addComponent(groundEntity, CollisionShapeComponent{
            .type = ShapeType::BOX, 
            .dimensions = {10.0f, 0.5f, 10.0f}
        });
    }

}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update systems in the correct order
        inputSystem->update(deltaTime);
        playerControlSystem->update(deltaTime);
        physicsSystem->update(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto const& cameraComponent = coordinator->getComponent<CameraComponent>(cameraEntity);
        auto const& cameraTransform = coordinator->getComponent<TransformComponent>(cameraEntity);
        auto shader = assetManager->getShader("pbr");

        renderSystem->draw(*coordinator, *assetManager, shader, cameraComponent, cameraTransform, lightPos, lightColor);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
