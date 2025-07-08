#include "ComponentManager.hpp"

void ComponentManager::removeEntity(Entity entity) {

    for(auto const & it : componentArrayMap){
        it.second->removeEntity(entity);
    }
    
}