#include <fstream>
#include <sstream>
#include <iostream>
#include "renderer/renderer.h"

G_NAMESPACE_BEGIN

Renderer::Renderer() : m_program(0) {}

Renderer::Renderer(const char* vertex, const char* frag) : Renderer()
{
    useShader(vertex, frag);
}

bool Renderer::useShader(const char* vertex, const char* frag)
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
        fsFile.open(frag);
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

Renderer::~Renderer()
{
    if (m_program != 0)
        glDeleteProgram(m_program);
}


/*
#pragma region Legacy
void Renderer::initialize()
{
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
}

void Renderer::renderParticles()
{
    glUseProgram(m_program);
    glBindVertexArray(vao);
    // load vertices

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::renderParticles(std::vector<Particle> &particles)
{
    glUseProgram(m_program);

    size_t psize = particles.size();
    float *pos = new float[psize * 3];
    // load vertices
    for (size_t i = 0; i < psize; ++i) {
        size_t base = i * 3;
        pos[base] = 2.0 * particles[i].pos(0) - 1.0;
        pos[base + 1] = 2.0 * particles[i].pos(2) - 1.0;
        pos[base + 2] = 2.0 * particles[i].pos(1) - 1.0;
    }

    // bind buffer
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * psize * 3, pos, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, psize);

    delete[] pos;
}
#pragma endregion
*/
G_NAMESPACE_END