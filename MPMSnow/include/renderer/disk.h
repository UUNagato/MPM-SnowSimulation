#pragma once

// Disk shape
#include "common/math.h"

G_NAMESPACE_BEGIN

class DiskShape
{
public:
    DiskShape(uint seg = 32);
    ~DiskShape();

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