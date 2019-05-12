#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <common/math.h>
#include <mpm/mpm.h>
#include <renderer/camera.h>
#include <renderer/shader.h>

// this is a simple renderer, camera is enabled. 
// Every particle is rendered by points. Rigidbody are outputed by pure colors. (no lighting)
// I rendered like this is because the most important one, snow rendering, is rendered in Houdini.

G_NAMESPACE_BEGIN

class MPMRenderer
{
public:
    MPMRenderer();
    ~MPMRenderer();

    void renderFrame(const MPM &mpm, Camera &cam);

private:
    Shader m_pointShader;
    Shader m_rigidbodyShader;

    uint m_pointVBO;
    uint m_pointVAO;

    void drawParticles(const std::vector<Particle> &particles);
    void drawRigidbodies(const std::vector<Rigidbody*> &rigidbodies);
};

G_NAMESPACE_END