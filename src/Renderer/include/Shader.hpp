#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int m_ID;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};