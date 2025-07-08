#include <gtest/gtest.h>
#include "SystemManager.hpp"
#include "ComponentManager.hpp" // Needed to get component IDs
#include "Types.hpp"

// Define dummy components and a system for testing
struct ComponentA {};
struct ComponentB {};

class DummySystem : public System {};


TEST(SystemManagerTest, changeSignature) {
    // ARRANGE
    ComponentManager componentManager;
    SystemManager systemManager;

    // Register components to get their IDs
    componentManager.registerComponent<ComponentA>();
    componentManager.registerComponent<ComponentB>();

    // Define the signature for DummySystem (requires ComponentA)
    Signature systemSignature;
    systemSignature.set(componentManager.getComponentTypeID<ComponentA>());

    // Register the system with its signature
    systemManager.registerSystem<DummySystem>(systemSignature);
    auto dummySystem = systemManager.getSystem<DummySystem>();

    // Create an entity
    Entity entity = 0;

    // ACT & ASSERT: Scenario 1 (Entity gains a matching signature)
    Signature entitySignatureA;
    entitySignatureA.set(componentManager.getComponentTypeID<ComponentA>());
    systemManager.changeSignature(entity, entitySignatureA);

    // Assert that the entity is now in the system's set
    ASSERT_EQ(dummySystem->getEntityCount(entity), 1);

    // ACT & ASSERT: Scenario 2 (Entity loses the matching signature)
    Signature entitySignatureNone; // An empty signature
    systemManager.changeSignature(entity, entitySignatureNone);

    // Assert that the entity is no longer in the system's set
    ASSERT_EQ(dummySystem->getEntityCount(entity), 0);
}

TEST(SystemManagerTest, EntityDestroyed) {
    // ARRANGE
    ComponentManager componentManager;
    SystemManager systemManager;
    componentManager.registerComponent<ComponentA>();
    Signature systemSignature;
    systemSignature.set(componentManager.getComponentTypeID<ComponentA>());
    systemManager.registerSystem<DummySystem>(systemSignature);
    auto dummySystem = systemManager.getSystem<DummySystem>();
    Entity entity = 0;
    Signature entitySignature;
    entitySignature.set(componentManager.getComponentTypeID<ComponentA>());
    systemManager.changeSignature(entity, entitySignature);
    ASSERT_EQ(dummySystem->getEntityCount(entity), 1); // Pre-condition check

    // ACT
    systemManager.removeEntity(entity);

    // ASSERT
    // Assert that the entity was removed from the system's set
    ASSERT_EQ(dummySystem->getEntityCount(entity), 0);
}