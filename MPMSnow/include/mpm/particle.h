//=======================================================
// MPM Particles
//=======================================================
#pragma once

#include "../common/math.h"

G_NAMESPACE_BEGIN

class Particle
{
public:
    Vector3 pos;        // position
    Vector3 vel;        // velocity
    Matrix3x3 fe;       // elastic part of deformation gradient
    Matrix3x3 C;        // Affine Matrix for APIC
    real Jp;            // Jacobian of plastic part deformation gradient
    real volume;

    Particle(Vector3 position, real vol = 1_f) : pos(position), vel(Vector3::Zero()), volume(vol), fe(Matrix3x3::Identity()),
        C(Matrix3x3::Zero()), Jp(1_f)
    {}
};

G_NAMESPACE_END