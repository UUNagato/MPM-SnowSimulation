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

    Particle(Vector3 position) : pos(position), vel(Vector3::Zero()), fe(Matrix3x3::Identity()),
        C(Matrix3x3::Zero()), Jp(1)
    {}
};

G_NAMESPACE_END