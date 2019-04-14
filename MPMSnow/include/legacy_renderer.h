#pragma once

#include <vector>
#include <string>
#include "util.h"
#include "particle.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

G_NAMESPACE_BEGIN

#define MPM_VERTEX_SHADER_PATH "./Shader/vs.glsl"
#define MPM_FRAGMENT_SHADER_PATH "./Shader/ps.glsl"

// a renderer class which receives particle array as input and then render them out.
class Renderer
{
public:
    Renderer();
    void renderParticles();
    void renderParticles(std::vector<Particle>& particles);
    void initialize();

    virtual ~Renderer();

protected:
    uint m_program;

    uint vbo;
    uint vao;

private:
};

G_NAMESPACE_END