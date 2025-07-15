#pragma once

#include <memory>
#include <btBulletDynamicsCommon.h>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class Space {
    private:

        //find pairs of possible colisions
        std::unique_ptr<btBroadphaseInterface> broadphase;

        std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> dispatcher;
        std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    public:
        Space(btVector3 gravity);
        ~Space();
        std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
        btDiscreteDynamicsWorld* getWorld();



};
