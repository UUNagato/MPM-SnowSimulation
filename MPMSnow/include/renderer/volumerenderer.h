#pragma once
#include <vector>
#include <common/math.h>
#include <mpm/particle.h>
#include <mpm/grid.h>
#include <glad/glad.h>
#include <renderer/shader.h>
#include <renderer/camera.h>
#include <renderer/cube.h>
#include <renderer/volumeregion.h>

// Volume Renderer for snow rendering

G_NAMESPACE_BEGIN

class VolumeRenderer
{
public:
    VolumeRenderer(int width, int height, int dim = 256);
    ~VolumeRenderer();

    void RenderSnow(std::vector<Particle> &particles, Camera &cam);
    void ViewportSizeChanged(int width, int height);

    // just for debug
    uint getStartMap() { return m_startTexture; }
    uint getEndMap() { return m_endTexture; }

private:
    int m_width;
    int m_height;
    int m_dimension;
    float m_gridsize;

    Grid m_grids;

    uint m_fbo;
    uint m_rbo;
    uint m_startTexture;
    uint m_endTexture;
    VolumeRegion m_region;
    float *densities;

    Shader m_objectSpaceSkipShader;
    Shader m_snowShader;

    CubeShape boundingCube;

    void rasterizeToGrid(std::vector<Particle> &particles);
    void generateVolumeData();

    void drawFrontFaces();
    void drawBackFaces();
};

G_NAMESPACE_END