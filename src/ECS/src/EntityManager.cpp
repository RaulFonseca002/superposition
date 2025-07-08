#include "EntityManager.hpp"

EntityManager::EntityManager(/* args */) {

    //fill the array from 0 to max entities - 1 
    entityVector.resize(MAX_ENTITIES);
    signatureVector.resize(MAX_ENTITIES);
    std:iota(entityVector.begin(), entityVector.end(), 0);
    std::fill(signatureVector.begin(), signatureVector.end(), 0);
    activeEntityCount = 0;
}

EntityManager::~EntityManager() {

}

Entity EntityManager::createEntity(){

    if(entityVector.empty()){
        throw std::runtime_error("No more entities available.");
    }

    Entity entity = entityVector.back();
    entityVector.pop_back();
    activeEntityCount++;

    return entity;
}

void EntityManager::removeEntity(Entity entity){

    if(entity >= MAX_ENTITIES) return;

    entityVector.push_back(entity);
    activeEntityCount--;
    return;
}

size_t EntityManager::getActiveEntityCount(){
    return activeEntityCount;
}

void EntityManager::assignSignature(Entity entity, Signature signature){
    signatureVector[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity){
    return signatureVector[entity];
}
