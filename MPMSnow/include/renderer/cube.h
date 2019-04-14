#pragma once
// cube shape, mainly used for cubemap render.
#include "common/math.h"

G_NAMESPACE_BEGIN

class CubeShape
{
public:
    CubeShape();
    ~CubeShape();

    uint virtual getVAO() { return vao; }
    void virtual draw();

protected:
    uint vao;
    uint vbo;
};

G_NAMESPACE_END