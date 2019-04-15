#pragma once

#include "common/math.h"

G_NAMESPACE_BEGIN

class Quad
{
public:
    Quad();
    ~Quad();

    uint virtual getVAO() { return vao; }
    void virtual draw();

protected:
    uint vao;
    uint vbo;
};

G_NAMESPACE_END