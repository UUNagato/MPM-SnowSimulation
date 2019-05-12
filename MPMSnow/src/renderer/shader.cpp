#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glad/glad.h>
#include "renderer/shader.h"

G_NAMESPACE_BEGIN

Shader::Shader() : m_program(0) {}

Shader::Shader(const char* vertex, const char* fragment)
{
    loadShader(vertex, fragment);
}

bool Shader::loadShader(const char* vertex, const char* fragment)
{
    std::string vertex_code;
    std::string frag_code;
    std::ifstream vsFile, fsFile;
    vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // if it has had a program yet
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    try {
        vsFile.open(vertex);
        fsFile.open(fragment);
        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();
        vsFile.close();
        fsFile.close();
        vertex_code = vsStream.str();
        frag_code = fsStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR:SHADER_FILE NOT LOAD SUCCESSFULLY" << std::endl;
    }
    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = frag_code.c_str();

    uint vertex_shader, fragment_shader;

    // Compile shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);

    int success;
    char err_info[255];
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, err_info);
        std::cout << "Vertex shader compile error:\n" << err_info << std::endl;
        return false;
    }
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, err_info);
        std::cout << "fragment shader compile error:\n" << err_info << std::endl;
        glDeleteShader(vertex_shader);
        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 512, NULL, err_info);
        std::cout << "Program link error:\n" << err_info << std::endl;
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    // since the shaders are no longer needed
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return true;
}

void Shader::use()
{
    if (m_program > 0)
        glUseProgram(m_program);
}

Shader::~Shader()
{
    if (m_program > 0)
        glDeleteShader(m_program);
}

#pragma region setter

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_program, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_program, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &value) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}


void Shader::setMaterial(const Material &mat) const
{
    setVec3("albedo", mat.albedo);
    setFloat("metallic", mat.metallic);
    setFloat("roughness", mat.roughness);
    setFloat("ao", mat.ao);

    if (mat.albedoMap > 0) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mat.albedoMap);
    }
    if (mat.normalMap > 0) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, mat.normalMap);
    }
    if (mat.metallicMap > 0) {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, mat.metallicMap);
    }
    if (mat.roughnessMap > 0) {
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, mat.roughnessMap);
    }
    if (mat.aoMap > 0) {
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, mat.aoMap);
    }
}
#pragma endregion

void Shader::validate() const
{
    int success = 0;
    glValidateProgram(m_program);
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_program, 512, NULL, info);
        std::cout << info << std::endl;
    }
}

G_NAMESPACE_END