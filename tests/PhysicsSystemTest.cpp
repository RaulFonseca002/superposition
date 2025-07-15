#include <gtest/gtest.h>
#include "Coordinator.hpp"
#include "SpaceManager.hpp"
#include "PhysicsSystem.hpp"
#include "Types.hpp"

TEST(PhysicsSystemTest, GravityAffectsEntity) {
    // ARRANGE
    auto coordinator = std::make_unique<Coordinator>();
    auto spaceManager = std::make_unique<SpaceManager>();
    
    // Register all necessary components
    coordinator->registerComponent<TransformComponent>();
    coordinator->registerComponent<RigidBodyComponent>();
    coordinator->registerComponent<CollisionShapeComponent>();
    
    // Define the signature for the physics system
    Signature signature;
    signature.set(coordinator->getComponentTypeID<TransformComponent>());
    signature.set(coordinator->getComponentTypeID<RigidBodyComponent>());
    signature.set(coordinator->getComponentTypeID<CollisionShapeComponent>());
    
    // Register the system with the coordinator
    coordinator->registerSystem<PhysicsSystem>(signature);

    // Get the system pointer FROM the coordinator
    auto physicsSystem = coordinator->getSystem<PhysicsSystem>();

    // Initialize the system using the retrieved pointer
    physicsSystem->init(coordinator.get(), spaceManager.get());
    
    // Create a physics space
    spaceManager->createSpace();

    // Create a dynamic entity
    Entity entity = coordinator->createEntity();
    coordinator->addComponent(entity, TransformComponent{
        .position = {0.0f, 100.0f, 0.0f}
    });
    coordinator->addComponent(entity, RigidBodyComponent{
        .mass = 1.0f 
    });
    coordinator->addComponent(entity, CollisionShapeComponent{
        .type = ShapeType::BOX,
        .dimensions = {1.0f, 1.0f, 1.0f}
    });
    
    float initialY = coordinator->getComponent<TransformComponent>(entity).position.y;

    // ACT
    // Simulate a few frames of physics
    float deltaTime = 1.0f / 60.0f;
    for (int i = 0; i < 60; ++i) {
        physicsSystem->update(deltaTime);
    }

    // ASSERT
    float finalY = coordinator->getComponent<TransformComponent>(entity).position.y;
    
    // Verify that the entity has fallen due to gravity
    ASSERT_LT(finalY, initialY); 
}
