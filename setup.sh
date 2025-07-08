#!/bin/bash
echo "Setting up project structure..."

# Create source module directories with include/ and src/ subfolders
mkdir -p src/Core/include src/Core/src
mkdir -p src/ECS/include src/ECS/src
mkdir -p src/Renderer/include src/Renderer/src
mkdir -p src/Physics/include src/Physics/src
mkdir -p src/Scene/include src/Scene/src
mkdir -p src/Components
mkdir -p src/Systems/include src/Systems/src

# Create asset directories
mkdir -p assets/models assets/scenes/ shaders/

# Create other top-level directories
mkdir -p lib docs scripts tests

# Create placeholder files in their new locations
touch src/Core/include/Application.hpp
touch src/Core/src/Application.cpp
touch src/ECS/include/EntityManager.hpp
touch src/ECS/src/EntityManager.cpp
touch src/main.cpp
touch tests/EntityManagerTest.cpp
touch CMakeLists.txt
touch .gitignore

echo "Project structure created."