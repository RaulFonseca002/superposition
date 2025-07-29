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

enum class PlayerAction {
    MOVE_FORWARD,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN
};

struct TransformComponent {
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    glm::vec3 right = {1.0f, 0.0f, 0.0f};
};

struct MeshComponent {
    std::string meshName;
};


struct CameraComponent {
    glm::mat4 projectionMatrix;
    bool primary = false;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float sensitivity = 0.1f;
};

struct RigidBodyComponent {

    float mass;
    float friction;
    float restitution; //bounceness
    float forceStrength = 200.0f;

    glm::vec3 force = {0.0f, 0.0f, 0.0f};
    glm::vec3 torque = {0.0f, 0.0f, 0.0f};

    float linearDamping = 0.1f;
    float angularDamping = 0.1f;
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

enum class ShapeType { BOX, SPHERE, CAPSULE };

struct CollisionShapeComponent{

    //for spheres we use x as radius
    //for capsules we use x and radius and y as height
    ShapeType type;
    glm::vec3 dimensions;
};

struct PlayerControlledComponent {
    std::map<int, PlayerAction> keyMap;
    std::map<PlayerAction, bool> actionState;

};