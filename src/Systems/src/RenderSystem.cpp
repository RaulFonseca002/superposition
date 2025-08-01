#include "RenderSystem.hpp"
#include "Coordinator.hpp"
#include "AssetManager.hpp"
#include "Mesh.hpp"
#include "Framebuffer.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct QuadVertex {
    glm::vec2 Position;
    glm::vec2 TexCoords;
};

RenderSystem::~RenderSystem(){
    framebuffer.reset();
}

void RenderSystem::init(Coordinator* coordinator, AssetManager* assetManager, int screenWidth, int screenHeight) {

    this->coordinator = coordinator;
    this->assetManager = assetManager;

    framebuffer = std::make_unique<Framebuffer>(screenWidth, screenHeight);

    pbrShader = assetManager->getShader("pbr");
    postProcessShader = assetManager->getShader("post_process");
    
    setupScreenQuad();
}

void RenderSystem::setupScreenQuad() {

    //set 6 verticies to cover the entire screen 
    QuadVertex quadVertices[] = {
        {{-1.0f,  1.0f}, {0.0f, 1.0f}},
        {{-1.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 1.0f, -1.0f}, {1.0f, 0.0f}},

        {{-1.0f,  1.0f}, {0.0f, 1.0f}},
        {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f}, {1.0f, 1.0f}}
    };

    unsigned int quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    
    // Use the QuadVertex struct and offsetof for a robust and readable setup
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, Position));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, TexCoords));
}

void RenderSystem::draw(const CameraComponent& camera,
                      const TransformComponent& cameraTransform,
                      const glm::vec3& lightPos,
                      const glm::vec3& lightColor)
{
    // --- 1. GEOMETRY PASS: Render the scene to the framebuffer ---
    framebuffer->bind();
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    
    // Clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pbrShader->use();

    // Set up camera and lighting uniforms (these are the same for all objects)
    glm::mat4 view = glm::lookAt(cameraTransform.position, cameraTransform.position + cameraTransform.front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = camera.projectionMatrix;
    pbrShader->setMat4("view", view);
    pbrShader->setMat4("projection", projection);
    pbrShader->setVec3("viewPos", cameraTransform.position);
    pbrShader->setVec3("lightPos", lightPos);
    pbrShader->setVec3("lightColor", lightColor);

    // Loop through all renderable entities and draw them
    for (auto const& entity : entitySet) {
        auto const& transform = coordinator->getComponent<TransformComponent>(entity);
        auto const& meshInfo = coordinator->getComponent<MeshComponent>(entity);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position) *
                          glm::mat4_cast(transform.rotation) *
                          glm::scale(glm::mat4(1.0f), transform.scale);
        pbrShader->setMat4("model", model);
        
        auto mesh = assetManager->getMesh(meshInfo.meshName);
        mesh->draw(*pbrShader, *assetManager);
    }
    
    // --- 2. POST-PROCESSING PASS: Render the framebuffer texture to the screen ---
    framebuffer->unbind(); // Bind back to the default framebuffer
    glDisable(GL_DEPTH_TEST); // No need for depth testing on a 2D quad

    // Clear the screen
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white for debugging if needed
    glClear(GL_COLOR_BUFFER_BIT);

    postProcessShader->use();
    postProcessShader->setFloat("exposure", exposure);

    // Bind the texture from our framebuffer
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer->textureColorBuffer);

    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}