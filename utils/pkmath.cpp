#include "pkmath.h"

#define _USE_MATH_DEFINES
#include <cmath>


namespace pk
{
    std::string vec2::toString() const
    {
        std::string out = "( " + std::to_string(x) + ", " + std::to_string(y) + " )";
        return out;
    }

    float vec2::length() const
    {
        return sqrtf((x * x) + (y * y));
    }

    float vec2::dotp(const vec2& other) const
    {
        return ((x * other.x) + (y * other.y));
    }

    vec2 vec2::normalize() const
    {
        const float len = length();
        return { x / len, y / len };
    }

    vec2 operator+(const vec2& left, const vec2& right)
    {
        return { left.x + right.x, left.y + right.y };
    }

    vec2 operator-(const vec2& left, const vec2& right)
    {
        return { left.x - right.x, left.y - right.y };
    }

    vec2 operator*(const vec2& left, const vec2& right)
    {
        return { left.x * right.x, left.y * right.y };
    }

    vec2 operator*(const vec2& left, float right)
    {
        return { left.x * right, left.y * right };
    }

    vec2 operator/(const vec2& left, float right)
    {
        return { left.x / right, left.y / right };
    }


    std::string vec3::toString() const
    {
        std::string out = "( " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " )";
        return out;
    }

    float vec3::length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z));
    }

    float vec3::dotp(const vec3& other) const
    {
        return ((x * other.x) + (y * other.y) + (z * other.z));
    }

    vec3 vec3::normalize() const
    {
        const float len = length();
        return { x / len, y / len, z / len };
    }

    vec3 vec3::lerp(const vec3& other, float amount) const
    {
        const vec3& v = *this;
        return v + ((other - v) * amount);
    }

    vec3 operator+(const vec3& left, const vec3& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z };
    }

    vec3 operator-(const vec3& left, const vec3& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z };
    }

    vec3 operator*(const vec3& left, const vec3& right)
    {
        return { left.x * right.x, left.y * right.y, left.z * right.z };
    }

    vec3 operator*(const vec3& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right };
    }

    vec3 operator/(const vec3& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right };
    }


    std::string vec4::toString() const
    {
        std::string out = "( " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + " )";
        return out;
    }

    float vec4::length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
    }

    float vec4::dotp(const vec4& other) const
    {
        return ((x * other.x) + (y * other.y) + (z * other.z) + (w * other.w));
    }

    vec4 vec4::normalize() const
    {
        const float len = length();
        return {
            x / len,
              y / len,
              z / len,
              w / len
        };
    }

    vec4 operator+(const vec4& left, const vec4& right)
    {
        return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
    }

    vec4 operator-(const vec4& left, const vec4& right)
    {
        return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
    }

    vec4 operator*(const vec4& left, const vec4& right)
    {
        return { left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w };
    }

    vec4 operator*(const vec4& left, float right)
    {
        return { left.x * right, left.y * right, left.z * right, left.w * right };
    }

    vec4 operator/(const vec4& left, float right)
    {
        return { left.x / right, left.y / right, left.z / right, left.w / right };
    }


    mat4::mat4(float diag)
    {
        memset(_e, 0, sizeof(float) * 16);
        //for (int i = 0; i < 4; ++i)
        //    _e[i * i + 4] = diag;


        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (i == j)
                    _e[i + j * 4] = diag;
            }
        }

    }

    std::string mat4::toString() const
    {
        std::string out = "";
        for (int i = 0; i < 4; ++i)
        {
            out += "[ ";
            for (int j = 0; j < 4; ++j)
            {
                out += std::to_string(_e[i + j * 4]);
                if (j < 3)
                    out += ", ";
            }
            out += " ]\n";
        }
        return out;
    }

    void mat4::operator=(const mat4& other)
    {
        memcpy(_e, other._e, sizeof(float) * 16);
    }

    bool mat4::operator==(const mat4& other) const
    {
        return memcmp(_e, other._e, sizeof(float) * 16) == 0;
    }

    bool mat4::operator!=(const mat4& other) const
    {
        return memcmp(_e, other._e, sizeof(float) * 16) != 0;
    }

    void mat4::setIdentity()
    {
        memset(_e, 0, sizeof(float) * 16);
        for (int i = 0; i < 4; ++i)
            _e[i + i * 4] = 1.0f;
    }

    mat4 mat4::transpose() const
    {
        mat4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result[i + j * 4] = _e[j + i * 4];
            }
        }
        return result;
    }

    //	*Found from: https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    //		comment on the site about this :
    //			"This was lifted from MESA implementation of the GLU library."
    void mat4::inverse()
    {
        mat4 inverseMatrix;

        inverseMatrix._e[0] = _e[5] * _e[10] * _e[15] -
            _e[5] * _e[11] * _e[14] -
            _e[9] * _e[6] * _e[15] +
            _e[9] * _e[7] * _e[14] +
            _e[13] * _e[6] * _e[11] -
            _e[13] * _e[7] * _e[10];

        inverseMatrix._e[4] = -_e[4] * _e[10] * _e[15] +
            _e[4] * _e[11] * _e[14] +
            _e[8] * _e[6] * _e[15] -
            _e[8] * _e[7] * _e[14] -
            _e[12] * _e[6] * _e[11] +
            _e[12] * _e[7] * _e[10];

        inverseMatrix._e[8] = _e[4] * _e[9] * _e[15] -
            _e[4] * _e[11] * _e[13] -
            _e[8] * _e[5] * _e[15] +
            _e[8] * _e[7] * _e[13] +
            _e[12] * _e[5] * _e[11] -
            _e[12] * _e[7] * _e[9];

        inverseMatrix._e[12] = -_e[4] * _e[9] * _e[14] +
            _e[4] * _e[10] * _e[13] +
            _e[8] * _e[5] * _e[14] -
            _e[8] * _e[6] * _e[13] -
            _e[12] * _e[5] * _e[10] +
            _e[12] * _e[6] * _e[9];

        inverseMatrix._e[1] = -_e[1] * _e[10] * _e[15] +
            _e[1] * _e[11] * _e[14] +
            _e[9] * _e[2] * _e[15] -
            _e[9] * _e[3] * _e[14] -
            _e[13] * _e[2] * _e[11] +
            _e[13] * _e[3] * _e[10];

        inverseMatrix._e[5] = _e[0] * _e[10] * _e[15] -
            _e[0] * _e[11] * _e[14] -
            _e[8] * _e[2] * _e[15] +
            _e[8] * _e[3] * _e[14] +
            _e[12] * _e[2] * _e[11] -
            _e[12] * _e[3] * _e[10];

        inverseMatrix._e[9] = -_e[0] * _e[9] * _e[15] +
            _e[0] * _e[11] * _e[13] +
            _e[8] * _e[1] * _e[15] -
            _e[8] * _e[3] * _e[13] -
            _e[12] * _e[1] * _e[11] +
            _e[12] * _e[3] * _e[9];

        inverseMatrix._e[13] = _e[0] * _e[9] * _e[14] -
            _e[0] * _e[10] * _e[13] -
            _e[8] * _e[1] * _e[14] +
            _e[8] * _e[2] * _e[13] +
            _e[12] * _e[1] * _e[10] -
            _e[12] * _e[2] * _e[9];

        inverseMatrix._e[2] = _e[1] * _e[6] * _e[15] -
            _e[1] * _e[7] * _e[14] -
            _e[5] * _e[2] * _e[15] +
            _e[5] * _e[3] * _e[14] +
            _e[13] * _e[2] * _e[7] -
            _e[13] * _e[3] * _e[6];

        inverseMatrix._e[6] = -_e[0] * _e[6] * _e[15] +
            _e[0] * _e[7] * _e[14] +
            _e[4] * _e[2] * _e[15] -
            _e[4] * _e[3] * _e[14] -
            _e[12] * _e[2] * _e[7] +
            _e[12] * _e[3] * _e[6];

        inverseMatrix._e[10] = _e[0] * _e[5] * _e[15] -
            _e[0] * _e[7] * _e[13] -
            _e[4] * _e[1] * _e[15] +
            _e[4] * _e[3] * _e[13] +
            _e[12] * _e[1] * _e[7] -
            _e[12] * _e[3] * _e[5];

        inverseMatrix._e[14] = -_e[0] * _e[5] * _e[14] +
            _e[0] * _e[6] * _e[13] +
            _e[4] * _e[1] * _e[14] -
            _e[4] * _e[2] * _e[13] -
            _e[12] * _e[1] * _e[6] +
            _e[12] * _e[2] * _e[5];

        inverseMatrix._e[3] = -_e[1] * _e[6] * _e[11] +
            _e[1] * _e[7] * _e[10] +
            _e[5] * _e[2] * _e[11] -
            _e[5] * _e[3] * _e[10] -
            _e[9] * _e[2] * _e[7] +
            _e[9] * _e[3] * _e[6];

        inverseMatrix._e[7] = _e[0] * _e[6] * _e[11] -
            _e[0] * _e[7] * _e[10] -
            _e[4] * _e[2] * _e[11] +
            _e[4] * _e[3] * _e[10] +
            _e[8] * _e[2] * _e[7] -
            _e[8] * _e[3] * _e[6];

        inverseMatrix._e[11] = -_e[0] * _e[5] * _e[11] +
            _e[0] * _e[7] * _e[9] +
            _e[4] * _e[1] * _e[11] -
            _e[4] * _e[3] * _e[9] -
            _e[8] * _e[1] * _e[7] +
            _e[8] * _e[3] * _e[5];

        inverseMatrix._e[15] = _e[0] * _e[5] * _e[10] -
            _e[0] * _e[6] * _e[9] -
            _e[4] * _e[1] * _e[10] +
            _e[4] * _e[2] * _e[9] +
            _e[8] * _e[1] * _e[6] -
            _e[8] * _e[2] * _e[5];


        float determinant = _e[0] * inverseMatrix._e[0] + _e[1] * inverseMatrix._e[4] + _e[2] * inverseMatrix._e[8] + _e[3] * inverseMatrix._e[12];

        *this = inverseMatrix; // THIS MAY NOT WORK! <- wtf is going on here!?!? don't remember at all..

        if (determinant == 0)
            return;

        for (int i = 0; i < 16; ++i)
            _e[i] *= (1.0f / determinant);
    }

    mat4 operator*(const mat4& left, const mat4& right)
    {
        mat4 result;

        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
                result[y + x * 4] =
                    left[y + 0 * 4] * right[0 + x * 4] +
                    left[y + 1 * 4] * right[1 + x * 4] +
                    left[y + 2 * 4] * right[2 + x * 4] +
                    left[y + 3 * 4] * right[3 + x * 4];
        }

        return result;
    }

    vec4 operator*(const mat4& left, const vec4& right)
    {
        return { left[0 + 0 * 4] * right.x + left[0 + 1 * 4] * right.y + left[0 + 2 * 4] * right.z + left[0 + 3 * 4] * right.w,  // x
            left[1 + 0 * 4] * right.x + left[1 + 1 * 4] * right.y + left[1 + 2 * 4] * right.z + left[1 + 3 * 4] * right.w,  // y
            left[2 + 0 * 4] * right.x + left[2 + 1 * 4] * right.y + left[2 + 2 * 4] * right.z + left[2 + 3 * 4] * right.w,  // z
            left[3 + 0 * 4] * right.x + left[3 + 1 * 4] * right.y + left[3 + 2 * 4] * right.z + left[3 + 3 * 4] * right.w };// w
    }


    quat::quat(const vec3& axis, float angle)
    {
        float sinHalfAngle = sin(angle / 2.0f);
        x = axis.x * sinHalfAngle;
        y = axis.y * sinHalfAngle;
        z = axis.z * sinHalfAngle;
        w = cos(angle / 2.0f);
    }

    std::string quat::toString() const
    {
        std::string out = "( " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + " )";
        return out;
    }

    quat quat::operator*(const quat& other) const
    {
        quat result;
        result.w = w * other.w - x * other.x - y * other.y - z * other.z;
        result.x = x * other.w + w * other.x + y * other.z - z * other.y;
        result.y = y * other.w + w * other.y + z * other.x - x * other.z;
        result.z = z * other.w + w * other.z + x * other.y - y * other.x;
        return result;
    }

    quat quat::operator*(const vec3& other) const
    {
        quat result;
        result.w = -x * other.x - y * other.y - z * other.z;
        result.x =  w * other.x + y * other.z - z * other.y;
        result.y =  w * other.y + z * other.x - x * other.z;
        result.z =  w * other.z + x * other.y - y * other.x;
        return result;
    }

    bool quat::operator==(const quat& other) const
    {
        return x == other.x && y == other.y && z == other.y && w == other.w;
    }

    quat quat::conjugate() const
    {
        return { -x, -y, -z, w };
    }

    // Used wikipedia "Quat-derived rotation matrix"
    // This can only be used for "unit quaternion"
    mat4 quat::toRotationMatrix() const
    {
        // We force the usage of unit quaternion here...
        quat unitQuat = normalize();

        float s = 1.0f;

        float sqx = unitQuat.x * unitQuat.x;
        float sqy = unitQuat.y * unitQuat.y;
        float sqz = unitQuat.z * unitQuat.z;

        mat4 rotationMatrix;

        rotationMatrix[0] = 1.0f - 2 * s * (sqy + sqz);
        rotationMatrix[4] = 2 * s * (unitQuat.x * unitQuat.y - unitQuat.z * unitQuat.w);
        rotationMatrix[8] = 2 * s * (unitQuat.x * unitQuat.z + unitQuat.y * unitQuat.w);

        rotationMatrix[1] = 2 * s * (unitQuat.x * unitQuat.y + unitQuat.z * unitQuat.w);
        rotationMatrix[5] = 1.0f - 2 * s * (sqx + sqz);
        rotationMatrix[9] = 2 * s * (unitQuat.y * unitQuat.z - unitQuat.x * unitQuat.w);

        rotationMatrix[2] = 2 * s * (unitQuat.x * unitQuat.z - unitQuat.y * unitQuat.w);
        rotationMatrix[6] = 2 * s * (unitQuat.y * unitQuat.z + unitQuat.x * unitQuat.w);
        rotationMatrix[10] = 1.0f - 2 * s * (sqx + sqy);

        rotationMatrix[15] = 1.0f;

        return rotationMatrix;
    }

    // Copied from wikipedia : https://en.wikipedia.org/wiki/Slerp
    #define QUATERNION_SLERP__DOT_THRESHOLD 0.9995f
    quat quat::slerp(const quat& other, float amount) const
    {
        // Only unit quaternions are valid rotations.
        // Normalize to avoid undefined behavior.
        quat ua = normalize();
        quat ub = other.normalize();

        // Compute the cosine of the angle between the two vectors.
        float dot = ua.dotp(ub);

        // If the dot product is negative, slerp won't take
        // the shorter path. Note that v1 and -v1 are equivalent when
        // the negation is applied to all four components. Fix by
        // reversing one quaternion.
        if (dot < 0.0f) {
            ub = ub * -1.0f;
            dot = -dot;
        }

        if (dot > QUATERNION_SLERP__DOT_THRESHOLD)
        {
            // If the inputs are too close for comfort, linearly interpolate
            // and normalize the result.
            quat result = ua + ((ub - ua) * amount);
            return result.normalize();
        }

        // Since dot is in range [0, DOT_THRESHOLD], acos is safe
        float theta_0 = acos(dot);        // theta_0 = angle between input vectors
        float theta = theta_0 * amount;   // theta = angle between v0 and result
        float sin_theta = sin(theta);     // compute this value only once
        float sin_theta_0 = sin(theta_0); // compute this value only once

        float s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
        float s1 = sin_theta / sin_theta_0;

        return (ua * s0) + (ub * s1);
    }


    mat4 create_proj_mat_ortho(
            float left,
            float right,
            float top,
            float bottom,
            float zNear,
            float zFar
            )
    {
        mat4 result;
        result.setIdentity();

        result[0] = 2.0f / (right - left);
        result[1 + 1 * 4] = 2.0f / (top - bottom);
        result[2 + 2 * 4] = -2.0f / (zFar - zNear);
        result[3 + 3 * 4] = 1.0f;
        result[0 + 3 * 4] = -((right + left) / (right - left));
        result[1 + 3 * 4] = -((top + bottom) / (top - bottom));
        result[2 + 3 * 4] = -((zFar + zNear) / (zFar - zNear));

        return result;
    }

    mat4 create_perspective_projection_matrix(
            float aspectRatio,
            float fov,
            float zNear, float zFar
            )
    {
        mat4 matrix;
        matrix.setIdentity();
        matrix[0] =			1.0f / (aspectRatio * tan(fov / 2.0f));
        matrix[1 + 1 * 4] = 1.0f / (tan(fov / 2.0f));
        matrix[2 + 2 * 4] = -((zFar + zNear) / (zFar - zNear));
        matrix[3 + 2 * 4] = -1.0f;
        matrix[2 + 3 * 4] = -((2.0f * zFar * zNear) / (zFar - zNear));
        matrix[3 + 3 * 4] = 0.0f;
        return matrix;
    }

    mat4 create_rotation_matrix(float pitch, float yaw, float roll)
    {
        mat4 pitchMatrix;
        mat4 yawMatrix;
        mat4 rollMatrix;
        pitchMatrix.setIdentity();
        yawMatrix.setIdentity();
        rollMatrix.setIdentity();

        pitchMatrix[1 + 1 * 4] = cos(pitch);
        pitchMatrix[1 + 2 * 4] = -sin(pitch);
        pitchMatrix[2 + 1 * 4] = sin(pitch);
        pitchMatrix[2 + 2 * 4] = cos(pitch);

        yawMatrix[0 + 0 * 4] = cos(yaw);
        yawMatrix[0 + 2 * 4] = sin(yaw);
        yawMatrix[2 + 0 * 4] = -sin(yaw);
        yawMatrix[2 + 2 * 4] = cos(yaw);

        rollMatrix[0 + 0 * 4] = cos(roll);
        rollMatrix[0 + 1 * 4] = -sin(roll);
        rollMatrix[1 + 0 * 4] = sin(roll);
        rollMatrix[1 + 1 * 4] = cos(roll);

        return pitchMatrix * yawMatrix * rollMatrix;
    }
}
