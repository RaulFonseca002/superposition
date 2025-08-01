#include "Mesh.hpp"
#include "AssetManager.hpp"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp> 

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<SubMesh>&& subMeshes)  {
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);
    this->subMeshes = std::move(subMeshes);
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

void Mesh::draw(Shader& shader, AssetManager& assetManager) {

    glBindVertexArray(VAO);

    for (const auto& subMesh : subMeshes) {
        auto material = assetManager.getMaterial(subMesh.materialName);

        if (material->albedoMap) {
            glActiveTexture(GL_TEXTURE0); 
            material->albedoMap->bind();
        }

        shader.setVec4("albedoFactor", material->albedoFactor);
        shader.setFloat("metallicFactor", material->metallicFactor);
        shader.setFloat("roughnessFactor", material->roughnessFactor);
        shader.setVec3("emissiveFactor", material->emissiveFactor);
        shader.setBool("doubleSided", material->doubleSided);
        glDrawElements(GL_TRIANGLES, 
                       subMesh.indexCount, 
                       GL_UNSIGNED_INT, 
                       (void*)(subMesh.indexOffset * sizeof(unsigned int)));
    }
    glBindVertexArray(0);
}

std::shared_ptr<Mesh> Mesh::CreateFromGLTF(const tinygltf::Model& model, const tinygltf::Mesh& gltfMesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<SubMesh> subMeshes;

    for (const auto& primitive : gltfMesh.primitives) {
        SubMesh subMesh;
        // This is the key fix: we track the starting index for this primitive's vertices
        unsigned int baseVertex = static_cast<unsigned int>(vertices.size());
        unsigned int indexOffset = static_cast<unsigned int>(indices.size());
        
        // --- Process Vertices ---
        const float* positions = nullptr;
        const float* normals = nullptr;
        const float* texcoords = nullptr;
        size_t vertexCount = 0;

        if (primitive.attributes.count("POSITION")) {
            const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
            const auto& view = model.bufferViews[accessor.bufferView];
            positions = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            vertexCount = accessor.count;
        }
        if (primitive.attributes.count("NORMAL")) {
            const auto& accessor = model.accessors[primitive.attributes.at("NORMAL")];
            const auto& view = model.bufferViews[accessor.bufferView];
            normals = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        }
        if (primitive.attributes.count("TEXCOORD_0")) {
            const auto& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const auto& view = model.bufferViews[accessor.bufferView];
            texcoords = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
        }

        for (size_t i = 0; i < vertexCount; i++) {
            Vertex vertex{};
            vertex.Position = glm::make_vec3(&positions[i * 3]);
            if (normals) vertex.Normal = glm::make_vec3(&normals[i * 3]);
            if (texcoords) vertex.TexCoords = glm::make_vec2(&texcoords[i * 2]);
            vertices.push_back(vertex);
        }
        
        // --- Process Indices ---
        const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
        const void* indexData = &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset];
        
        for (size_t i = 0; i < indexAccessor.count; i++) {
            // Add the baseVertex offset to correctly map the index to the combined vertex buffer
            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                indices.push_back(baseVertex + static_cast<const unsigned short*>(indexData)[i]);
            } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                indices.push_back(baseVertex + static_cast<const unsigned int*>(indexData)[i]);
            }
        }

        if (primitive.material >= 0) {
            subMesh.materialName = model.materials[primitive.material].name;
        } else {
            subMesh.materialName = "default";
        }
        
        subMesh.indexOffset = indexOffset;
        subMesh.indexCount = indexAccessor.count;
        subMeshes.push_back(subMesh);
    }
    
    return std::make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(subMeshes));
}