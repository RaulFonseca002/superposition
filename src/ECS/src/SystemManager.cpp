#include "SystemManager.hpp"

void SystemManager::removeEntity(Entity entity){
    for(auto const & it : systemMap){
        it.second->removeEntity(entity);
    }
}

void SystemManager::changeSignature(Entity entity, Signature signature){
        
    for (auto const& pair : systemMap) {
        auto const& typeName = pair.first;
        auto const& system = pair.second;
        auto const& systemSignature = signatureMap[typeName];

        if ((signature & systemSignature) == systemSignature) {

            if (system->entitySet.find(entity) == system->entitySet.end()) {
                system->entitySet.insert(entity);
                system->onEntityAdded(entity);
            }
        } else {

            if (system->entitySet.find(entity) != system->entitySet.end()) {
                system->entitySet.erase(entity);
                system->onEntityRemoved(entity);
            }
        }
    }
}