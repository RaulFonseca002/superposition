#include "Mesh.hpp"
#include <glad/glad.h>

// Constructor implementation
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    this->vertices = vertices;
    this->indices = indices;

    // Now that we have all the required data, set up the vertex buffers and attribute pointers.
    setupMesh();
}

// Private setup method implementation
void Mesh::setupMesh() {
    // 1. Generate the buffer IDs
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 2. Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // 3. Copy our vertices array into a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 4. Copy our index array into an element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 5. Set the vertex attribute pointers
    // This tells OpenGL how to interpret the vertex data in the VBO.

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Unbind the VAO to prevent accidental modification
    glBindVertexArray(0);
}

// Draw method implementation
void Mesh::draw(Shader& shader) {
    // For now, our draw call is simple. It will get more complex when we add textures.
    
    // Bind the VAO for this mesh
    glBindVertexArray(VAO);
    
    // Issue the draw call
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    
    // Unbind the VAO again afterwards
    glBindVertexArray(0);
}
