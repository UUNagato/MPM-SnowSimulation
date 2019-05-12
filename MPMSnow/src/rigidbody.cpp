#include <renderer/sphere.h>
#include <mpm/rigidbody.h>

G_NAMESPACE_BEGIN

#pragma region Sphere
bool RigidSphere::isPointCollided(const Vector3 &pos) const {
    real r2 = pos.cwiseProduct(pos).sum();
    real t2 = (1_f + boundary) * (1_f + boundary);
    return r2 <= t2;
}

Vector3 RigidSphere::getPointVelocity(const Vector3 &pos) const {
    return Vector3::Zero();
};

Vector3 RigidSphere::getPointNormal(const Vector3 &pos) const {
    return pos.normalized();
}

void RigidSphere::draw() {
    m_sphereshape.draw();
}
#pragma endregion

#pragma region Disk

void RotatingDisk::setRotationSpeed(real rts)
{
    m_rotationSpeed = rts;
}

void RotatingDisk::setLiftingDistance(real dis)
{
    if (dis < 0_f)
        dis = 0_f;
    // should convert to local distance
    Vector3 up(0_f, 1_f, 0_f);
    Vector3 world = toWorldDir(up);
    world.normalize();
    world *= dis;
    m_liftDistance = toLocalDir(world)(1);
}

void RotatingDisk::setLiftingFrequency(real f)
{
    if (f < 0_f)
        f = 0_f;
    m_liftFrequency = f;
}

void RotatingDisk::update(real dt)
{
    // since the disk only moves along Y axis.
    real curHeight = m_localMovement(1, 3);

    // check distance
    curHeight = clamp(curHeight, 0_f, m_liftDistance);

    // get speed
    real moveSpeed = 0_f;
    if (m_liftFrequency != 0_f)
        moveSpeed = m_liftDistance * m_liftFrequency;
    if (m_moveFactor == 0_f)
        m_moveFactor = 1_f;
    curHeight += m_moveFactor * moveSpeed * dt;

    // switch
    if (curHeight >= m_liftDistance) {
        curHeight = m_liftDistance;
        m_moveFactor = -1_f;
    }
    else if (curHeight <= 0_f) {
        curHeight = 0_f;
        m_moveFactor = 1_f;
    }

    // update localpos
    m_localMovement(1, 3) = curHeight;
    m_inverseMovement = m_localMovement.inverse();
}

Vector3 RotatingDisk::toLocalPos(const Vector3 &pos) const
{
    Vector4 p = m_inverseMovement * m_w2l * vec4(pos, 1_f);
    return vec3FromVec4(p);
}

Vector3 RotatingDisk::toLocalDir(const Vector3 &pos) const
{
    Vector4 p = m_inverseMovement * m_w2l * vec4(pos, 0_f);
    return vec3FromVec4(p);
}

Vector3 RotatingDisk::toWorldPos(const Vector3 &pos) const
{
    Vector4 p = m_l2w * m_localMovement * vec4(pos, 1_f);
    return vec3FromVec4(p);
}

Vector3 RotatingDisk::toWorldDir(const Vector3 &pos) const
{
    Vector4 p = m_l2w * m_localMovement * vec4(pos, 0_f);
    return vec3FromVec4(p);
}

Vector3 RotatingDisk::getPointNormal(const Vector3 &pos) const
{
    if (pos(1) >= 0.5_f)
        return Vector3(0_f, 1_f, 0_f);
    return Vector3(0_f, -1_f, 0_f);
}

Vector3 RotatingDisk::getPointVelocity(const Vector3 &pos) const
{
    // first from lifting speed
    real liftSpeed = 0_f;
    if (m_liftFrequency != 0_f)
        liftSpeed = m_liftDistance * m_liftFrequency;
    Vector3 base(0_f, m_moveFactor * liftSpeed, 0_f);

    // add rotation speed
    real linearSpeed = m_rotationSpeed * std::sqrt(pos(0) * pos(0) + pos(2) * pos(2));
    // calculate rotate direction
    Vector3 angular(-pos(2), 0_f, pos(0));
    if (angular != Vector3::Zero())
        angular.normalize();
    
    return base + angular * linearSpeed;
}

bool RotatingDisk::isPointCollided(const Vector3 &pos) const
{
    // first calculate radius
    real sqrr = pos(0) * pos(0) + pos(2) * pos(2);
    if (sqrr > 1_f)
        return false;

    // if it's in the circle, then check y axis

    real upper = 1_f + boundary;
    real lower = -boundary;

    return pos(1) >= lower && pos(1) <= upper;
}

void RotatingDisk::draw()
{
    m_disk.draw();
}

glm::mat4 RotatingDisk::getGLToWorldMatrix()
{
    return convertToGLMMat4(m_l2w * m_localMovement);
}

void RotatingDisk::setLocalToWorld(const Matrix4x4 &m)
{
    Vector3 worldLift = toWorldDir(Vector3(0_f, m_liftDistance, 0_f));
    real finalDis = worldLift.norm();
    m_l2w = m;
    m_w2l = m_l2w.inverse();
    setLiftingDistance(finalDis);
}

#pragma endregion

G_NAMESPACE_END