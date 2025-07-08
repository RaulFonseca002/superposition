#include "Types.hpp"
#include <map>
#include <unordered_set>
#include <memory>
#include "System.hpp"

class SystemManager {

    private:
        std::unordered_map<ComponentTypeName, std::shared_ptr<System>> systemMap;
        std::unordered_map<ComponentTypeName, Signature> signatureMap;

    public:

        template<typename T>
        void registerSystem(Signature signature){

            const char* typeName = typeid(T).name();

            assert(systemMap.find(typeName) == systemMap.end() && "Registering system more than once.");

            auto system = std::make_shared<T>();
            systemMap.insert({typeName, system});            
            signatureMap.insert({typeName, signature});
        }

        template<typename T>
        std::shared_ptr<T> getSystem() {
            const char* typeName = typeid(T).name();
            assert(systemMap.find(typeName) != systemMap.end() && "System not registered before use.");
            return std::static_pointer_cast<T>(systemMap[typeName]);
        }

        void removeEntity(Entity entity);
        void changeSignature(Entity entity, Signature signature);


};
