#pragma once

#include <common/util.h>
#include <common/math.h>

// similar with mpm kernel. But not pursuing physically correctness.
// It pursues more acceptable visual effect.

G_NAMESPACE_BEGIN

class RenderKernel {
public:
    RenderKernel() {};

    void calculateWeight(const Vector3 &fx) {
        w[0] = fx.cwiseInverse();
        w[1] = (fx - Vector3::Constant(1.0)).cwiseInverse();
        w[2] = (Vector3::Constant(2.0) - fx).cwiseAbs().cwiseInverse();
        w[3] = (Vector3::Constant(3.0) - fx).cwiseInverse();
        w[4] = (Vector3::Constant(4.0) - fx).cwiseInverse();
    }

    real getWip(int x, int y, int z) const {
        // no boundary check because it's only used for experiment performance.
        return w[x].x() * w[y].y() * w[z].z();
    }

private:
    Vector3 w[5];
};

G_NAMESPACE_END