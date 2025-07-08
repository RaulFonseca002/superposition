#pragma once

#include "Types.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <cassert>

class I_ComponentArray {
public:
    virtual ~I_ComponentArray() = default;
    virtual void removeEntity(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public I_ComponentArray {

    private:
    std::vector<T> componentDataVector;
    std::unordered_map<Entity, size_t> entityToIndexMap;
    std::unordered_map<size_t, Entity> indexToEntityMap;

    public:

        void insertData(Entity entity, T component) {

            assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to same entity more than once.");

            size_t index = componentDataVector.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
            componentDataVector.push_back(component);
        }

        void removeData(Entity entity) {

            assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component.");

            size_t removedIndex = entityToIndexMap[entity];
            size_t swappedIndex = componentDataVector.size() - 1;
            Entity swappedEntity = indexToEntityMap[swappedIndex];
            
            //override the removed index with the last element
            componentDataVector[removedIndex] = componentDataVector[swappedIndex];
            componentDataVector.pop_back();

            //swap the removed and last element in the maps
            entityToIndexMap[swappedEntity] = removedIndex;
            indexToEntityMap[removedIndex]  = swappedEntity;

            //remove entity from the maps
            entityToIndexMap.erase(entity);
            indexToEntityMap.erase(swappedIndex);
        }

        T& getData(Entity entity) {

            if (entityToIndexMap.find(entity) == entityToIndexMap.end()) {
                throw std::runtime_error("Retrieving non-existent component.");
            }            
            
            return componentDataVector[entityToIndexMap[entity]];
        }

        void removeEntity(Entity entity) override {
            if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
                removeData(entity);
            }
        }


};


class ComponentManager {

    private:
        std::unordered_map<const char*, ComponentTypeID> typeNameToIdMap{};
        std::unordered_map<const char*, std::shared_ptr<I_ComponentArray>> componentArrayMap;
        ComponentTypeID nextComponentID = 0;

        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray() {
            const char* typeName = typeid(T).name();
            assert(typeNameToIdMap.find(typeName) != typeNameToIdMap.end() && "Component not registered before use.");
            return std::static_pointer_cast<ComponentArray<T>>(componentArrayMap[typeName]);
        }

    public:

        template<typename T>
        void registerComponent() {

            const char* typeName = typeid(T).name();
            
            assert(typeNameToIdMap.find(typeName) == typeNameToIdMap.end() && "Registering component type more than once.");

            typeNameToIdMap.insert({typeName, nextComponentID});
            componentArrayMap.insert({typeName, std::make_shared<ComponentArray<T>>()});

            nextComponentID++;
        }

        template<typename T>
        ComponentTypeID getComponentTypeID() {
            const char* typeName = typeid(T).name();
            assert(typeNameToIdMap.find(typeName) != typeNameToIdMap.end() && "Component not registered before use.");
            return typeNameToIdMap[typeName];
        }

        template<typename T>
        void addComponent(Entity entity, T component) {
            getComponentArray<T>()->insertData(entity, component);
        }

        template<typename T>
        void removeComponent(Entity entity) {
            getComponentArray<T>()->removeData(entity);
        }

        template<typename T>
        T& getComponent(Entity entity) {
            return getComponentArray<T>()->getData(entity);
        }

        void removeEntity(Entity entity);

};