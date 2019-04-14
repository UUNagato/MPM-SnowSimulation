#pragma once

#include "util.h"
#include <Eigen/Dense>

G_NAMESPACE_BEGIN

typedef Eigen::Matrix<real, 3, 1> Vector3;
typedef Eigen::Matrix<uint, 3, 1> Vector3u;
typedef Eigen::Matrix<int, 3, 1> Vector3i;

typedef Eigen::Matrix<real, 3, 3> Matrix3x3;

FORCE_INLINE Vector3 sqr(const Vector3 &v)
{
    return Vector3(v[0] * v[0], v[1] * v[1], v[2] * v[2]);
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

G_NAMESPACE_END