#pragma once

#include "util.h"
#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

G_NAMESPACE_BEGIN

typedef Eigen::Matrix<real, 3, 1> Vector3;
typedef Eigen::Matrix<real, 4, 1> Vector4;
typedef Eigen::Matrix<float, 3, 1> Vector3f;
typedef Eigen::Matrix<double, 3, 1> Vector3d;
typedef Eigen::Matrix<uint, 3, 1> Vector3u;
typedef Eigen::Matrix<int, 3, 1> Vector3i;

typedef Eigen::Matrix<real, 3, 3> Matrix3x3;
typedef Eigen::Matrix<real, 4, 4> Matrix4x4;

FORCE_INLINE Vector3 sqr(const Vector3 &v)
{
    return v.cwiseProduct(v);
}

FORCE_INLINE Vector4 vec4(const Vector3 &v, const real w)
{
    return Vector4(v(0), v(1), v(2), w);
}

FORCE_INLINE Vector3 vec3FromVec4(const Vector4 &v)
{
    Vector3 ret(v(0), v(1), v(2));
    return ret;
}

FORCE_INLINE Vector3 eliminateOneAxis(const Vector3 &pos, const Vector3 &axis)
{
    Vector3 norm = axis.normalized();
    float length = static_cast<float>(pos.dot(norm));
    return pos - (norm * length);
}

FORCE_INLINE void getSqrAndCub(const Vector3 &v, Vector3 &sqr, Vector3 &cub)
{
    for (int i = 0; i < 3; ++i) {
        sqr[i] = v[i] * v[i];
        cub[i] = sqr[i] * v[i];
    }
}

FORCE_INLINE void polarDecomposite(const Matrix3x3 &matrix, Matrix3x3 &r, Matrix3x3 &s)
{
    Eigen::JacobiSVD<Matrix3x3> svd(matrix, Eigen::ComputeFullU | Eigen::ComputeFullV);
    // get R and S
    Vector3 singularValues = svd.singularValues();
    Matrix3x3 u = svd.matrixU();
    Matrix3x3 v = svd.matrixV();
    Matrix3x3 sig = Matrix3x3::Zero();
    for (int i = 0; i < 3; ++i)
        sig(i, i) = singularValues[i];

    r = u * v.transpose();
    s = v * sig * v.transpose();
}

FORCE_INLINE real clamp(real value, const real min, const real max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

FORCE_INLINE glm::mat4 convertToGLMMat4(const Matrix4x4 &mat)
{
    const real *data = mat.data();
    glm::mat4 ret;
    float *dest = glm::value_ptr(ret);
    for (int i = 0; i < 16; ++i)
        dest[i] = static_cast<float>(data[i]);
    return ret;
}

FORCE_INLINE Matrix4x4 convertToEigen(const glm::mat4 &mat)
{
    const float *data = glm::value_ptr(mat);
    Matrix4x4 ret;
    real *dest = ret.data();
    for (int i = 0; i < 16; ++i)
        dest[i] = static_cast<real>(data[i]);
    return ret;
}

FORCE_INLINE Matrix4x4 TSMatrix(Vector3 t, Vector3 scale)
{
    Matrix4x4 target(Matrix4x4::Zero());
    target(0, 0) = scale(0);
    target(1, 1) = scale(1);
    target(2, 2) = scale(2);
    target.col(3) = Vector4(t(0), t(1), t(2), 1_f);
    return target;
}

G_NAMESPACE_END