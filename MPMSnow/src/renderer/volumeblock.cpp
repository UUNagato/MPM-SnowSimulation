#include <glm/ext/matrix_transform.hpp>
#include <renderer/volumeblock.h>

#define CHECK_MIN(x,min) if(x<min) min=x;
#define CHECK_MAX(x,max) if(x>max) max=x;

G_NAMESPACE_BEGIN

VolumeBlock::VolumeBlock() : m_vao(0), m_vbo(0), m_minX(-1.0f), m_maxX(1.0f), m_minY(-1.0f), m_maxY(1.0f),
m_minZ(-1.0f), m_maxZ(1.0f) 
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
}

VolumeBlock::~VolumeBlock()
{
    SafeReleaseVAO(1, m_vao);
    SafeReleaseBuffer(1, m_vbo);
}

void VolumeBlock::updateBlocks(Grid &grids, float *densities)
{
    // Loop grids to find available grid
    m_blockVertice.clear();         // Here assuming clear won't release memory.
    Vector3u dim = grids.getDimension();
    float space = 1.0f / (float)dim(0);
    float volume = space * space * space * 1000.0f;

    // bounding box calculation
    m_minX = 999.0f; m_minY = 999.0f; m_minZ = 999.0f;
    m_maxX = -1.0f; m_maxY = -1.0f; m_maxZ = -1.0f;

    for (uint i = 0; i < dim[0]; ++i)
        for (uint j = 0; j < dim[1]; ++j)
            for (uint k = 0; k < dim[2]; ++k) {
                // if grid mass isn't zero, it means it contributes to density
                GridUnit &g = grids(i, j, k);
                float x = (float)i, y = (float)k, z = (float)j;     // y,z swaped
                float x1 = x + 1.0f, y1 = y + 1.0f, z1 = z + 1.0f;

                if (g.mass > 0) {
                    if (densities != nullptr)
                        densities[j * dim(0) * dim(2) + k * dim(0) + i] = g.mass / volume;
                    // Update bounding box
                    CHECK_MIN(x*space, m_minX); CHECK_MAX(x1*space, m_maxX);
                    CHECK_MIN(y*space, m_minY); CHECK_MAX(y1*space, m_maxY);
                    CHECK_MIN(z*space, m_minZ); CHECK_MAX(z1*space, m_maxZ);

                    /*
                    // draw cube
                    // front face
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);       // bottom-left
                    m_blockVertice.push_back(x1 * space);  m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);      // bottom-right
                    m_blockVertice.push_back(x1 * space);  m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);      // top-right
                    m_blockVertice.push_back(x1 * space);  m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);      // top-right
                    m_blockVertice.push_back(x * space);  m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);      // top-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);      // bottom-left
                    // left face
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);  // bottom-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);  // top-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);  // top-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-left
                    // right face
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);       // bottom-left
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);        // top-left
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);        // bottom-left
                    // top face
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);       // bottom-left
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);        // bottom-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z1 * space);        // bottom-left
                    // bottom face
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);       // bottom-left
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);        // top-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);        // top-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z1 * space);        // top-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-left
                    // backface
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);       // bottom-left
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-right
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y1 * space);
                    m_blockVertice.push_back(z * space);        // top-left
                    m_blockVertice.push_back(x1 * space);    m_blockVertice.push_back(y * space);
                    m_blockVertice.push_back(z * space);        // bottom-left*/
                }
                else {
                    if (densities != nullptr)
                        densities[j * dim(0) * dim(2) + k * dim(0) + i] = 0;
                }
            }

    // extend bounding box for better interpolation
    m_maxX += space;    m_maxY += space;    m_maxZ += space;
    m_minX -= space;    m_minY -= space;    m_minZ -= space;

    // create bounding blocks
    // front face
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);       // bottom-left
    m_blockVertice.push_back(m_maxX);  m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);      // bottom-right
    m_blockVertice.push_back(m_maxX);  m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);      // top-right
    m_blockVertice.push_back(m_maxX);  m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);      // top-right
    m_blockVertice.push_back(m_minX);  m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);      // top-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);      // bottom-left
    // left face
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);  // bottom-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);  // top-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);  // top-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-left
    // right face
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);       // bottom-left
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);        // top-left
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);        // bottom-left
    // top face
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);       // bottom-left
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);        // bottom-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_maxZ);        // bottom-left
    // bottom face
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);       // bottom-left
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);        // top-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);        // top-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_maxZ);        // top-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-left
    // backface
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);       // bottom-left
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_minX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-right
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_maxY);
    m_blockVertice.push_back(m_minZ);        // top-left
    m_blockVertice.push_back(m_maxX);    m_blockVertice.push_back(m_minY);
    m_blockVertice.push_back(m_minZ);        // bottom-left

    // bind vbo
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_blockVertice.size() * sizeof(float), &m_blockVertice[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void VolumeBlock::drawFrontFace()
{
    glClearDepth(1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthFunc(GL_LESS);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_blockVertice.size() / 3);
}

void VolumeBlock::drawBackFace()
{
    glClearDepth(0.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthFunc(GL_GREATER);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_blockVertice.size() / 3);
    glCullFace(GL_BACK);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);
}

void VolumeBlock::drawBlocks()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_blockVertice.size() / 3);
}

glm::mat4 VolumeBlock::getBoundingBoxScale()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3((m_maxX + m_minX) * 0.5f, (m_maxY + m_minY) * 0.5f, (m_maxZ + m_minZ) * 0.5f));
    model = glm::scale(model, glm::vec3((m_maxX - m_minX) * 0.5f, (m_maxY - m_minY) * 0.5f, (m_maxZ - m_minZ) * 0.5f));
    return model;
}

G_NAMESPACE_END