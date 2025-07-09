#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glm/gtx/hash.hpp>

class Shader;
// A struct to define the data that each vertex contains.
// For now, we only care about position, but we add the others for future use.
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    bool operator==(const Vertex& other) const {
        return Position == other.Position && Normal == other.Normal && TexCoords == other.TexCoords;
    }
};

struct SubMesh {
    std::string materialName;
    unsigned int indexCount;
    unsigned int indexOffset; 
};


namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.Position) ^
                   (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.TexCoords) << 1);
        }
    };
}

class Mesh {
public:
    // Mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<SubMesh>      subMeshes; 
    unsigned int VAO;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<SubMesh>& subMeshes);
    void draw(Shader& shader, class AssetManager& assetManager);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};