
#include "RenderSystem.hpp"
#include "Mesh.hpp" // We need the Mesh class here
#include <glm/gtc/matrix_transform.hpp>

// This is a temporary, simple asset manager.
// In a real engine, this would be a dedicated class.
#include <map>
extern std::map<AssetID, std::shared_ptr<Mesh>> g_meshCache;


void RenderSystem::draw(Coordinator& coordinator,
                        std::shared_ptr<Shader> shader,
                        const CameraComponent& camera,
                        const TransformComponent& cameraTransform)
{
    shader->use();

    // Set up camera matrices
    glm::mat4 view = glm::lookAt(cameraTransform.position, cameraTransform.position + glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 projection = camera.projectionMatrix;

    // Loop through entities and draw them
    for (auto const& entity : entitySet) {
        auto const& transform = coordinator.getComponent<TransformComponent>(entity);
        auto const& meshInfo = coordinator.getComponent<MeshComponent>(entity);

        // Calculate model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model = model * glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        // Calculate MVP and send to shader
        glm::mat4 mvp = projection * view * model;
        shader->setMat4("mvp", mvp);

        // Get mesh from cache and draw
        auto mesh = g_meshCache[meshInfo.meshID];
        mesh->draw(*shader);
    }
}
