#include <gtest/gtest.h>
#include "Coordinator.hpp"
#include "Types.hpp"

// Define dummy component and system for this test file
struct DummyComponent {
    int value;
};

class DummySystem : public System {};


TEST(CoordinatorTest, Orchestration) {
    // ARRANGE
    Coordinator coordinator; // The constructor creates the managers

    // Register a component type
    coordinator.registerComponent<DummyComponent>();

    // Define a signature for the dummy system
    Signature signature;
    signature.set(coordinator.getComponentTypeID<DummyComponent>());

    // Register the dummy system with its signature
    coordinator.registerSystem<DummySystem>(signature);

    // Get the system object for later checks
    auto dummySystem = coordinator.getSystem<DummySystem>();

    // Create an entity
    Entity entity = coordinator.createEntity();
    
    // Check pre-conditions: the system should not be tracking the entity yet.
    ASSERT_EQ(dummySystem->getEntityCount(entity), 0);

    // ACT
    // Add a component to the entity. This is the main action to test.
    coordinator.addComponent(entity, DummyComponent{100});

    // ASSERT
    // Check if the Coordinator correctly orchestrated all the managers.
    // The SystemManager should have been notified and added the entity
    // to the DummySystem's set.
    ASSERT_EQ(dummySystem->getEntityCount(entity), 1);
}