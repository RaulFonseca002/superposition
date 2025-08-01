#pragma once

#include "System.hpp"
#include "Framebuffer.hpp"
#include <memory>
#include <glm/glm.hpp>


// Forward-declarations
class Coordinator;
class AssetManager;
class Shader;
class Framebuffer;
struct CameraComponent;
struct TransformComponent;

class RenderSystem : public System {

    private:

        Coordinator* coordinator;
        AssetManager* assetManager;
        unsigned int quadVAO;
            
        std::unique_ptr<Framebuffer> framebuffer;
        std::shared_ptr<Shader> pbrShader;
        std::shared_ptr<Shader> postProcessShader;

        void setupScreenQuad();
        float exposure = 1.0f;

    public:

        ~RenderSystem();

        void init(  Coordinator* coordinator, 
                    AssetManager* assetManager, 
                    int screenWidth = 1280, 
                    int screenHeight = 720);

        void draw(const CameraComponent& camera,
                const TransformComponent& cameraTransform,
                const glm::vec3& lightPos,
                const glm::vec3& lightColor);

        void onEntityAdded(Entity entity) override {}
        void onEntityRemoved(Entity entity) override {}

        void setExposure(float exposure) {this->exposure = exposure;}
};