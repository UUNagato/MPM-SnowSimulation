#pragma once

#include "WeightKernel.h"

G_NAMESPACE_BEGIN

class QuadraticWeight : public WeightKernel
{
public:
    QuadraticWeight() : calculated(false), WeightKernel() {}

    void virtual calculateWeight(const Vector3& fx) override {
        w[0] = Vector3::Constant(0.5).cwiseProduct(sqr(Vector3::Constant(1.5) - fx));
        w[1] = Vector3::Constant(0.75) - sqr(fx - Vector3::Constant(1.0));
        w[2] = Vector3::Constant(0.5).cwiseProduct(sqr(fx - Vector3::Constant(0.5)));
        dw[0] = fx - Vector3::Constant(1.5); // -(3/2 - |x|)*|x|' = x - 3/2, when fx > 0  
        dw[1] = -2.0 * (fx - Vector3::Constant(1.0)); // -2|x| * |x|' = -2x no matter the sign of fx - 1
        dw[2] = fx - Vector3::Constant(0.5);    // -(3/2 - |x|)*|x|' = 3/2 + x, when fx - 2 < 0
        calculated = true;
    }

    real virtual getWip(int x, int y, int z) override {
        // no boundary check because it's only used for experiment performance.
        return w[x].x() * w[y].y() * w[z].z();
    }

    Vector3 virtual getDWip(int x, int y, int z) override {
        Vector3 ret(dw[x].x() * w[y].y() * w[z].z(),
            w[x].x() * dw[y].y() * w[z].z(),
            w[x].x() * w[y].y() * dw[z].z()
        );
        return ret;
    }

private:
    Vector3 w[3];
    Vector3 dw[3];
    bool calculated;
};

G_NAMESPACE_END