#include <gtest/gtest.h>
#include "SystemManager.hpp"
#include "ComponentManager.hpp"
#include "Types.hpp"
#include "System.hpp"

// Define a dummy system that correctly implements the System interface
class DummySystem : public System {
public:
    void onEntityAdded(Entity entity) override {
        // Empty for test
    }
    void onEntityRemoved(Entity entity) override {
        // Empty for test
    }
};

// Define some dummy components for testing
struct ComponentA {};
struct ComponentB {};

TEST(SystemManagerTest, changeSignature) {
    // ARRANGE
    auto componentManager = std::make_unique<ComponentManager>();
    auto systemManager = std::make_unique<SystemManager>();

    componentManager->registerComponent<ComponentA>();
    componentManager->registerComponent<ComponentB>();

    Signature systemSignature;
    systemSignature.set(componentManager->getComponentTypeID<ComponentA>());
    systemManager->registerSystem<DummySystem>(systemSignature);

    auto dummySystem = systemManager->getSystem<DummySystem>();
    Entity entity = 0;

    // ACT & ASSERT
    Signature entitySignature;
    entitySignature.set(componentManager->getComponentTypeID<ComponentA>());
    systemManager->changeSignature(entity, entitySignature);
    ASSERT_EQ(dummySystem->getEntityCount(entity), 1);

    entitySignature.set(componentManager->getComponentTypeID<ComponentB>());
    systemManager->changeSignature(entity, entitySignature);
    ASSERT_EQ(dummySystem->getEntityCount(entity), 1);

    entitySignature.reset(componentManager->getComponentTypeID<ComponentA>());
    systemManager->changeSignature(entity, entitySignature);
    ASSERT_EQ(dummySystem->getEntityCount(entity), 0);
}
