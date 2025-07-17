# --- 1. Basic Project Setup ---
cmake_minimum_required(VERSION 3.16)
project(Superposition VERSION 1.0)

# --- 2. Set C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 3. Add Subdirectories for Dependencies ---
add_subdirectory(lib/glfw)
add_subdirectory(lib/bullet)

# Find the standard Threads package
find_package(Threads REQUIRED)

# --- 4. Collect Source Files ---
# Explicitly list all source files for the executable. This is more robust.
set(SOURCES
    src/main.cpp
    src/Core/src/Application.cpp
    src/Core/src/AssetManager.cpp
    src/Core/src/Space.cpp
    src/Core/src/SpaceManager.cpp
    src/ECS/src/Coordinator.cpp
    src/ECS/src/EntityManager.cpp
    src/ECS/src/ComponentManager.cpp
    src/ECS/src/SystemManager.cpp
    src/Renderer/src/Shader.cpp
    src/Renderer/src/Mesh.cpp
    src/Systems/src/RenderSystem.cpp
    src/Systems/src/PhysicsSystem.cpp
    src/Systems/src/InputSystem.cpp
    lib/glad/src/glad.c
)

# --- 5. Create the Executable ---
add_executable(Superposition ${SOURCES})

# --- 6. Configure the Superposition Target ---
# Add Include Directories
target_include_directories(Superposition PRIVATE
    "${CMAKE_SOURCE_DIR}/src/Core/include"
    "${CMAKE_SOURCE_DIR}/src/ECS/include"
    "${CMAKE_SOURCE_DIR}/src/Renderer/include"
    "${CMAKE_SOURCE_DIR}/src/Systems/include"
    "${CMAKE_SOURCE_DIR}/lib/glad/include"
    "${CMAKE_SOURCE_DIR}/lib/glm"
    "${CMAKE_SOURCE_DIR}/lib/tinyobjloader"
    "${CMAKE_SOURCE_DIR}/lib/bullet/src" 
    "${CMAKE_SOURCE_DIR}/lib/glfw/include"
)

# Add Compile Definitions
target_compile_definitions(Superposition PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Link Libraries
target_link_libraries(Superposition PRIVATE
    glfw
    Threads::Threads
    BulletDynamics
    BulletCollision
    LinearMath
)

# --- 7. Unit Testing Setup ---
enable_testing()
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)

# Create a separate executable for our tests
add_executable(UnitTests
    # Test files
    tests/EntityManagerTest.cpp
    tests/ComponentManagerTest.cpp
    tests/SystemManagerTest.cpp
    tests/CoordinatorTest.cpp
    tests/PhysicsSystemTest.cpp

    # Source files needed by the tests
    src/ECS/src/EntityManager.cpp
    src/ECS/src/ComponentManager.cpp
    src/ECS/src/SystemManager.cpp
    src/ECS/src/Coordinator.cpp
    src/Core/src/Space.cpp
    src/Core/src/SpaceManager.cpp
    src/Systems/src/PhysicsSystem.cpp
    src/Systems/src/InputSystem.cpp
)

# The test executable needs access to the same include directories and definitions
target_include_directories(UnitTests PRIVATE
    "${CMAKE_SOURCE_DIR}/src/Core/include"
    "${CMAKE_SOURCE_DIR}/src/ECS/include"
    "${CMAKE_SOURCE_DIR}/src/Renderer/include"
    "${CMAKE_SOURCE_DIR}/src/Systems/include"
    "${CMAKE_SOURCE_DIR}/lib/glad/include"
    "${CMAKE_SOURCE_DIR}/lib/glm"
    "${CMAKE_SOURCE_DIR}/lib/bullet/src"
    "${CMAKE_SOURCE_DIR}/lib/glfw/include"
)
target_compile_definitions(UnitTests PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Link the test executable against GoogleTest and Bullet
target_link_libraries(UnitTests PRIVATE 
    gtest_main
    glfw
    Threads::Threads
    BulletDynamics
    BulletCollision
    LinearMath
)

# Automatically discover all tests within the test executable
include(GoogleTest)
gtest_discover_tests(UnitTests)
