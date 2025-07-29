#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <glm/gtx/hash.hpp>
#include <tiny_gltf.h> 

class Shader;
class AssetManager;

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

        Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<SubMesh>&& subMeshes);
        void draw(Shader& shader, AssetManager& assetManager);
        
        static std::shared_ptr<Mesh> CreateFromGLTF(const tinygltf::Model& model, const tinygltf::Mesh& gltfMesh);

    private:
        unsigned int VBO, EBO;
        void setupMesh();
        
};