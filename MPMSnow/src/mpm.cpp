#include "mpm/mpm.h"
#include "common/util.h"
#include "mpm/QuadraticWeight.h"
#include <cmath>

G_NAMESPACE_BEGIN

// Stomakin
/*
#pragma region Stomakin
void MPM::advance()
{
    if (first_step) {
        firstStepCalculation();
        first_step = false;
    }
    grid.reset();

    real mu0 = e0 / (2.0 * (1.0 + poisson));
    real lambda0 = e0 * poisson / ((1 + poisson) * (1 - 2 * poisson));

    for (Particle& p : particles) {
        // calculate Wip
        Vector3i basic_coord = (p.pos * grid_inv_space).cast<int>();
        Vector3 fx = (p.pos * grid_inv_space) - basic_coord.cast<real>();
        Vector3 one = Vector3::Ones();
        Vector3 wi[4] = { calculateWiF2(fx + one), calculateWiF1(fx), calculateWiF1(one - fx),
                        calculateWiF2(one - fx + one) };
        Vector3 dwi[4] = { calculateDWi(fx + one), calculateDWi(fx), calculateDWi(one - fx),
                        calculateDWi(one - fx + one) };

        // Calculate force.
        Eigen::JacobiSVD<Matrix3x3> svd(p.fe, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Vector3 singularValues = svd.singularValues();
        bool update_elastic = false;
        Matrix3x3 new_singular_matrix = Matrix3x3::Zero();
        // clamp the singular values
        for (int i = 0; i < 3; ++i) {
            if (singularValues(i) > 1 + c_s) {
                update_elastic = true;
                singularValues(i) = 1 + c_s;
            }
            if (singularValues(i) < 1 - c_c) {
                update_elastic = true;
                singularValues(i) = 1 - c_c;
            }
            new_singular_matrix(i, i) = singularValues(i);
        }

        real Jf = p.fe.determinant() * p.Jp;

        if (update_elastic) {
            p.fe = svd.matrixU() * new_singular_matrix * svd.matrixV().transpose();
            p.Jp = Jf / p.fe.determinant();       // Update Jp
        }

        Matrix3x3 r = svd.matrixU() * svd.matrixV().transpose();
        real J = singularValues(0) * singularValues(1) * singularValues(2);
        real hardenFactor = std::exp(h_c * (1 - p.Jp));
        real mu = mu0 * hardenFactor;
        real lambda = lambda0 * hardenFactor;

        //Matrix3x3 first_piola = 2.0 * mu * (p.fe - r) + lambda * (J - 1) * J * p.fe.inversetranspose();
        // cauchy_stress is (1/J)first_piolaF^T
        // so that we can directly calculate cauchy_stress
        Matrix3x3 cauchy_stress = (2.0 * mu) * (p.fe - r) * p.fe.transpose() + lambda * (J - 1) * J * Matrix3x3::Identity();
        // stress: J * Jp * p.volume is current volume. But when calculating cauchy_stress, Jp is devided
        Matrix3x3 stress = -J * p.volume * cauchy_stress;


        // Rasterize to Grid mass and velocity
        for (int i = -1; i < 3; ++i)
            for (int j = -1; j < 3; ++j)
                for (int k = -1; k < 3; ++k) {
                    GridUnit &g = grid(basic_coord[0] + i, basic_coord[1] + j, basic_coord[2] + k);
                    real wip = wi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z();
                    g.mass += particle_mass * wip;
                    Vector3 velocity = p.vel * particle_mass * wip;
                    Vector3 dwip(dwi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z(),
                        wi[i + 1].x() * dwi[j + 1].y() * wi[k + 1].z(),
                        wi[i + 1].x() * wi[j + 1].y() * dwi[k + 1].z());
                    dwip = dwip * grid_inv_space;           // finish derivative.
                    velocity += dt * stress * dwip;         // add force to velocity
                    g.velocity += velocity;                 // normalize is put off.
                }
    }

    // For every node
    for (int i = 0; i < grid_dim; ++i)
        for (int j = 0; j < grid_dim; ++j)
            for (int k = 0; k < grid_dim; ++k) {
                // normalize velocity
                GridUnit &g = grid(i, j, k);
                if (g.mass > 0) {
                    g.velocity /= g.mass;                       // normailize speed
                    g.velocity += dt * Vector3(0, 0, -200);     // gravity
                    // grid boundary
                    real boundary = 0.1, x = (real)i * grid_space, y = (real)j * grid_space,
                        z = (real)k * grid_space;
                    if (ZOBoundary(x, boundary) || ZOBoundary(y, boundary) || z > 1 - boundary)
                        g.velocity.setZero();
                    else if (z < boundary)
                        g.velocity.z() = std::max(0.0, g.velocity.z());
                }
            }
    
    // G2P
    for (Particle& p : particles) {
        // calculate Wip
        Vector3i basic_coord = (p.pos * grid_inv_space).cast<int>();
        Vector3 fx = (p.pos * grid_inv_space) - basic_coord.cast<real>();
        Vector3 one = Vector3::Ones();
        Vector3 wi[4] = { calculateWiF2(fx + one), calculateWiF1(fx), calculateWiF1(one - fx),
                        calculateWiF2(one - fx + one) };
        Vector3 dwi[4] = { calculateDWi(fx + one), calculateDWi(fx), calculateDWi(one - fx),
                        calculateDWi(one - fx + one) };
        
        Vector3 picv = Vector3::Zero();
        Vector3 flipv = p.vel;
        Matrix3x3 dv = Matrix3x3::Zero();
        for (int i = -1; i < 3; ++i)
            for (int j = -1; j < 3; ++j)
                for (int k = -1; k < 3; ++k) {
                    GridUnit &g = grid(basic_coord[0] + i, basic_coord[1] + j, basic_coord[2] + k);
                    real wip = wi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z();
                    Vector3 dwip(dwi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z(),
                        wi[i + 1].x() * dwi[j + 1].y() * wi[k + 1].z(),
                        wi[i + 1].x() * wi[j + 1].y() * dwi[k + 1].z());
                    // update deformation gradient
                    dv += g.velocity * dwip.transpose();

                    // use PIC-FLIP blend to calculate velocity
                    picv += g.velocity * wip;
                    Vector3 pv2g = p.vel * particle_mass * wip / g.mass;
                    flipv += (g.velocity - pv2g) * wip;
                }
        p.fe += dt * dv * p.fe;
        p.vel = 0.05 * picv + 0.95 * flipv;
        if (p.pos.z() < 0.05)
            p.vel.z() = std::max(0.0, p.vel.z());
        p.pos += dt * p.vel;
    }
}
#pragma endregion
*/

