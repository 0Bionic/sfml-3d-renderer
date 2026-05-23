#include "Math.hpp"

Vector3 add(const Vector3& lhs, const Vector3& rhs)
{
    (void)lhs;
    (void)rhs;
    // TODO: Implement math.
    return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Vector3 subtract(const Vector3& lhs, const Vector3& rhs)
{
    (void)lhs;
    (void)rhs;
    // TODO: Implement math.
    return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector3 multiply(const Matrix4x4& matrix, const Vector3& vector)
{
    (void)matrix;
    (void)vector;
    // TODO: Implement math.
    // Rather than using a loop based implementation, we implement multiplication manually to make the process much more efficient.
    // This is because we know that the matrix is a 4x4 matrix and the vector is a 3D vector, so we can use the following formula:
    //
    // | a11 a12 a13 a14 | | x |   | a11*x + a12*y + a13*z + a14 |
    // | a21 a22 a23 a24 | | y | = | a21*x + a22*y + a23*z + a24 |
    // | a31 a32 a33 a34 | | z |   | a31*x + a32*y + a33*z + a34 |
    // | a41 a42 a43 a44 | | 1 |   | a41*x + a42*y + a43*z + a44 |
    //
    // So we can just multiply the matrix by the vector manually.
    return Vector3{matrix.m[0][0] * vector.x + matrix.m[0][1] * vector.y + matrix.m[0][2] * vector.z + matrix.m[0][3], matrix.m[1][0] * vector.x + matrix.m[1][1] * vector.y + matrix.m[1][2] * vector.z + matrix.m[1][3], matrix.m[2][0] * vector.x + matrix.m[2][1] * vector.y + matrix.m[2][2] * vector.z + matrix.m[2][3]};
}

Matrix4x4 multiply(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
    Matrix4x4 result;
    
    // Row 0
    result.m[0][0] = (lhs.m[0][0] * rhs.m[0][0]) + (lhs.m[0][1] * rhs.m[1][0]) + (lhs.m[0][2] * rhs.m[2][0]) + (lhs.m[0][3] * rhs.m[3][0]);
    result.m[0][1] = (lhs.m[0][0] * rhs.m[0][1]) + (lhs.m[0][1] * rhs.m[1][1]) + (lhs.m[0][2] * rhs.m[2][1]) + (lhs.m[0][3] * rhs.m[3][1]);
    result.m[0][2] = (lhs.m[0][0] * rhs.m[0][2]) + (lhs.m[0][1] * rhs.m[1][2]) + (lhs.m[0][2] * rhs.m[2][2]) + (lhs.m[0][3] * rhs.m[3][2]);
    result.m[0][3] = (lhs.m[0][0] * rhs.m[0][3]) + (lhs.m[0][1] * rhs.m[1][3]) + (lhs.m[0][2] * rhs.m[2][3]) + (lhs.m[0][3] * rhs.m[3][3]);

    // Row 1
    result.m[1][0] = (lhs.m[1][0] * rhs.m[0][0]) + (lhs.m[1][1] * rhs.m[1][0]) + (lhs.m[1][2] * rhs.m[2][0]) + (lhs.m[1][3] * rhs.m[3][0]);
    result.m[1][1] = (lhs.m[1][0] * rhs.m[0][1]) + (lhs.m[1][1] * rhs.m[1][1]) + (lhs.m[1][2] * rhs.m[2][1]) + (lhs.m[1][3] * rhs.m[3][1]);
    result.m[1][2] = (lhs.m[1][0] * rhs.m[0][2]) + (lhs.m[1][1] * rhs.m[1][2]) + (lhs.m[1][2] * rhs.m[2][2]) + (lhs.m[1][3] * rhs.m[3][2]);
    result.m[1][3] = (lhs.m[1][0] * rhs.m[0][3]) + (lhs.m[1][1] * rhs.m[1][3]) + (lhs.m[1][2] * rhs.m[2][3]) + (lhs.m[1][3] * rhs.m[3][3]);

    // Row 2
    result.m[2][0] = (lhs.m[2][0] * rhs.m[0][0]) + (lhs.m[2][1] * rhs.m[1][0]) + (lhs.m[2][2] * rhs.m[2][0]) + (lhs.m[2][3] * rhs.m[3][0]);
    result.m[2][1] = (lhs.m[2][0] * rhs.m[0][1]) + (lhs.m[2][1] * rhs.m[1][1]) + (lhs.m[2][2] * rhs.m[2][1]) + (lhs.m[2][3] * rhs.m[3][1]);
    result.m[2][2] = (lhs.m[2][0] * rhs.m[0][2]) + (lhs.m[2][1] * rhs.m[1][2]) + (lhs.m[2][2] * rhs.m[2][2]) + (lhs.m[2][3] * rhs.m[3][2]);
    result.m[2][3] = (lhs.m[2][0] * rhs.m[0][3]) + (lhs.m[2][1] * rhs.m[1][3]) + (lhs.m[2][2] * rhs.m[2][3]) + (lhs.m[2][3] * rhs.m[3][3]);

    // Row 3
    result.m[3][0] = (lhs.m[3][0] * rhs.m[0][0]) + (lhs.m[3][1] * rhs.m[1][0]) + (lhs.m[3][2] * rhs.m[2][0]) + (lhs.m[3][3] * rhs.m[3][0]);
    result.m[3][1] = (lhs.m[3][0] * rhs.m[0][1]) + (lhs.m[3][1] * rhs.m[1][1]) + (lhs.m[3][2] * rhs.m[2][1]) + (lhs.m[3][3] * rhs.m[3][1]);
    result.m[3][2] = (lhs.m[3][0] * rhs.m[0][2]) + (lhs.m[3][1] * rhs.m[1][2]) + (lhs.m[3][2] * rhs.m[2][2]) + (lhs.m[3][3] * rhs.m[3][2]);
    result.m[3][3] = (lhs.m[3][0] * rhs.m[0][3]) + (lhs.m[3][1] * rhs.m[1][3]) + (lhs.m[3][2] * rhs.m[2][3]) + (lhs.m[3][3] * rhs.m[3][3]);

    return result;
}

Matrix4x4 makeIdentityMatrix()
{
    // TODO: Implement math.
    Matrix4x4 result;
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 makeRotationXMatrix(float radians)
{
    (void)radians;
    // TODO: Implement math.
}

Matrix4x4 makeRotationYMatrix(float radians)
{
    (void)radians;
    // TODO: Implement math.
}

Matrix4x4 makeRotationZMatrix(float radians)
{
    (void)radians;
    // TODO: Implement math.
}

Matrix4x4 makeTranslationMatrix(float x, float y, float z)
{
    (void)x;
    (void)y;
    (void)z;
    // TODO: Implement math.
}

Matrix4x4 makeProjectionMatrix(float fieldOfViewDegrees, float aspectRatio, float nearPlane, float farPlane)
{
    (void)fieldOfViewDegrees;
    (void)aspectRatio;
    (void)nearPlane;
    (void)farPlane;
    // TODO: Implement math.
}
