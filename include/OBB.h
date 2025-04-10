#pragma once
#include <vector>
#include <array>
#include <limits>

struct Vector3
{
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vector3 operator+(const Vector3 &other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    Vector3 operator-(const Vector3 &other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    float dot(const Vector3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }
    Vector3 cross(const Vector3 &other) const
    {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }
    float length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }
    void normalize()
    {
        float len = length();
        if (len != 0)
        {
            x /= len;
            y /= len;
            z /= len;
        }
    }
};

using OBB = std::array<Vector3, 8>;

inline Vector3 computeMean(const std::vector<Vector3> &points)
{
    Vector3 mean;
    if (points.empty())
        return mean;
    for (const auto &p : points)
    {
        mean.x += p.x;
        mean.y += p.y;
        mean.z += p.z;
    }
    mean.x /= points.size();
    mean.y /= points.size();
    mean.z /= points.size();
    return mean;
}

struct Matrix3
{
    float data[3][3];
};

inline Matrix3 computeCovariance(const std::vector<Vector3> &points, const Vector3 &mean)
{
    Matrix3 cov = {0};
    if (points.empty())
        return cov;

    for (const auto &p : points)
    {
        Vector3 delta = p - mean;
        cov.data[0][0] += delta.x * delta.x;
        cov.data[0][1] += delta.x * delta.y;
        cov.data[0][2] += delta.x * delta.z;
        cov.data[1][1] += delta.y * delta.y;
        cov.data[1][2] += delta.y * delta.z;
        cov.data[2][2] += delta.z * delta.z;
    }
    // 填充对称部分
    cov.data[1][0] = cov.data[0][1];
    cov.data[2][0] = cov.data[0][2];
    cov.data[2][1] = cov.data[1][2];

    float invN = 1.0f / points.size();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            cov.data[i][j] *= invN;
    return cov;
}

inline Vector3 matrixMultiply(const Matrix3 &mat, const Vector3 &vec)
{
    return Vector3(
        mat.data[0][0] * vec.x + mat.data[0][1] * vec.y + mat.data[0][2] * vec.z,
        mat.data[1][0] * vec.x + mat.data[1][1] * vec.y + mat.data[1][2] * vec.z,
        mat.data[2][0] * vec.x + mat.data[2][1] * vec.y + mat.data[2][2] * vec.z);
}

inline Vector3 powerIteration(const Matrix3 &mat, int maxIter = 100)
{
    Vector3 b(1, 1, 1);
    b.normalize();
    for (int i = 0; i < maxIter; ++i)
    {
        Vector3 bn = matrixMultiply(mat, b);
        bn.normalize();
        if (bn.dot(b) > 0.9999f)
            break;
        b = bn;
    }
    return b;
}

inline Vector3 orthogonalIteration(const Matrix3 &mat, const Vector3 &u, int maxIter = 100)
{
    Vector3 v(0, 1, 0);
    v = v - u * v.dot(u); // 正交化
    v.normalize();

    for (int i = 0; i < maxIter; ++i)
    {
        Vector3 vn = matrixMultiply(mat, v);
        vn = vn - u * vn.dot(u); // 保持正交
        vn.normalize();
        if (vn.dot(v) > 0.9999f)
            break;
        v = vn;
    }
    return v;
}

inline OBB computeOBB(const std::vector<Vector3> &points)
{
    OBB corners;
    if (points.empty())
        return corners;

    // 计算均值中心和协方差矩阵
    Vector3 mean = computeMean(points);
    Matrix3 cov = computeCovariance(points, mean);

    // 计算主方向
    Vector3 u = powerIteration(cov);
    u.normalize();

    // 计算次方向（与u正交）
    Vector3 v = orthogonalIteration(cov, u);
    v.normalize();

    // 计算第三个方向
    Vector3 w = u.cross(v);
    w.normalize();

    // 计算投影极值
    struct
    {
        float min, max;
    } proj[3] = {
        {FLT_MAX, -FLT_MAX},
        {FLT_MAX, -FLT_MAX},
        {FLT_MAX, -FLT_MAX}};

    for (const auto &p : points)
    {
        Vector3 delta = p - mean;
        float pu = delta.dot(u), pv = delta.dot(v), pw = delta.dot(w);
        proj[0].min = std::min(proj[0].min, pu);
        proj[0].max = std::max(proj[0].max, pu);
        proj[1].min = std::min(proj[1].min, pv);
        proj[1].max = std::max(proj[1].max, pv);
        proj[2].min = std::min(proj[2].min, pw);
        proj[2].max = std::max(proj[2].max, pw);
    }

    // 计算OBB中心
    Vector3 center = mean +
                     u * (proj[0].min + proj[0].max) * 0.5f +
                     v * (proj[1].min + proj[1].max) * 0.5f +
                     w * (proj[2].min + proj[2].max) * 0.5f;

    // 计算半长
    Vector3 halfExtents(
        (proj[0].max - proj[0].min) * 0.5f,
        (proj[1].max - proj[1].min) * 0.5f,
        (proj[2].max - proj[2].min) * 0.5f);

    // 生成八个角点
    for (int i = 0; i < 8; ++i)
    {
        float sx = (i & 1) ? 1.0f : -1.0f;
        float sy = (i & 2) ? 1.0f : -1.0f;
        float sz = (i & 4) ? 1.0f : -1.0f;

        corners[i] = center +
                     u * (sx * halfExtents.x) +
                     v * (sy * halfExtents.y) +
                     w * (sz * halfExtents.z);
    }

    return corners;
}