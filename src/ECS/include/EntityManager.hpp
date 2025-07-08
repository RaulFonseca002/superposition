#pragma once
#include <vector>
#include <numeric>
#include <iostream>
#include "Types.hpp"

class EntityManager {

private:
    std::vector<Entity> entityVector;
    std::vector<Signature> signatureVector;
    int activeEntityCount;
public:

    EntityManager(/* args */);
    ~EntityManager();
    Entity createEntity();
    void removeEntity(Entity entity);
    size_t getActiveEntityCount();
    void assignSignature(Entity entity, Signature signature);
    Signature getSignature(Entity entity);
};
