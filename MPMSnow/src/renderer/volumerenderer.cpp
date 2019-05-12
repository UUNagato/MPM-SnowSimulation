#include <iostream>
#include <renderer/volumerenderer.h>
#include <mpm/QuadraticWeight.h>
#include <glm/glm.hpp>
#include <renderer/cube.h>

G_NAMESPACE_BEGIN

VolumeRenderer::VolumeRenderer(int width, int height, int dim) :
    m_width(width), m_height(height), m_startTexture(0), m_endTexture(0), m_fbo(0), m_rbo(0)
{
    // grid initialize
    m_dimension = 256;
    while (m_dimension < dim)  m_dimension <<= 1;
    m_grids.resize(Vector3u::Constant(m_dimension));
    m_gridsize = 1.f / m_dimension;

    ViewportSizeChanged(width, height);

    // shader
    m_objectSpaceSkipShader.loadShader("./Shader/volume/objectspaceskip_vs.glsl",
        "./Shader/volume/objectspaceskip_fs.glsl");
    m_snowShader.loadShader("./Shader/volume/snow_vs.glsl", "./Shader/volume/snow_emission_fs.glsl");
    m_snowShader.use();
    m_snowShader.setInt("startMap", 0);
    m_snowShader.setInt("endMap", 1);
    m_snowShader.setInt("volumeData", 2);

    // make sure 3d texture is enabled
    glEnable(GL_TEXTURE_3D);
}

VolumeRenderer::~VolumeRenderer()
{
    if (m_rbo > 0)
        glDeleteRenderbuffers(1, &m_rbo);
    if (m_fbo > 0)
        glDeleteRenderbuffers(1, &m_fbo);
    SafeReleaseTextures(1, m_startTexture);
    SafeReleaseTextures(1, m_endTexture);

    if (densities)
        delete[] densities;
}

void VolumeRenderer::ViewportSizeChanged(int width, int height)
{
    m_width = width;
    m_height = height;
    // create texture
    if (m_startTexture > 0)
        glDeleteTextures(1, &m_startTexture);
    glGenTextures(1, &m_startTexture);
    glBindTexture(GL_TEXTURE_2D, m_startTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (m_endTexture > 0)
        glDeleteTextures(1, &m_endTexture);
    glGenTextures(1, &m_endTexture);
    glBindTexture(GL_TEXTURE_2D, m_endTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // create fbo
    if (m_rbo > 0)
        glDeleteRenderbuffers(1, &m_rbo);
    if (m_fbo > 0)
        glDeleteFramebuffers(1, &m_fbo);

    
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool renderonce = false;
void VolumeRenderer::RenderSnow(std::vector<Particle> &particles, Camera &cam)
{
    if (renderonce == false) {
        rasterizeToGrid(particles);
        renderonce = true;
    }
    m_objectSpaceSkipShader.use();
    // set cam, etc
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(10.0f));
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(cam.getZoom()), (float)m_width/ (float)m_height, 0.1f, 100.0f);
    m_objectSpaceSkipShader.setMat4("view", view);
    m_objectSpaceSkipShader.setMat4("projection", projection);

    glm::mat4 boundingBox = model * m_region.getBoundingBox();
    m_objectSpaceSkipShader.setMat4("model", boundingBox);

    drawFrontFaces();
    drawBackFaces();

    // render snow
    // use blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_snowShader.use();
    m_snowShader.setMat4("model", boundingBox);
    m_snowShader.setMat4("view", view);
    m_snowShader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_startTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_endTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, m_region.getVolumeTexture());
    boundingCube.draw();
}

void VolumeRenderer::rasterizeToGrid(std::vector<Particle> &particles)
{
    m_region.updateVolumeData(particles);
}

void VolumeRenderer::generateVolumeData()
{
    // Update texture
}

void VolumeRenderer::drawFrontFaces()
{
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_startTexture, 0);
    // render to it
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw it
    boundingCube.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumeRenderer::drawBackFaces()
{
    glCullFace(GL_FRONT);
    glDepthFunc(GL_GREATER);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_endTexture, 0);
    glClearColor(0.f, 0.f, 1.f, 1.f);
    glClearDepth(0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    boundingCube.draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glClearDepth(1.0);
}

G_NAMESPACE_END