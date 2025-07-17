#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

class Coordinator {

    private:
        std::unique_ptr<EntityManager> entityManager;
        std::unique_ptr<ComponentManager> componentManager;
        std::unique_ptr<SystemManager> systemManager;
    public:

        Coordinator(/* args */);

        template<typename T>
        void addComponent(Entity entity, T component){

            size_t id;
            Signature signature;

            componentManager->addComponent<T>(entity, component);

            id = componentManager->getComponentTypeID<T>();
            signature = entityManager->getSignature(entity);
            signature.set(id, true);
            entityManager->assignSignature(entity, signature);
            systemManager->changeSignature(entity, signature);
        }

        template<typename T>
        void removeComponent(Entity entity){

            size_t id;
            Signature signature;

            componentManager->removeComponent<T>(entity);

            id = componentManager->getComponentTypeID<T>();
            signature = entityManager->getSignature(entity);
            signature.set(id, false);
            entityManager->assignSignature(entity, signature);
            systemManager->changeSignature(entity, signature);
        }

        template<typename T>
        void registerComponent(){
            componentManager->registerComponent<T>();
        }


        template<typename T>
        T& getComponent(Entity entity){
            return componentManager->getComponent<T>(entity);
        }

        template<typename T>
        void registerSystem(Signature signature){
            systemManager->registerSystem<T>(signature);
        }

        template<typename T>
        std::shared_ptr<T> getSystem() {
            return systemManager->getSystem<T>();
        }

        template<typename T>
        ComponentTypeID getComponentTypeID() {
            return componentManager->getComponentTypeID<T>();
        }

        Entity createEntity();
        void removeEntity(Entity entity);
        std::uint32_t getActiveEntityCount();

        template<typename T>
        bool hasComponent(Entity entity) {
            return entityManager->getSignature(entity).test(componentManager->getComponentTypeID<T>());
        }

};

