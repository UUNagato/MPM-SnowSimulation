#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <common/math.h>
#include <mpm/grid.h>
#include <renderer/cube.h>

G_NAMESPACE_BEGIN
// used to generate grids that needed by volume rendering

class VolumeBlock
{
public:
    VolumeBlock();
    ~VolumeBlock();

    void updateBlocks(Grid &grids, float *densities = nullptr);
    void drawFrontFace();
    void drawBackFace();
    void drawBlocks();
    glm::mat4 getBoundingBoxScale();

private:
    uint m_vao;         // vao for grids
    uint m_vbo;         // vbo for grids
    std::vector<float> m_blockVertice;          // block vertices
    
    float m_minX, m_minY, m_minZ;
    float m_maxX, m_maxY, m_maxZ;
};

G_NAMESPACE_END