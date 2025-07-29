#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
    public:
        unsigned int m_ID;
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        void use();
        
        void setMat4(const std::string& name, const glm::mat4& mat) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setFloat(const std::string& name, float value) const;

    private:
        void checkCompileErrors(unsigned int shader, std::string type);
};