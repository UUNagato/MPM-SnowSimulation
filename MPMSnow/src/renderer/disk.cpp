#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <renderer/disk.h>

G_NAMESPACE_BEGIN

DiskShape::DiskShape(uint seg) : segment(seg), vao(0), vbo(0), ebo(0), indice_number(0)
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

    // first add two center point
    positions.push_back(glm::vec3(0.f, 1.f, 0.f));
    uv.push_back(glm::vec2(0.25f, 0.25f));
    normals.push_back(glm::vec3(0.f, 1.f, 0.f));

    positions.push_back(glm::vec3(0.f));
    uv.push_back(glm::vec2(0.75f, 0.75f));
    normals.push_back(glm::vec3(0.f, -1.f, 0.f));

    const float PI = 3.14159265359f;
    for (uint x = 0; x <= segment; ++x) {
        float f_seg = (float)segment;
        float xSeg = (float)x / f_seg;
        float xPos = std::cos(xSeg * 2.0f * PI);
        float zPos = std::sin(xSeg * 2.0f * PI);

        // every position needs two normals and two uvs.
        positions.push_back(glm::vec3(xPos, 1.f, zPos));
        glm::vec2 baseUV = glm::vec2(xPos, zPos) * 0.25f;
        uv.push_back(baseUV + glm::vec2(0.25f, 0.25f));
        normals.push_back(glm::vec3(0.f, 1.f, 0.f));

        positions.push_back(glm::vec3(xPos, 1.f, zPos));
        uv.push_back(glm::vec2(xSeg, 0.5f));
        normals.push_back(glm::vec3(xPos, 0.f, zPos));

        positions.push_back(glm::vec3(xPos, 0.f, zPos));
        uv.push_back(baseUV + glm::vec2(0.75f, 0.25f));
        normals.push_back(glm::vec3(0.f, -1.f, 0.f));

        positions.push_back(glm::vec3(xPos, 0.f, zPos));
        uv.push_back(glm::vec2(xSeg, 1.0f));
        normals.push_back(glm::vec3(xPos, 0.f, zPos));
    }

    // for indice.
    // every four positions. They are up-up, up-out, down-down, down-out
    for (uint i = 0; i < segment - 1; ++i) {
        uint start_pos = i * 4 + 2;
        // first connect upper
        indices.push_back(start_pos);
        indices.push_back(start_pos + 4);
        indices.push_back(0);
        // then connect side
        indices.push_back(start_pos + 1);
        indices.push_back(start_pos + 3);
        indices.push_back(start_pos + 5);
        indices.push_back(start_pos + 3);
        indices.push_back(start_pos + 5);
        indices.push_back(start_pos + 7);
        // last connect lower
        indices.push_back(start_pos + 2);
        indices.push_back(start_pos + 6);
        indices.push_back(1);
    }
    // first connect upper
    uint start_pos = 4 * segment - 4;
    indices.push_back(start_pos);
    indices.push_back(4);
    indices.push_back(0);
    // then connect side
    indices.push_back(start_pos + 1);
    indices.push_back(start_pos + 3);
    indices.push_back(3);
    indices.push_back(start_pos + 3);
    indices.push_back(3);
    indices.push_back(5);
    // last connect lower
    indices.push_back(start_pos + 2);
    indices.push_back(4);
    indices.push_back(1);

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

DiskShape::~DiskShape()
{
    if (vao > 0)
        glDeleteVertexArrays(1, &vao);
    if (ebo > 0)
        glDeleteBuffers(1, &ebo);
    if (vbo > 0)
        glDeleteBuffers(1, &vbo);
}

void DiskShape::draw()
{
    if (vao > 0) {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLE_STRIP, indice_number, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

G_NAMESPACE_END