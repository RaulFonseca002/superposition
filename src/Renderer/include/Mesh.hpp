#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Shader.hpp" // We need the Shader class for the draw method

// A struct to define the data that each vertex contains.
// For now, we only care about position, but we add the others for future use.
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    // Mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

    // Constructor
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Renders the mesh
    void draw(Shader& shader);

private:
    // Render data
    unsigned int VBO, EBO;

    // Initializes all the buffer objects/arrays
    void setupMesh();
};