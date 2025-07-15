#pragma once

#include "Types.hpp"
#include <unordered_set>

class System {
    public:
        std::unordered_set<Entity> entitySet;
        virtual ~System() = default;
        virtual void onEntityAdded(Entity entity) = 0;
        virtual void onEntityRemoved(Entity entity) = 0;
        
        void addEntity(Entity entity){
            entitySet.insert(entity);
        }

        void removeEntity(Entity entity){
            entitySet.erase(entity);
        }

        size_t getEntityCount(Entity entity){
            return entitySet.count(entity);
        }
};

