#pragma once

#include "Types.hpp"
#include "Space.hpp"
#include <map>
#include <memory>
#include <btBulletDynamicsCommon.h> 

class SpaceManager {
    private:
        std::map<int, std::unique_ptr<Space>> spaces;
        SpaceID nextID = 0;
    public:
        SpaceID createSpace(btVector3 gravity = btVector3(0, -9.81, 0));
        Space* getSpace(SpaceID id);
};
