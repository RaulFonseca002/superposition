#include "PhysicsSystem.hpp"
#include <iostream>

void PhysicsSystem::update(float deltaTime) {
    // Placeholder logic. In a real engine, this would call the Bullet physics world.
    // For now, we can just print a message to show it's being called.
    std::cout << "Updating physics with delta time: " << deltaTime << std::endl;
}
