#pragma once
#include <iostream>
#include <stdlib.h>

#include <common/math.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mpm/grid.h>
#include <mpm/particle.h>
#include <renderer/sphere.h>
#include <renderer/disk.h>

G_NAMESPACE_BEGIN

// A class mainly used as an interface.
// Use for collision.
class Rigidbody
{
public:
    Rigidbody(real sticky, real damping) : stickiness(sticky), damp(damping), m_l2w(Matrix4x4::Identity()), m_w2l(Matrix4x4::Identity()),
            boundary(0.001_f) {}

    virtual ~Rigidbody() {};

    bool virtual isPointCollided(const Vector3 &pos) const = 0;
    Vector3 virtual getPointVelocity(const Vector3 &pos) const = 0;
    Vector3 virtual getPointNormal(const Vector3 &pos) const = 0;

    // standard interaction.
    void virtual interactWithGrid(const Vector3 &pos, GridUnit &g, real dt) const {
        Vector3 localPos = toLocalPos(pos);

        if (!isPointCollided(localPos))
            return;             // don't add any collision.

        // else, update grid speed.
        Vector3 normal = getPointNormal(localPos);
        Vector3 pointVel = getPointVelocity(localPos);
        Vector3 localSpeed = toLocalDir(g.velocity) - pointVel;
        real speedAlongNormal = localSpeed.dot(normal);
        localSpeed = eliminateOneAxis(localSpeed, normal);

        // add friction
        real friction = damp * dt;
        real length = localSpeed.norm();
        if (length < friction) {
            localSpeed.setZero();
        }
        else {
            localSpeed *= (length - friction) / length;
        }

        // add bounce and stickiness
        if (speedAlongNormal > 0) {
            // allow velocity along normal.
            localSpeed += normal * speedAlongNormal;
        }
        
        // stickness, lerp to make velocity same with the point velocity
        real stickFactor = clamp(stickiness, 0_f, 1_f);
        localSpeed *= (1.0_f - stickFactor);

        /*
        // debug: check if eliminated
        if (pos(0) > 0.5_f && g.velocity(0) > 0_f && localSpeed(0) <= 0_f)
        {
            std::cout << "Grid positive x velocity is eliminated!!!\n" << pos.transpose() << std::endl;
            std::cout << "normal:" << normal.transpose() << std::endl;
            std::cout << "origin speed:" << g.velocity.transpose() << std::endl;
            Vector3 tmp = toLocalDir(g.velocity) - getPointVelocity(pos);
            std::cout << "convert to local:" << tmp.transpose() << std::endl;
            tmp = eliminateOneAxis(tmp, normal);
            std::cout << "after eliminated:" << tmp.transpose() << std::endl;
            std::cout << "current speed:" << localSpeed.transpose() << std::endl;
            std::cout << "Finally got:" << toWorldDir(localSpeed) << std::endl;
        }*/

        g.velocity = toWorldDir(localSpeed + pointVel);
    };

    // standard interaction.
    void virtual interactWithParticle(Particle &p, real dt) const {
        Vector3 localPos = toLocalPos(p.pos);

        if (!isPointCollided(localPos))
            return;             // don't add any collision.

        // else, update grid speed.
        Vector3 normal = getPointNormal(localPos);
        Vector3 pointVel = getPointVelocity(localPos);
        Vector3 localSpeed = toLocalDir(p.vel) - pointVel;
        real speedAlongNormal = localSpeed.dot(normal);
        localSpeed = eliminateOneAxis(localSpeed, normal);

        // add friction
        real friction = damp * dt;
        real length = localSpeed.norm();
        if (length < friction) {
            localSpeed.setZero();
        }
        else {
            localSpeed *= (length - friction) / length;
        }

        // add bounce and stickiness
        if (speedAlongNormal > 0) {
            // allow velocity along normal.
            localSpeed += normal * speedAlongNormal;
        }

        // stickness, lerp to make velocity same with the point velocity
        real stickFactor = clamp(stickiness, 0_f, 1_f);
        localSpeed *= (1.0_f - stickFactor);

        p.vel = toWorldDir(localSpeed + pointVel);
    };

