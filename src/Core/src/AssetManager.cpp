#include "AssetManager.hpp"
#include "Coordinator.hpp"
#include <iostream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

Scene& AssetManager::loadScene(const std::string& sceneName, const std::string& path, Coordinator& coordinator) {

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret;
    if (path.substr(path.find_last_of(".") + 1) == "glb") {
        ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    } else {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }

    if (!warn.empty()) { std::cout << "glTF WARN: " << warn << std::endl; }
    if (!err.empty()) { std::cerr << "glTF ERR: " << err << std::endl; }
    if (!ret) { throw std::runtime_error("Failed to load glTF file."); }

    scenes[sceneName] = Scene();
    Scene& currentScene = scenes[sceneName];

    processMaterials(model);

    for (const auto& mesh : model.meshes)
        meshes[mesh.name] = Mesh::CreateFromGLTF(model, mesh);
    

    int sceneIndex = model.defaultScene > -1 ? model.defaultScene : 0;
    const tinygltf::Scene& scene = model.scenes[sceneIndex];

    for (int nodeIndex : scene.nodes) {
        const tinygltf::Node& node = model.nodes[nodeIndex];
        if (node.mesh < 0) continue;

        Entity entity = coordinator.createEntity();
        currentScene[node.name] = entity;
        
        TransformComponent transform;
        if (!node.translation.empty()) {
            transform.position = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
        }
        if (!node.rotation.empty()) {
            transform.rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        }
        if (!node.scale.empty()) {
            transform.scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
        }
        coordinator.addComponent(entity, transform);

        const auto& mesh = model.meshes[node.mesh];
        coordinator.addComponent(entity, MeshComponent{.meshName = mesh.name});
    }
    
    return currentScene;
}

void AssetManager::processMaterials(const tinygltf::Model& model) {

    for (const auto& texture : model.textures) {
        const auto& image = model.images[texture.source];
        auto tex = std::make_shared<Texture>(image.image.data(), image.width, image.height, image.component);
        textures[image.name] = tex;
    }

    for (const auto& material : model.materials) {
        auto mat = std::make_shared<Material>();
        
        const auto& pbr = material.pbrMetallicRoughness;
        if (pbr.baseColorTexture.index >= 0) {
            const auto& textureInfo = model.textures[pbr.baseColorTexture.index];
            const auto& imageInfo = model.images[textureInfo.source];
            mat->albedoMap = getTexture(imageInfo.name);
        }
        mat->albedoFactor = glm::vec4(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
        mat->metallicFactor = pbr.metallicFactor;
        mat->roughnessFactor = pbr.roughnessFactor;

        if (material.emissiveFactor.size() == 3) {
            mat->emissiveFactor = glm::vec3(material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]);
        }
        
        mat->doubleSided = material.doubleSided;

        materials[material.name] = mat;
    }
}

std::shared_ptr<Shader> AssetManager::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
    auto shader = std::make_shared<Shader>(vertPath, fragPath);
    shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> AssetManager::getShader(const std::string& name) { return shaders.at(name); }

std::shared_ptr<Mesh> AssetManager::getMesh(const std::string& name) { return meshes.at(name); }

std::shared_ptr<Texture> AssetManager::getTexture(const std::string& name) { return textures.at(name); }

std::shared_ptr<Material> AssetManager::getMaterial(const std::string& name) {

    if (materials.find(name) == materials.end()) {

        if (materials.find("default") == materials.end()) {
            
            auto defaultMat = std::make_shared<Material>();
            materials["default"] = defaultMat;
        }

        return materials["default"];
    }

    return materials.at(name);
}

Scene* AssetManager::getScene(const std::string& sceneName) {
    if (scenes.count(sceneName)) { return &scenes[sceneName]; }
    return nullptr;
}

Entity AssetManager::getEntityFromScene(const std::string& sceneName, const std::string& entityName) {
    Scene* scene = getScene(sceneName);
    if (scene && scene->count(entityName)) { return (*scene)[entityName]; }
    return -1;
}
