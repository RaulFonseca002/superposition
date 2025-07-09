#include "RenderSystem.hpp"
#include "Coordinator.hpp"
#include "AssetManager.hpp"
#include "Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void RenderSystem::draw(Coordinator& coordinator,
                        AssetManager& assetManager,
                        std::shared_ptr<Shader> shader,
                        const CameraComponent& camera,
                        const TransformComponent& cameraTransform,
                        const glm::vec3& lightPos,
                        const glm::vec3& lightColor)
{
    shader->use();

    glm::mat4 view = glm::lookAt(cameraTransform.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = camera.projectionMatrix;

    shader->setVec3("light_pos", lightPos);
    shader->setVec3("light_color", lightColor);
    shader->setVec3("view_pos", cameraTransform.position);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    for (auto const& entity : entitySet) {
        auto const& transform = coordinator.getComponent<TransformComponent>(entity);
        auto const& meshInfo = coordinator.getComponent<MeshComponent>(entity);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position) *
                          glm::mat4_cast(transform.rotation) *
                          glm::scale(glm::mat4(1.0f), transform.scale);
        shader->setMat4("model", model);
        
        auto mesh = assetManager.getMesh(meshInfo.meshName);
        mesh->draw(*shader, assetManager);
    }
}
