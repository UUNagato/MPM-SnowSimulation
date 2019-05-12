#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <common/math.h>
#include <mpm/particle.h>

G_NAMESPACE_BEGIN

// This class create volume data and proxy geometry by analyzing mpm particles.
class VolumeRegion
{
public:
    VolumeRegion();
    ~VolumeRegion();

    void updateVolumeData(std::vector<Particle> &particles);

    int getGridIndex(int x, int y, int z) const;
    uint getVolumeTexture() const { return m_textureData; }

    glm::mat4 getBoundingBox() const { return m_l2w; }

private:
    float m_minBlockSize;       // the minimal block size in MPM coordinates
    float m_maxBlockSize;       // the maximal block size in MPM coordinates

    float *m_data;

    uint m_textureData;         // 3d texture data
    glm::mat4 m_l2w;            // local to world
};

G_NAMESPACE_END