#pragma once
#include "System.hpp"
#include "Coordinator.hpp"
#include "InputSystem.hpp"
#include "Types.hpp"

class Coordinator;
class InputSystem; 

class PlayerControlSystem : public System {
    private:
        Coordinator* coordinator;
        InputSystem* inputSystem; 

        void processCameraLook(Entity entity);

        void processMovementFlight( TransformComponent& transform, 
                                    PlayerControlledComponent playerControl, 
                                    float deltaTime);

        void processMovementBody(   TransformComponent& transform, 
                                    PlayerControlledComponent const& playerControl,
                                    RigidBodyComponent& rigidBody);

    public:
        void init(Coordinator* coordinator, InputSystem* inputSystem);
        void update(float deltaTime);
        void onEntityAdded(Entity entity) override {}
        void onEntityRemoved(Entity entity) override {}
        void addKeyBinding(Entity entity, std::pair<int, PlayerAction> keyActionPair);

};