// APIC
#pragma region APIC

// APIC version
void MPM::advance()
{
    // reset grid
    grid.reset();

    // calculate mu and lambda
    real mu0 = e0 / (2.0 * (1.0 + poisson));
    real lambda0 = e0 * poisson / ((1 + poisson) * (1 - 2 * poisson));

    // P2G, use quadratic spline
    for (Particle &p : particles) {
        Vector3i base_coord = (p.pos * grid_inv_space - Vector3::Constant(0.5)).cast<int>();
        Vector3 fx = p.pos * grid_inv_space - base_coord.cast<real>();
        QuadraticWeight w;
        w.calculateWeight(fx);      // calculate Wip

        // use svd to do polar decomp
        Matrix3x3 r, s;
        polarDecomposite(p.fe, r, s);

        // calculate lame param
        real J = p.fe.determinant();
        real hardenFactor = std::exp(h_c * (1 - p.Jp));
        real mu = mu0 * hardenFactor;
        real lambda = lambda0 * hardenFactor;

        // directly calculate cauchy
        Matrix3x3 cauchy = (2.0 * mu * (p.fe - r) * p.fe.transpose()) + (lambda * (J - 1) * J * Matrix3x3::Identity());
        // force // calculate dwip with trilinear
        Matrix3x3 force = -4.0 * grid_inv_space * grid_inv_space * particle_volume * cauchy;

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k) {
                    // contribute mass
                    Vector3 dpos = (Vector3(i, j, k) - fx) * grid_space;
                    GridUnit &g = grid(base_coord.x() + i, base_coord.y() + j, base_coord.z() + k);
                    real wip = w.getWip(i, j, k);
                    g.mass += particle_mass * wip;
                    g.velocity += wip * (particle_mass * (p.vel + p.C * dpos) + dt * force * dpos);
                }
    }

    // for all grid nodes
    for (int i = 0; i < grid_dim; ++i)
        for (int j = 0; j < grid_dim; ++j)
            for (int k = 0; k < grid_dim; ++k) {
                // normalize mass
                GridUnit &g = grid(i, j, k);
                if (g.mass > 0) {
                    g.velocity /= g.mass;                           // normalize velocity
                    g.velocity += dt * Vector3(0, 0, -200);         // gravity

                    real boundary = 0.05, x = (real)i * grid_space, y = (real)j * grid_space,
                        z = (real)k * grid_space;
                    if (ZOBoundary(x, boundary) || ZOBoundary(y, boundary) || z > 1 - boundary)
                        g.velocity.setZero();                                   // sticky
                    else if (z < boundary)
                        g.velocity.z() = std::max(0.0, g.velocity.z());         // bounce
                }
                else {
                    g.mass = 0;
                    g.velocity.setZero();
                }
            }

    // G2P
    for (Particle &p : particles) {
        Vector3i base_coord = (p.pos * grid_inv_space - Vector3::Constant(0.5)).cast<int>();
        Vector3 fx = p.pos * grid_inv_space - base_coord.cast<real>();
        QuadraticWeight w;
        w.calculateWeight(fx);      // calculate Wip

        p.C.setZero();
        p.vel.setZero();

        // APIC transfer
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k) {
                    Vector3 dpos = (Vector3(i, j, k) - fx);
                    GridUnit &g = grid(base_coord.x() + i, base_coord.y() + j, base_coord.z() + k);
                    real wip = w.getWip(i, j, k);
                    p.vel += wip * g.velocity;
                    p.C += wip * (4.0 * grid_inv_space) * (g.velocity * dpos.transpose());
                }
        p.pos += p.vel * dt;

        // update F
        p.fe = p.fe + dt * p.C * p.fe;
        // constraint to the given range
        Eigen::JacobiSVD<Matrix3x3> svd(p.fe, Eigen::ComputeFullU | Eigen::ComputeFullV);

        Vector3 singularvalues = svd.singularValues();
        Matrix3x3 newSingularMat = Matrix3x3::Zero();
        for (int i = 0; i < 3; ++i)
            newSingularMat(i, i) = clamp(singularvalues(i), 1 - c_c, 1 + c_s);

        // old J
        real oldJ = p.fe.determinant() * p.Jp;
        p.fe = svd.matrixU() * newSingularMat * svd.matrixV().transpose();
        p.Jp = oldJ / p.fe.determinant();
    }
}
#pragma endregion

