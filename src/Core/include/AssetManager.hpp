#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Types.hpp" 
#include <map>
#include <string>
#include <memory>
#include <vector>

using Scene = std::map<std::string, Entity>;

class Coordinator;

struct Material {
    std::shared_ptr<Texture> albedoMap;
    glm::vec4 albedoFactor = glm::vec4(1.0f);
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec3 emissiveFactor = glm::vec3(0.0f); 
    bool doubleSided = false;
};

class AssetManager {

    private:
        std::map<std::string, Scene> scenes;
        std::map<std::string, std::shared_ptr<Shader>> shaders;
        std::map<std::string, std::shared_ptr<Mesh>> meshes;
        std::map<std::string, std::shared_ptr<Texture>> textures;
        std::map<std::string, std::shared_ptr<Material>> materials;

        void processMaterials(const tinygltf::Model& model);

    public:

        Scene& loadScene(const std::string& sceneName, const std::string& path, Coordinator& coordinator);
        std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);

        std::shared_ptr<Shader> getShader(const std::string& name);
        std::shared_ptr<Mesh> getMesh(const std::string& name);
        std::shared_ptr<Texture> getTexture(const std::string& name);
        std::shared_ptr<Material> getMaterial(const std::string& name);
        Scene* getScene(const std::string& sceneName);
        Entity getEntityFromScene(const std::string& sceneName, const std::string& entityName);

};