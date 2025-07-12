# --- 1. Basic Project Setup ---
cmake_minimum_required(VERSION 3.16)
project(Superposition VERSION 1.0)

# --- 2. Set C++ Standard ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 3. Collect Source Files ---
file(GLOB_RECURSE MODULE_SOURCES "src/*/*.cpp")
set(SOURCES
    src/main.cpp
    ${MODULE_SOURCES}
    lib/glad/src/glad.c
)

# --- 4. Create the Executable ---
add_executable(Superposition ${SOURCES})

# Add Bullet Physics
# We add the main library and disable building demos and tests for faster compiles
option(BUILD_BULLET2_DEMOS "Build Bullet2 demos" OFF)
option(BUILD_CPU_DEMOS "Build cpu demos" OFF)
option(BUILD_OPENGL3_DEMOS "Build opengl3 demos" OFF)
option(BUILD_UNIT_TESTS "Build Bullet unit tests" OFF)
add_subdirectory(lib/bullet)

# --- 5. Add Include Directories ---
target_include_directories(Superposition PRIVATE
    # Engine Modules
    "${CMAKE_SOURCE_DIR}/src/Core/include"
    "${CMAKE_SOURCE_DIR}/src/ECS/include"
    "${CMAKE_SOURCE_DIR}/src/Renderer/include"
    "${CMAKE_SOURCE_DIR}/src/Physics/include"
    "${CMAKE_SOURCE_DIR}/src/Scene/include"
    "${CMAKE_SOURCE_DIR}/src/Components"
    "${CMAKE_SOURCE_DIR}/src/Systems/include"
    # Libraries
    "${CMAKE_SOURCE_DIR}/lib/glad/include"
    "${CMAKE_SOURCE_DIR}/lib/glm"
    "${CMAKE_SOURCE_DIR}/lib/tinyobjloader"
    "${CMAKE_SOURCE_DIR}/lib/bullet"

)

target_compile_definitions(Superposition PRIVATE GLM_ENABLE_EXPERIMENTAL)

# --- 6. Add and Link Dependencies ---
add_subdirectory(lib/glfw)
find_package(Threads REQUIRED)
target_link_libraries(Superposition PRIVATE
    glfw
    Threads::Threads
)

# --- 7. Unit Testing Setup ---
enable_testing()
add_subdirectory(lib/googletest EXCLUDE_FROM_ALL)

# Create a separate executable for our tests
add_executable(UnitTests
    # List all test files and the .cpp files they are testing
    tests/EntityManagerTest.cpp
    tests/ComponentManagerTest.cpp
    tests/SystemManagerTest.cpp
    tests/CoordinatorTest.cpp
    src/ECS/src/EntityManager.cpp
    src/ECS/src/ComponentManager.cpp
    src/ECS/src/SystenManager.cpp
    src/ECS/src/Coordinator.cpp
)

# The test executable needs access to the same include directories
target_include_directories(UnitTests PRIVATE
    # Engine Modules
    "${CMAKE_SOURCE_DIR}/src/Core/include"
    "${CMAKE_SOURCE_DIR}/src/ECS/include"
    "${CMAKE_SOURCE_DIR}/src/Renderer/include"
    "${CMAKE_SOURCE_DIR}/src/Physics/include"
    "${CMAKE_SOURCE_DIR}/src/Scene/include"
    "${CMAKE_SOURCE_DIR}/src/Components"
    "${CMAKE_SOURCE_DIR}/src/Systems/include"
    # Libraries
    "${CMAKE_SOURCE_DIR}/lib/glad/include"
    "${CMAKE_SOURCE_DIR}/lib/glm"
)

# Link the test executable against GoogleTest
target_link_libraries(UnitTests PRIVATE gtest_main)

# Automatically discover all tests within the test executable
include(GoogleTest)
gtest_discover_tests(UnitTests)