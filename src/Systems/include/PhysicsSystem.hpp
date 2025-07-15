#pragma once

#include "System.hpp"
#include "Types.hpp"
#include <memory>
#include <map>

class btRigidBody;
class Coordinator;
class SpaceManager;
class Space;

class PhysicsSystem : public System {

    private:

        SpaceID currentMainSpace = 0;

        void addEntityToPhysics(Entity entity, Space* space);
        void removeEntityFromPhysics(Entity entity, Space* space);

        Coordinator* coordinator;
        SpaceManager* spaceManager;
        
        std::map<Entity, btRigidBody*> entityToRigidBodyMap;

    public:

        SpaceID getCurrentSpace();
        void setCurrentSpace(SpaceID id);
        void init(Coordinator* coordinator, SpaceManager* spaceManager);
        void update(float deltaTime);
        void updateEntitySignature(Entity entity, Signature newSignature);
        void onEntityAdded(Entity entity) override;
        void onEntityRemoved(Entity entity) override;

};