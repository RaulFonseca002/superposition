#include "SpaceManager.hpp"

SpaceID SpaceManager::createSpace(btVector3 gravity) {

    SpaceID id = nextID;
    nextID++;

    spaces.insert({id, std::make_unique<Space>(gravity)});
    
    return id;
}

Space* SpaceManager::getSpace(SpaceID id){
    return spaces.at(id).get();
}