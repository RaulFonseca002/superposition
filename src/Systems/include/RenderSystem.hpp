#pragma once
#include "System.hpp"
#include "Coordinator.hpp"
#include "Shader.hpp"
#include <memory>

class RenderSystem : public System {
public:
    void draw(Coordinator& coordinator,
              std::shared_ptr<Shader> shader,
              const CameraComponent& camera,
              const TransformComponent& cameraTransform);
};