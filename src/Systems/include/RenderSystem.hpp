#pragma once

#include "System.hpp"
#include <memory>
#include <glm/glm.hpp>

// Forward-declarations
class Coordinator;
class AssetManager;
class Shader;
struct CameraComponent;
struct TransformComponent;

class RenderSystem : public System {
public:
    void draw(Coordinator& coordinator,
              AssetManager& assetManager,
              std::shared_ptr<Shader> shader,
              const CameraComponent& camera,
              const TransformComponent& cameraTransform,
              const glm::vec3& lightPos,
              const glm::vec3& lightColor);

    void onEntityAdded(Entity entity) override;
    void onEntityRemoved(Entity entity) override;
};

