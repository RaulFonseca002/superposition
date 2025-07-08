#include "Coordinator.hpp"

Coordinator::Coordinator() {
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::createEntity() {
    return entityManager->createEntity();
}

void Coordinator::removeEntity(Entity entity) {
    entityManager->removeEntity(entity);
    componentManager->removeEntity(entity);
    systemManager->removeEntity(entity);
}

std::uint32_t Coordinator::getActiveEntityCount() {
    return entityManager->getActiveEntityCount();
}