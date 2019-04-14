#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "common/util.h"
#include "common/math.h"
#include "renderer/material.h"

G_NAMESPACE_BEGIN

class Shader
{
public:
    Shader();                 // default constructor
    Shader(const char* vertex, const char* fragment); // given path constructor

    virtual ~Shader();        // destructor
    bool virtual loadShader(const char* vertex, const char* fragment);
    void virtual use();

    // setter
    void setFloat(const std::string &name, float value) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &value) const;
    void setMat3(const std::string &name, const glm::mat3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;

    void setMaterial(const Material &mat) const;

protected:
    uint m_program;             // shader program
};

G_NAMESPACE_END