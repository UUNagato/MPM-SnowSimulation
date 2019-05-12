#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <renderer/sphere.h>

G_NAMESPACE_BEGIN

SphereShape::SphereShape(uint seg) : segment(seg), vao(0), vbo(0), ebo(0), indice_number(0)
{
    if (seg <= 3)
        segment = 3;

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<uint> indices;

    const float PI = 3.14159265359f;
    for (uint y = 0; y <= segment; ++y)
        for (uint x = 0; x <= segment; ++x) {
            float f_seg = (float)segment;
            float xSeg = (float)x / f_seg;
            float ySeg = (float)y / f_seg;
            float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
            float yPos = std::cos(ySeg * PI);
            float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSeg, ySeg));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }

    bool oddRow = false;
    for (int y = 0; y < segment; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (int x = 0; x <= segment; ++x)
            {
                indices.push_back(y       * (segment + 1) + x);
                indices.push_back((y + 1) * (segment + 1) + x);
            }
        }
        else
        {
            for (int x = segment; x >= 0; --x)
            {
                indices.push_back((y + 1) * (segment + 1) + x);
                indices.push_back(y       * (segment + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    indice_number = indices.size();

    std::vector<float> data;
    for (int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
    uint stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
}

SphereShape::~SphereShape()
{
    if (vao > 0)
        glDeleteVertexArrays(1, &vao);
    if (ebo > 0)
        glDeleteBuffers(1, &ebo);
    if (vbo > 0)
        glDeleteBuffers(1, &vbo);
}

void SphereShape::draw()
{
    if (vao > 0) {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, indice_number, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

G_NAMESPACE_END