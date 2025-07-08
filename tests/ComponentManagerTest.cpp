#include <gtest/gtest.h>
#include "ComponentManager.hpp"
#include "Types.hpp"

// Define some simple components for testing purposes.
struct Position {
    float x, y;
};

struct Velocity {
    float vx, vy;
};


TEST(ComponentManagerTest, RegisterAndGetComponentType) {
    ComponentManager componentManager;

    // Register the components
    componentManager.registerComponent<Position>();
    componentManager.registerComponent<Velocity>();

    // Assert that they get unique, sequential type IDs
    ASSERT_EQ(componentManager.getComponentTypeID<Position>(), 0);
    ASSERT_EQ(componentManager.getComponentTypeID<Velocity>(), 1);
}

TEST(ComponentManagerTest, AddAndGetComponent) {
    ComponentManager componentManager;
    componentManager.registerComponent<Position>();

    Entity entity = 0;
    Position position = {10.0f, 20.0f};

    // Add a component and ensure no errors are thrown
    ASSERT_NO_THROW(componentManager.addComponent(entity, position));

    // Retrieve the component and check its values
    Position& retrievedPosition = componentManager.getComponent<Position>(entity);
    ASSERT_EQ(retrievedPosition.x, 10.0f);
    ASSERT_EQ(retrievedPosition.y, 20.0f);
}

TEST(ComponentManagerTest, RemoveComponent) {
    ComponentManager componentManager;
    componentManager.registerComponent<Position>();

    Entity entity = 0;
    Position position = {10.0f, 20.0f};

    componentManager.addComponent(entity, position);
    ASSERT_NO_THROW(componentManager.removeComponent<Position>(entity));

    // Assert that getting the component now throws an exception
    ASSERT_THROW(componentManager.getComponent<Position>(entity), std::runtime_error);
}

TEST(ComponentManagerTest, EntityDestroyed) {
    ComponentManager componentManager;
    componentManager.registerComponent<Position>();
    componentManager.registerComponent<Velocity>();

    Entity entity = 5;
    componentManager.addComponent<Position>(entity, {1.0f, 2.0f});
    componentManager.addComponent<Velocity>(entity, {3.0f, 4.0f});

    // Notify the manager that the entity has been destroyed
    componentManager.removeEntity(entity);

    // Assert that getting either component now throws an exception
    ASSERT_THROW(componentManager.getComponent<Position>(entity), std::runtime_error);
    ASSERT_THROW(componentManager.getComponent<Velocity>(entity), std::runtime_error);
}