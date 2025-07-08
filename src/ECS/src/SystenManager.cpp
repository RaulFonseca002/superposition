#include "SystemManager.hpp"

void SystemManager::removeEntity(Entity entity){
    for(auto const & it : systemMap){
        it.second->removeEntity(entity);
    }
}

void SystemManager::changeSignature(Entity entity, Signature signature){
        
    for(auto const & it : signatureMap){
        if((it.second & signature) == it.second){
            systemMap[it.first]->addEntity(entity);
        }else{
            systemMap[it.first]->removeEntity(entity);
        }
    }
}