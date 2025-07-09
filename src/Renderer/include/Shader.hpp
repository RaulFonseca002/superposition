#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    // The OpenGL ID for the shader program
    unsigned int m_ID;

    // Constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Activates the shader program
    void use();
    
    // Uniform setter utility functions
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setFloat(const std::string& name, float value) const;

private:
    // A private helper function to check for compile/link errors
    void checkCompileErrors(unsigned int shader, std::string type);
};