#include "PhysicsSystem.hpp"
#include "SpaceManager.hpp"
#include "Coordinator.hpp"
#include <btBulletDynamicsCommon.h>
#include <iostream>

void PhysicsSystem::init(Coordinator* coordinator, SpaceManager* spaceManager) {
    this->coordinator = coordinator;
    this->spaceManager = spaceManager;
}

void PhysicsSystem::update(float deltaTime) {

    Space* mainSpace = spaceManager->getSpace(currentMainSpace);
    if (!mainSpace) return;

    mainSpace->dynamicsWorld->stepSimulation(deltaTime, 10);

    for (auto const& pair : entityToRigidBodyMap) {
        Entity entity = pair.first;
        btRigidBody* body = pair.second;
        
        auto& transform = coordinator->getComponent<TransformComponent>(entity);

        btTransform btTransform;
        if (body && body->getMotionState()) {
            //copy body to btTransform variable
            body->getMotionState()->getWorldTransform(btTransform);
        } else {
            continue;
        }

        btVector3 pos = btTransform.getOrigin();
        transform.position = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
        
        btQuaternion rot = btTransform.getRotation();
        transform.rotation = glm::quat(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
    }
}

void PhysicsSystem::addEntityToPhysics(Entity entity, Space* space) {

    if (entityToRigidBodyMap.find(entity) != entityToRigidBodyMap.end()) {
        // Entity already has a rigid body, do nothing.
        return;
    }

    auto const& transform = coordinator->getComponent<TransformComponent>(entity);
    auto const& rigidBody = coordinator->getComponent<RigidBodyComponent>(entity);
    auto const& shapeInfo = coordinator->getComponent<CollisionShapeComponent>(entity);

    btCollisionShape* colShape = nullptr;
        
    switch(shapeInfo.type) {
        case ShapeType::BOX:
            colShape = new btBoxShape(btVector3(shapeInfo.dimensions.x, shapeInfo.dimensions.y, shapeInfo.dimensions.z));
            break;
        case ShapeType::SPHERE:
            colShape = new btSphereShape(shapeInfo.dimensions.x); // Use x as radius
            break;
        case ShapeType::CAPSULE:
            colShape = new btCapsuleShape(shapeInfo.dimensions.x, shapeInfo.dimensions.y); // Use x as radius, y as height
            break;
    }

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(
                                transform.position.x, 
                                transform.position.y, 
                                transform.position.z));

    startTransform.setRotation(btQuaternion
                                (transform.rotation.x, 
                                transform.rotation.y, 
                                transform.rotation.z, 
                                transform.rotation.w));


    btScalar mass(rigidBody.mass);
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic) {
        colShape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    space->dynamicsWorld->addRigidBody(body);
    entityToRigidBodyMap[entity] = body;

}

void PhysicsSystem::removeEntityFromPhysics(Entity entity, Space* space) {

    if (entityToRigidBodyMap.find(entity) == entityToRigidBodyMap.end()) {
        // Entity not in physics world, do nothing
        return;
    }

    btRigidBody* body = entityToRigidBodyMap[entity];

    if (body && body->getMotionState()) {
        delete body->getMotionState();
    }

    if (body) {
        space->dynamicsWorld->removeRigidBody(body);
        delete body->getCollisionShape();
        delete body;
    }

    entityToRigidBodyMap.erase(entity);
}

void PhysicsSystem::setCurrentSpace(SpaceID id){
    currentMainSpace = id;
}
        
SpaceID PhysicsSystem::getCurrentSpace(){
    return currentMainSpace;
}

void PhysicsSystem::onEntityAdded(Entity entity) {
    Space* mainSpace = spaceManager->getSpace(currentMainSpace);
    if (!mainSpace) return;
    addEntityToPhysics(entity, mainSpace);
}

void PhysicsSystem::onEntityRemoved(Entity entity) {
    Space* mainSpace = spaceManager->getSpace(currentMainSpace);
    if (!mainSpace) return;
    removeEntityFromPhysics(entity, mainSpace);
}
