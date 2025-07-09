#include "AssetManager.hpp"
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// --- Shader Methods ---
std::shared_ptr<Shader> AssetManager::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
    auto shader = std::make_shared<Shader>(vertPath, fragPath);
    shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> AssetManager::getShader(const std::string& name) {
    return shaders.at(name);
}

// --- Mesh Methods ---
std::shared_ptr<Mesh> AssetManager::loadMesh(const std::string& name, const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> loaded_materials;
    std::string warn, err;

    std::string::size_type pos = path.find_last_of("/");
    std::string mtl_basepath = (pos == std::string::npos) ? "" : path.substr(0, pos + 1);

    if (!tinyobj::LoadObj(&attrib, &shapes, &loaded_materials, &warn, &err, path.c_str(), mtl_basepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    processMaterials(loaded_materials);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<SubMesh> subMeshes;
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        SubMesh subMesh;
        unsigned int indexOffset = static_cast<unsigned int>(indices.size());
        unsigned int indexCount = 0;
        
        // Find the material for this shape
        if (!shape.mesh.material_ids.empty() && shape.mesh.material_ids[0] >= 0) {
            subMesh.materialName = loaded_materials[shape.mesh.material_ids[0]].name;
        } else {
            subMesh.materialName = "default"; // Fallback material
        }

        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};
            vertex.Position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            if (index.normal_index >= 0) {
                vertex.Normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }
            if (index.texcoord_index >= 0) {
                vertex.TexCoords = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
            indexCount++;
        }
        
        subMesh.indexOffset = indexOffset;
        subMesh.indexCount = indexCount;
        subMeshes.push_back(subMesh);
    }

    auto mesh = std::make_shared<Mesh>(vertices, indices, subMeshes);
    meshes[name] = mesh;
    return mesh;
}

std::shared_ptr<Mesh> AssetManager::getMesh(const std::string& name) {
    return meshes.at(name);
}

// --- Material Methods ---
void AssetManager::processMaterials(const std::vector<tinyobj::material_t>& loaded_materials) {
    for (const auto& material : loaded_materials) {
        auto mat = std::make_shared<Material>();
        mat->diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        mat->specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
        mat->shininess = material.shininess;
        
        this->materials[material.name] = mat;
    }
}

std::shared_ptr<Material> AssetManager::getMaterial(const std::string& name) {
    if (materials.find(name) == materials.end()) {
        if (materials.find("default") == materials.end()) {
            auto defaultMat = std::make_shared<Material>();
            defaultMat->diffuse = glm::vec3(0.7f);
            defaultMat->specular = glm::vec3(0.5f);
            defaultMat->shininess = 32.0f;
            materials["default"] = defaultMat;
        }
        return materials["default"];
    }
    return materials.at(name);
}
