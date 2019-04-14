#pragma once
#include "../common/util.h"
#include "../common/math.h"

G_NAMESPACE_BEGIN

// an abstract class mainly used for calculate kernel
class WeightKernel
{
public:
    void virtual calculateWeight(const Vector3& pos) = 0;
    real virtual getWip(int x, int y, int z) = 0;
    Vector3 virtual getDWip(int x, int y, int z) = 0;
};

G_NAMESPACE_END