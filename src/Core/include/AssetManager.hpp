#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include <map>
#include <string>
#include <memory>
#include <unordered_map>

struct Material {
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

namespace tinyobj {
    struct material_t;
}

class AssetManager {
public:
    std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
    std::shared_ptr<Shader> getShader(const std::string& name);

    std::shared_ptr<Mesh> loadMesh(const std::string& name, const std::string& path);
    std::shared_ptr<Mesh> getMesh(const std::string& name);
    
    std::shared_ptr<Material> getMaterial(const std::string& name);

private:
    void processMaterials(const std::vector<tinyobj::material_t>& materials);
    
    std::map<std::string, std::shared_ptr<Shader>> shaders;
    std::map<std::string, std::shared_ptr<Mesh>> meshes;
    std::map<std::string, std::shared_ptr<Material>> materials;
};