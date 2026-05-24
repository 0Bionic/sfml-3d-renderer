#include <cmath>
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
    return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Vector3 multiply(const Matrix4x4& matrix, const Vector3& vector)
{
    Vector3 result;
    
    // Calculate the raw x', y', and z'
    result.x = matrix.m[0][0] * vector.x + matrix.m[0][1] * vector.y + matrix.m[0][2] * vector.z + matrix.m[0][3];
    result.y = matrix.m[1][0] * vector.x + matrix.m[1][1] * vector.y + matrix.m[1][2] * vector.z + matrix.m[1][3];
    result.z = matrix.m[2][0] * vector.x + matrix.m[2][1] * vector.y + matrix.m[2][2] * vector.z + matrix.m[2][3];

    // Calculate the new 4th dimension (w)
    float w = matrix.m[3][0] * vector.x + matrix.m[3][1] * vector.y + matrix.m[3][2] * vector.z + matrix.m[3][3];

    // Perform the perspective divide (if w is not zero to prevent dividing by zero crashes)
    if (w != 0.0f) 
    {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }

    return result;
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
    Matrix4x4 result;

    float c = std::cos(radians);
    float s = std::sin(radians);

    // Main Diagonal 
    result.m[0][0] = 1.0f;
    result.m[3][3] = 1.0f;

    // Row 1
    result.m[1][1] = c;
    result.m[1][2] = -s;

    // Row 2
    result.m[2][1] = s;
    result.m[2][2] = c;

    return result;
}

Matrix4x4 makeRotationYMatrix(float radians)
{
    Matrix4x4 result;
    
    float c = std::cos(radians);
    float s = std::sin(radians);

    // Row 0
    result.m[0][0] = c;
    result.m[0][2] = s;
    
    // Row 1
    result.m[1][1] = 1.0f;
    
    // Row 2
    result.m[2][0] = -s;
    result.m[2][2] = c;
    
    // Row 3 (w component remains 1)
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 makeRotationZMatrix(float radians)
{
    Matrix4x4 result;
    
    float c = std::cos(radians);
    float s = std::sin(radians);

    // Row 0
    result.m[0][0] = c;
    result.m[0][1] = -s;
    
    // Row 1
    result.m[1][0] = s;
    result.m[1][1] = c;
    
    // Main diagonal
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    return result;
}

Matrix4x4 makeTranslationMatrix(float x, float y, float z)
{
    Matrix4x4 result;
    
    // Set the main diagonal (Identity) so the object doesn't scale to 0
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;

    // Plug the translation values into the 4th column
    result.m[0][3] = x;
    result.m[1][3] = y;
    result.m[2][3] = z;

    return result;
}


Matrix4x4 makeProjectionMatrix(float fieldOfViewDegrees, float aspectRatio, float nearPlane, float farPlane)
{
    Matrix4x4 result; // Initializes all elements to 0
    
    // Convert degrees to radians
    float fovRadians = fieldOfViewDegrees * (3.14159f / 180.0f);
    
    // Calculate the focal length
    float f = 1.0f / std::tan(fovRadians / 2.0f);
    
    // Calculate the Z-depth scaling formula
    float zRange = farPlane - nearPlane;

    // Row 0
    result.m[0][0] = f / aspectRatio;
    
    // Row 1
    result.m[1][1] = f;
    
    // Row 2 (Normalizes the Z coordinate between 0 and 1)
    result.m[2][2] = farPlane / zRange;
    result.m[2][3] = -(farPlane * nearPlane) / zRange;
    
    // Row 3 (Copies Z into W for the perspective divide!)
    result.m[3][2] = 1.0f;
    result.m[3][3] = 0.0f; // Explicitly 0, replacing the Identity matrix's 1

    return result;
}