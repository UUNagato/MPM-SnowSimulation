//=========================================================
// MPM snow simulation based on 
// [Stomakhin, Alexey. et al A material point method for snow simulation, 2013]
//=========================================================


#pragma once

#include <vector>
#include "../common/math.h"
#include "mpmkernelbase.h"
#include "particle.h"
#include "grid.h"

G_NAMESPACE_BEGIN

#define ZOBoundary(x, b) (x<b)||(x>1-b)

class MPM : public MPMBase
{
protected:
    real grid_space;            // gird_space
    real grid_inv_space;        // inverse space
    real c_c, c_s;              // critical compression and stretch
    real h_c;                   // harden coefficient
    real i_d;                   // initial density
    real e0;                    // Young's modulus
    real poisson;               // Poisson's ratio
    real dt;
    uint grid_dim;              // grid dimension
    std::vector<Particle> particles;    // particles
    real particle_mass;                 // particle mass
    real particle_volume;               // particle volume
    Grid grid;

    bool first_step;                    // mark whether it's the first step since some special initialization
                                        // works need to be done.

public:
    // Constructor, default parameters are from paper.
    MPM(uint d, real delta_time = 1e-5) :
        c_c(2.5e-2),
        c_s(7.5e-3),
        h_c(10),
        e0(1.4e5),
        poisson(0.2),
        first_step(true),
        dt(delta_time),
        grid_dim(d),
        grid_space(1. / d),
        grid_inv_space(1. / grid_space),
        particle_mass(1.0),
        particle_volume(1.0),
        grid(Vector3u(d, d, d)),
        MPMBase()
    {
    }

    void firstStepCalculation();

    void advance();

    void addObject(Vector3 pos);

    virtual ~MPM()
    {
        MPMBase::~MPMBase();
    }

    FORCE_INLINE real calculateWi(Particle &p, Vector3i &base_coord);

    FORCE_INLINE Vector3 calculateWiF1(const Vector3 &x) {
        Vector3 sqr, cub;
        getSqrAndCub(x, sqr, cub);
        return (Vector3::Constant(0.5).cwiseProduct(cub) - sqr + Vector3::Constant(0.66666f));
    }

    FORCE_INLINE Vector3 calculateWiF2(const Vector3 &x) {
        Vector3 sqr, cub;
        getSqrAndCub(x, sqr, cub);
        return (Vector3::Constant(-1.0 / 6.0).cwiseProduct(cub) + sqr - 2.0 * x
            + Vector3::Constant(1.33333f));
    }

    FORCE_INLINE Vector3 calculateDWi(const Vector3 &x) {
        Vector3 ret;
        for (int i = 0; i < 3; ++i) {
            if (x[i] < 1) {
                ret[i] = (1.5 * x[i] - 2.0) * x[i];
            }
            else if (x[i] < 2) {
                ret[i] = (-0.5 * x[i] + 2.0) * x[i] - 2.0;
            }
            else {
                ret[i] = 0;
            }
        }
        return ret;
    }

    std::vector<Particle>& getParticles() { return particles; }
};

G_NAMESPACE_END