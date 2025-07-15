#include "Space.hpp"

Space:: ~Space() = default;

Space::Space(btVector3 gravity) {

    collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    broadphase = std::make_unique<btDbvtBroadphase>();
    solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
        dispatcher.get(),
        broadphase.get(),
        solver.get(),
        collisionConfiguration.get()
    );

    dynamicsWorld->setGravity(gravity);
}

btDiscreteDynamicsWorld* Space::getWorld(){
    return dynamicsWorld.get();
}
