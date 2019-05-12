#include <iostream>
#include <glad/glad.h>
#include <renderer/volumeregion.h>
#include <mpm/QuadraticWeight.h>
#include <renderer/renderkernel.h>


#define UPDATEMINMAX(v, min, max) if(v<min) min=v;if(v>max) max=v;

G_NAMESPACE_BEGIN

VolumeRegion::VolumeRegion() : m_data(nullptr)
{
    m_data = new float[64 * 64 * 64];
    
    glGenTextures(1, &m_textureData);
    glBindTexture(GL_TEXTURE_3D, m_textureData);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, 64, 64, 64, 0, GL_RED, GL_FLOAT, (void*)0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);
}

VolumeRegion::~VolumeRegion()
{
    if (m_data)
        delete[] m_data;

    SafeReleaseTextures(1, m_textureData);
}

void VolumeRegion::updateVolumeData(std::vector<Particle> &particles)
{
    // first decide the grid size
    real minX = 1.0, minY = 1.0, minZ = 1.0;
    real maxX = 0, maxY = 0, maxZ = 0;
    for (Particle p : particles) {
        UPDATEMINMAX(p.pos(0), minX, maxX);
        UPDATEMINMAX(p.pos(1), minY, maxY);
        UPDATEMINMAX(p.pos(2), minZ, maxZ);
    }

    // add a little boundary in oder to avoid truncate coincidence.
    minX -= 0.0001; minY -= 0.0001; minZ -= 0.0001;
    maxX += 0.0001; maxY += 0.0001; maxZ += 0.0001;

    real dimension = 64.0;
    real tripleSize = dimension * dimension * dimension;
    // currently, just divide the bounding space roughly;
    real xFactor = (maxX - minX) / (dimension - 5.0);
    real yFactor = (maxY - minY) / (dimension - 5.0);
    real zFactor = (maxZ - minZ) / (dimension - 5.0);

    Vector3 origin = Vector3(minX - 2.0 * xFactor, minY - 2.0 * yFactor, minZ - 2.0 * zFactor);
    Vector3 endpoint = Vector3(maxX + 3.0 * xFactor, maxY + 3.0 * yFactor, maxZ + 3.0 * zFactor);

    for (int i = 0; i < tripleSize; ++i)
        m_data[i] = 0.0f;

    for (Particle p : particles) {
        Vector3 new_coord = (p.pos - origin).cwiseQuotient(endpoint - origin);      // new coordinates
        Vector3i base_coord = (new_coord * dimension - Vector3::Constant(1.5)).cast<int>();
        Vector3 fx = new_coord * dimension - base_coord.cast<real>();
        RenderKernel w;
        w.calculateWeight(fx);      // calculate Wip
        // Rasterize to grid
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                for (int k = 0; k < 5; ++k) {
                    real wip = w.getWip(i, j, k);
                    m_data[getGridIndex(base_coord(0) + i, base_coord(1) + j, base_coord(2) + k)] += wip * 1.0f;
                }
            }
        }
    }

    // take grid to calculate density
    float volume = xFactor * yFactor * zFactor;
    for (int i = 0; i < tripleSize; ++i)
        m_data[i] = m_data[i] / volume;

    // isolated brick analysis
    // make sure every block has neighbor blocks
    for (int i = 1; i < 63; i++) {
        for (int j = 1; j < 63; j++) {
            for (int k = 1; k < 63; k++) {
                int base = getGridIndex(i, j, k);
                if (m_data[base] == 0) continue;
                if (m_data[base - 1] != 0 || m_data[base + 1] != 0) continue;
                if (m_data[base - 63] != 0 || m_data[base - 64] != 0 || m_data[base - 62] != 0) continue;
                if (m_data[base - 4095] != 0 || m_data[base - 4096] != 0 || m_data[base - 4097] != 0) continue;
                if (m_data[base + 63] != 0 || m_data[base + 64] != 0 || m_data[base + 62] != 0) continue;
                if (m_data[base + 4095] != 0 || m_data[base + 4096] != 0 || m_data[base + 4097] != 0) continue;
                std::cout << "Grid:" << i << "," << j << "," << k << " is isolated." << std::endl;
            }
        }
    }

    // sum all non zero
    int nonzero = 0;
    for (int i = 0; i < tripleSize; ++i)
        if (m_data[i] != 0)
            nonzero++;

    std::cout << nonzero << " are non zero. The percentage is " << ((float)nonzero / tripleSize) << std::endl;

    // update data
    glBindTexture(GL_TEXTURE_3D, m_textureData);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, dimension, dimension, dimension, GL_RED, GL_FLOAT, m_data);
    glBindTexture(GL_TEXTURE_3D, 0);

    glm::vec3 glmorigin((float)origin.x(), (float)origin.y(), (float)origin.z());
    glm::vec3 glmend((float)endpoint.x(), (float)endpoint.y(), (float)endpoint.z());

    // update l2w matrix
    m_l2w = glm::mat4(1.0f);
    m_l2w = glm::translate(m_l2w, (glmend + glmorigin) * 0.5f);
    m_l2w = glm::scale(m_l2w, (glmend - glmorigin) * 0.5f);
}

int VolumeRegion::getGridIndex(int x, int y, int z) const
{
    return (x + y * 64 + z * 64 * 64);
}

G_NAMESPACE_END