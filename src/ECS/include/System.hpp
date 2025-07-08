#pragma once

#include "Types.hpp"
#include <unordered_set>

class System {
    protected:
        std::unordered_set<Entity> entitySet;
    public:
        virtual ~System() = default;
        
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

