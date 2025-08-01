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

    // --- 1. Apply forces from components to the simulation ---
    for (auto const& entity : entitySet) {
        auto& rigidBody = coordinator->getComponent<RigidBodyComponent>(entity);
        btRigidBody* body = entityToRigidBodyMap[entity];

        // If a force has been set in the component, apply it
        if (body && body->isActive() && rigidBody.force != glm::vec3(0.0f)) {
            body->applyCentralForce(btVector3(rigidBody.force.x, rigidBody.force.y, rigidBody.force.z));
        }
        
        // Clear the force for the next frame
        rigidBody.force = glm::vec3(0.0f);
    }

    // --- 2. Step the simulation ---
    mainSpace->dynamicsWorld->stepSimulation(deltaTime, 10);

    // --- 3. Sync simulation results back to components ---
    for (auto const& pair : entityToRigidBodyMap) {
        Entity entity = pair.first;
        btRigidBody* body = pair.second;
        
        auto const& rigidBody = coordinator->getComponent<RigidBodyComponent>(entity);
        btVector3 velocity = body->getLinearVelocity();
        btScalar speed = velocity.length();
        if (speed > rigidBody.maxSpeed) {
            velocity *= rigidBody.maxSpeed / speed;
            body->setLinearVelocity(velocity);
        }
        
        auto& transform = coordinator->getComponent<TransformComponent>(entity);

        btTransform btTransform;
        if (body && body->getMotionState()) {
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

    if (entityToRigidBodyMap.find(entity) != entityToRigidBodyMap.end()) return;

    auto const& transform = coordinator->getComponent<TransformComponent>(entity);
    auto const& rigidBody = coordinator->getComponent<RigidBodyComponent>(entity);
    auto const& shapeInfo = coordinator->getComponent<CollisionShapeComponent>(entity);

    btCollisionShape* colShape = nullptr;
        
    switch(shapeInfo.type) {
        case ShapeType::BOX:
            colShape = new btBoxShape(btVector3(shapeInfo.dimensions.x, shapeInfo.dimensions.y, shapeInfo.dimensions.z));
            break;
        case ShapeType::SPHERE:
            colShape = new btSphereShape(shapeInfo.dimensions.x);
            break;
        case ShapeType::CAPSULE:
            colShape = new btCapsuleShape(shapeInfo.dimensions.x, shapeInfo.dimensions.y);
            break;
    }

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3( transform.position.x, 
                                        transform.position.y, 
                                        transform.position.z));

    startTransform.setRotation(btQuaternion(transform.rotation.x,
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
    
    body->setDamping(rigidBody.linearDamping, rigidBody.angularDamping);
    rbInfo.m_friction = rigidBody.friction;

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
