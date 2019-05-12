//=========================================================
// MPM snow simulation based on 
// [Stomakhin, Alexey. et al A material point method for snow simulation, 2013]
//=========================================================


#pragma once

#include <vector>
#include <common/math.h>
#include <mpm/mpmkernelbase.h>
#include <mpm/particle.h>
#include <mpm/grid.h>
#include <mpm/rigidbody.h>

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
    std::vector<Rigidbody*> rigidbodies; // rigidbodies
    real particle_mass;                 // particle mass
    real particle_volume;               // particle volume
    Grid grid;

    bool first_step;                    // mark whether it's the first step since some special initialization
                                        // works need to be done.

public:
    // Constructor, default parameters are from paper.
    MPM(uint d, real delta_time = 2e-5) :
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

    void addCollideBody(Rigidbody *body, const Matrix4x4 &world);

    virtual ~MPM()
    {
        // clear rigidbodies
        for (Rigidbody *rig : rigidbodies)
            delete rig;
        rigidbodies.clear();
        MPMBase::~MPMBase();
    }

    const std::vector<Particle>& getParticles() const { return particles; }
    const std::vector<Rigidbody*>& getRigidBodies() const { return rigidbodies; }
};

G_NAMESPACE_END