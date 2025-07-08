#pragma once
#include <unordered_set>
#include <bitset>
#include <any>
#include <map>
#include <string>
#include <cstdint> 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

const std::uint8_t MAX_COMPONENTS = 32;
const uint32_t MAX_ENTITIES = 5000;

using Entity = std::uint32_t;
using AssetID = std::uint32_t;
using SpaceID = std::uint32_t;
using ComponentTypeID = std::uint8_t;
using ComponentTypeName = const char*;
using Signature = std::bitset<MAX_COMPONENTS>;
using ComponentOverrides = std::map<ComponentTypeID, std::any>;


struct TransformComponent{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct MeshComponent{
    AssetID meshID;
    AssetID materialID;
};

struct CameraComponent{
    glm::mat4 projectionMatrix;
    bool primary;
};

struct RigidBodyComponent {
    float mass;
    float friction;
    float restitution;
};

struct InterspaceLinkProperties {

    // If true, forces applied to one instance are mirrored on all others.
    bool shareAppliedForces = true;

    // If true, velocity changes from collisions on one instance are mirrored on all others.
    bool shareCollisionImpulses = true;

};


struct SpatialStateComponent {
    InterspaceLinkProperties linkProperties;
    std::map<SpaceID, ComponentOverrides> instances;
};

struct SpatialManifoldComponent {
    enum class Type {STATIC_PORTAL, VIEW_DEPENDENT};
    Type type;
    Entity targetManifold;
    SpaceID targetSpace;
};
