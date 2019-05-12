#pragma once

#include <vector>
#include <memory>
#include "../common/math.h"

G_NAMESPACE_BEGIN

class GridUnit
{
public:
    real mass;
    Vector3 velocity;

    GridUnit() : mass(0), velocity(Vector3::Zero())
    {}
};

// Grid class
class Grid
{
public:
    
protected:
    Vector3u dimension;         // dimension of girds
    Vector3u real_dimension;    // real dimension.
    std::vector<GridUnit> grids;        // grids

public:
    Grid(Vector3u dim) : dimension(dim)
    {
        real_dimension = dim + Vector3u::Constant(2);
        grids.resize(real_dimension.x() * real_dimension.y() * real_dimension.z());
    }

    Grid() : dimension(Vector3u::Zero())
    {}

    void resize(Vector3u ndim)
    {
        real_dimension = ndim + Vector3u::Constant(2);
        dimension = ndim;
        grids.resize(real_dimension.x() * real_dimension.y() * real_dimension.z());
    }

    GridUnit& operator() (int x, int y, int z)
    {
        x += 1;
        y += 1;
        z += 1;
        uint twod = real_dimension.x() * real_dimension.y();
        return grids[z * twod + y * real_dimension.x() + x];
    }
    
    // Reset the grids
    void reset()
    {
        memset(grids.data(), 0, sizeof(GridUnit) * grids.size());
    }

    // Get dimension of Grid
    Vector3u getDimension()
    {
        return dimension;
    }
};

G_NAMESPACE_END