void MPM::firstStepCalculation()
{
    grid.reset();

    // calculate mass
    for (Particle p : particles) {
        // calculate Wip
        Vector3i basic_coord = (p.pos * grid_inv_space).cast<int>();
        Vector3 fx = (p.pos * grid_inv_space) - basic_coord.cast<real>();
        Vector3 one = Vector3::Ones();
        Vector3 wi[4] = { calculateWiF2(fx + one), calculateWiF1(fx), calculateWiF1(one - fx),
                        calculateWiF2(one - fx + one) };

        // Rasterize grid to particle
        for (int i = -1; i < 3; ++i)
            for (int j = -1; j < 3; ++j)
                for (int k = -1; k < 3; ++k) {
                    GridUnit &g = grid(basic_coord[0] + i, basic_coord[1] + j, basic_coord[2] + k);
                    real wip = wi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z();
                    g.mass += particle_mass * wip;
                }
    }

    // initialize volume of particle
    // calculate mass
    for (Particle p : particles) {
        // calculate Wip
        Vector3i basic_coord = (p.pos * grid_inv_space).cast<int>();
        Vector3 fx = (p.pos * grid_inv_space) - basic_coord.cast<real>();
        Vector3 one = Vector3::Ones();
        Vector3 wi[4] = { calculateWiF2(fx + one), calculateWiF1(fx), calculateWiF1(one - fx),
                        calculateWiF2(one - fx + one) };

        // Rasterize grid to particle
        real particle_density = 0;
        for (int i = -1; i < 3; ++i)
            for (int j = -1; j < 3; ++j)
                for (int k = -1; k < 3; ++k) {
                    GridUnit &g = grid(basic_coord[0] + i, basic_coord[1] + j, basic_coord[2] + k);
                    real wip = wi[i + 1].x() * wi[j + 1].y() * wi[k + 1].z();
                    particle_density += g.mass * wip;
                }
        particle_density = particle_density * grid_inv_space * grid_inv_space * grid_inv_space;
        // p.volume = particle_mass / particle_density;
    }
}

// Randomly generate particles in a cube
void MPM::addObject(Vector3 pos)
{
    for (int i = 0; i < 500; ++i) {
        particles.push_back(Particle(Vector3::Random() * 0.04 + pos));
    }
}

real MPM::calculateWi(Particle &p, Vector3i &base_coord)
{
    return 0;
    // Here just use cubic B-splines

}

G_NAMESPACE_END