#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <renderer/mpmrenderer.h>

G_NAMESPACE_BEGIN

MPMRenderer::MPMRenderer() : m_pointVAO(0), m_pointVBO(0)
{
    m_pointShader.loadShader("./Shader/basicparticle.vs", "./Shader/basicparticle.fs");
    m_rigidbodyShader.loadShader("./Shader/basicparticle.vs", "./Shader/basicparticle.fs");

    glGenBuffers(1, &m_pointVBO);
    glGenVertexArrays(1, &m_pointVAO);
}

MPMRenderer::~MPMRenderer()
{
    SafeReleaseVAO(1, m_pointVAO);
    SafeReleaseBuffer(1, m_pointVBO);
}

void MPMRenderer::renderFrame(const MPM &mpm, Camera &cam)
{
    glm::mat4 model(1.0f);
    // remap from [0,1] to [-1,1]
    model = glm::scale(model, glm::vec3(2.f));
    model = glm::translate(model, glm::vec3(-0.5f));

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(cam.getZoom()), 1024.f / 768.f, 0.1f, 100.0f);
    m_pointShader.use();
    m_pointShader.setMat4("model", model);
    m_pointShader.setMat4("view", view);
    m_pointShader.setMat4("projection", projection);
    m_pointShader.setVec3("pointcolor", glm::vec3(0_f, 0_f, 0.8_f));
    drawParticles(mpm.getParticles());

    m_rigidbodyShader.use();
    m_rigidbodyShader.setMat4("view", view);
    m_rigidbodyShader.setMat4("projection", projection);
    m_rigidbodyShader.setVec3("pointcolor", glm::vec3(1_f, 1_f, 1_f));
    drawRigidbodies(mpm.getRigidBodies());
}

void MPMRenderer::drawParticles(const std::vector<Particle> &particles)
{
    // construct particles data
    float *data = new float[3 * particles.size()];
    for (int i = 0; i < particles.size(); ++i) {
        data[i * 3] = static_cast<float>(particles[i].pos(0));
        data[i * 3 + 1] = static_cast<float>(particles[i].pos(1));
        data[i * 3 + 2] = static_cast<float>(particles[i].pos(2));
    }

    // bind buffer and render
    glBindVertexArray(m_pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * particles.size(), data, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glPointSize(1.f);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete[] data;
}

void MPMRenderer::drawRigidbodies(const std::vector<Rigidbody*> &rigidbodies)
{
    // same thing, remap from [0,1] to [-1, 1]
    glm::mat4 model(1.0f);
    model = glm::scale(model, glm::vec3(2.f));
    model = glm::translate(model, glm::vec3(-0.5f));
    for (Rigidbody* rb : rigidbodies) {
        glm::mat4 l2w = rb->getGLToWorldMatrix();
        m_rigidbodyShader.setMat4("model", model * l2w);
        rb->draw();
    }
}

G_NAMESPACE_END