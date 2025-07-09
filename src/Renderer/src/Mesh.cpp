#include "Mesh.hpp"
#include "AssetManager.hpp"
#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<SubMesh>& subMeshes) {
    this->vertices = vertices;
    this->indices = indices;
    this->subMeshes = subMeshes;
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glBindVertexArray(0);
}

// The draw function now loops through submeshes
void Mesh::draw(Shader& shader, AssetManager& assetManager) {
    glBindVertexArray(VAO);
    for (const auto& subMesh : subMeshes) {
        // Get the material for this sub-mesh
        auto material = assetManager.getMaterial(subMesh.materialName);

        // Send material properties to the shader
        shader.setVec3("material_diffuse", material->diffuse);
        shader.setVec3("material_specular", material->specular);
        shader.setFloat("material_shininess", material->shininess);

        // Issue the draw call for this part of the mesh
        glDrawElements(GL_TRIANGLES, 
                       subMesh.indexCount, 
                       GL_UNSIGNED_INT, 
                       (void*)(subMesh.indexOffset * sizeof(unsigned int)));
    }
    glBindVertexArray(0);
}
