#pragma once

// Generate Sphere shape class
// I didn't design it too detail or inherit from some abstract classes like Shape.
// It's only used to debug pbrt system. So I don't need other shapes.
// That's why I directly coded this class

#include "common/math.h"

G_NAMESPACE_BEGIN

class SphereShape
{
public:
    SphereShape(uint seg = 32);
    ~SphereShape();

    uint getVAO() { return vao; }
    uint getIndexCount() { return indice_number; }

    void virtual draw();

private:
    uint segment;
    uint vbo;
    uint ebo;
    uint vao;
    uint indice_number;
};

G_NAMESPACE_END