    void virtual update(real dt) {};                // update rigidbody status
    void virtual draw() {};                         // draw object.

    glm::mat4 virtual getGLToWorldMatrix() {
        return convertToGLMMat4(m_l2w);
    }

    // set position
    void virtual setLocalToWorld(const Matrix4x4 &m) {
        m_l2w = m;
        m_w2l = m_l2w.inverse();
    }

    void virtual setLocalToWorld(const glm::mat4 &m) {
        setLocalToWorld(convertToEigen(m));
    }

protected:
    real stickiness;                        // stickiness
    real damp;                              // damp factor for friction.
    Matrix4x4 m_l2w;                        // local to world
    Matrix4x4 m_w2l;                        // world to local
    real boundary;                          // add a little boundary

    virtual Vector3 toLocalPos(const Vector3 &pos) const {
        Vector4 p = m_w2l * vec4(pos, 1_f);
        return vec3FromVec4(p);
    }

    virtual Vector3 toLocalDir(const Vector3 &pos) const {
        Vector4 p = m_w2l * vec4(pos, 0_f);
        return vec3FromVec4(p);
    }

    virtual Vector3 toWorldPos(const Vector3 &pos) const {
        Vector4 p = m_l2w * vec4(pos, 1_f);
        return vec3FromVec4(p);
    }

    virtual Vector3 toWorldDir(const Vector3 &pos) const {
        Vector4 p = m_l2w * vec4(pos, 0_f);
        return vec3FromVec4(p);
    }
};

// Sphere
class RigidSphere : public Rigidbody
{
public:
    RigidSphere(real sticky = 0_f, real damping = 1_f) : Rigidbody(sticky, damping) {}

    bool virtual isPointCollided(const Vector3 &pos) const override;
    Vector3 virtual getPointVelocity(const Vector3 &pos) const override;
    Vector3 virtual getPointNormal(const Vector3 &pos) const override;
    void virtual draw() override;
    // in order to make sure alignment

    virtual ~RigidSphere() {};

    // Not willing to add this. But not sure why C4316 warning is triggered.
    // In order to make sure allocate 32-bit aligned memory. I had to do this.
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 32);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }


private:
    SphereShape m_sphereshape;
};

// Disk
class RotatingDisk : public Rigidbody
{
public:
    RotatingDisk(real rotateSpeed = 0_f, real liftFrequency = 0_f,
        real liftDistance = 0_f, real sticky = 0_f, real damping = 1_f
    ) 
        : m_rotationSpeed(rotateSpeed), m_liftFrequency(liftFrequency), m_moveFactor(1_f),
        m_localMovement(Matrix4x4::Identity()), Rigidbody(sticky, damping) 
    {
        m_inverseMovement = m_localMovement.inverse();
        setLiftingDistance(liftDistance);
    }

    bool virtual isPointCollided(const Vector3 &pos) const override;
    Vector3 virtual getPointVelocity(const Vector3 &pos) const override;
    Vector3 virtual getPointNormal(const Vector3 &pos) const override;
    void virtual draw() override;
    void virtual update(real dt) override;
    // in order to make sure alignment

    virtual ~RotatingDisk() {};

    // Not willing to add this. But not sure why C4316 warning is triggered.
    // In order to make sure allocate 32-bit aligned memory. I had to do this.
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 32);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

    glm::mat4 virtual getGLToWorldMatrix() override;

    void setRotationSpeed(real spd);
    void setLiftingFrequency(real f);
    void setLiftingDistance(real d);

private:
    real m_rotationSpeed;
    real m_liftFrequency;
    real m_liftDistance;
    real m_moveFactor;

    Matrix4x4 m_localMovement;
    Matrix4x4 m_inverseMovement;

    virtual Vector3 toLocalPos(const Vector3 &pos) const override;
    virtual Vector3 toLocalDir(const Vector3 &pos) const override;
    virtual Vector3 toWorldPos(const Vector3 &pos) const override;
    virtual Vector3 toWorldDir(const Vector3 &pos) const override;

    // set position
    void virtual setLocalToWorld(const Matrix4x4 &m) override;

    DiskShape m_disk;
};

G_NAMESPACE_END