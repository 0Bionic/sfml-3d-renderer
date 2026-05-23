#pragma once

struct Vector3
{
    float x{};
    float y{};
    float z{};
};

struct Matrix4x4
{
    float m[4][4]{};
};

// TODO: Implement math.
Vector3 add(const Vector3& lhs, const Vector3& rhs);

// TODO: Implement math.
Vector3 subtract(const Vector3& lhs, const Vector3& rhs);

// TODO: Implement math.
Vector3 multiply(const Matrix4x4& matrix, const Vector3& vector);

// TODO: Implement math.
Matrix4x4 multiply(const Matrix4x4& lhs, const Matrix4x4& rhs);

// TODO: Implement math.
Matrix4x4 makeIdentityMatrix();

// TODO: Implement math.
Matrix4x4 makeRotationXMatrix(float radians);

// TODO: Implement math.
Matrix4x4 makeRotationYMatrix(float radians);

// TODO: Implement math.
Matrix4x4 makeRotationZMatrix(float radians);

// TODO: Implement math.
Matrix4x4 makeTranslationMatrix(float x, float y, float z);

// TODO: Implement math.
Matrix4x4 makeProjectionMatrix(float fieldOfViewDegrees, float aspectRatio, float nearPlane, float farPlane);
