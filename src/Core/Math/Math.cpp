// src/Core/Math/Math.cpp
// Core math utilities implementation
// Provides platform-agnostic math operations using the types defined in MathTypes.h

#include "Core/Math/Math.h"
#include <cmath>
#include <algorithm>

namespace FinalStorm {
namespace Math {

float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

float degrees(float radians) {
    return radians * (180.0f / M_PI);
}

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float smoothstep(float edge0, float edge1, float x) {
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

#ifdef USE_SIMD

float3 normalize(const float3& v) {
    return simd_normalize(v);
}

float dot(const float3& a, const float3& b) {
    return simd_dot(a, b);
}

float3 cross(const float3& a, const float3& b) {
    return simd_cross(a, b);
}

float length(const float3& v) {
    return simd_length(v);
}

float4x4 inverse(const float4x4& m) {
    return simd_inverse(m);
}

float4x4 transpose(const float4x4& m) {
    return simd_transpose(m);
}

float4x4 lookAt(const float3& eye, const float3& center, const float3& up) {
    float3 f = normalize(center - eye);
    float3 s = normalize(cross(f, up));
    float3 u = cross(s, f);
    
    float4x4 result = float4x4(
        float4(s.x, u.x, -f.x, 0.0f),
        float4(s.y, u.y, -f.y, 0.0f),
        float4(s.z, u.z, -f.z, 0.0f),
        float4(-dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f)
    );
    
    return transpose(result);
}

float4x4 perspective(float fovy, float aspect, float near, float far) {
    float y = 1.0f / tanf(fovy * 0.5f);
    float x = y / aspect;
    float z = far / (near - far);
    
    return float4x4(
        float4(x, 0, 0, 0),
        float4(0, y, 0, 0),
        float4(0, 0, z, -1),
        float4(0, 0, near * z, 0)
    );
}

float4x4 ortho(float left, float right, float bottom, float top, float near, float far) {
    float rl = 1.0f / (right - left);
    float tb = 1.0f / (top - bottom);
    float fn = 1.0f / (far - near);
    
    return float4x4(
        float4(2.0f * rl, 0, 0, 0),
        float4(0, 2.0f * tb, 0, 0),
        float4(0, 0, -1.0f * fn, 0),
        float4(-(right + left) * rl, -(top + bottom) * tb, -near * fn, 1)
    );
}

float4x4 translate(const float4x4& m, const float3& v) {
    float4x4 result = m;
    result.columns[3] = m.columns[0] * v.x + m.columns[1] * v.y + m.columns[2] * v.z + m.columns[3];
    return result;
}

float4x4 rotate(const float4x4& m, float angle, const float3& axis) {
    float c = cosf(angle);
    float s = sinf(angle);
    float3 a = normalize(axis);
    float3 temp = (1.0f - c) * a;
    
    float4x4 rotation;
    rotation.columns[0][0] = c + temp[0] * a[0];
    rotation.columns[0][1] = temp[0] * a[1] + s * a[2];
    rotation.columns[0][2] = temp[0] * a[2] - s * a[1];
    rotation.columns[0][3] = 0;
    
    rotation.columns[1][0] = temp[1] * a[0] - s * a[2];
    rotation.columns[1][1] = c + temp[1] * a[1];
    rotation.columns[1][2] = temp[1] * a[2] + s * a[0];
    rotation.columns[1][3] = 0;
    
    rotation.columns[2][0] = temp[2] * a[0] + s * a[1];
    rotation.columns[2][1] = temp[2] * a[1] - s * a[0];
    rotation.columns[2][2] = c + temp[2] * a[2];
    rotation.columns[2][3] = 0;
    
    rotation.columns[3] = float4(0, 0, 0, 1);
    
    return m * rotation;
}

float4x4 scale(const float4x4& m, const float3& v) {
    float4x4 result;
    result.columns[0] = m.columns[0] * v.x;
    result.columns[1] = m.columns[1] * v.y;
    result.columns[2] = m.columns[2] * v.z;
    result.columns[3] = m.columns[3];
    return result;
}

#else // GLM implementation

float3 normalize(const float3& v) {
    return glm::normalize(v);
}

float dot(const float3& a, const float3& b) {
    return glm::dot(a, b);
}

float3 cross(const float3& a, const float3& b) {
    return glm::cross(a, b);
}

float length(const float3& v) {
    return glm::length(v);
}

float4x4 inverse(const float4x4& m) {
    return glm::inverse(m);
}

float4x4 transpose(const float4x4& m) {
    return glm::transpose(m);
}

float4x4 lookAt(const float3& eye, const float3& center, const float3& up) {
    return glm::lookAt(eye, center, up);
}

float4x4 perspective(float fovy, float aspect, float near, float far) {
    return glm::perspective(fovy, aspect, near, far);
}

float4x4 ortho(float left, float right, float bottom, float top, float near, float far) {
    return glm::ortho(left, right, bottom, top, near, far);
}

float4x4 translate(const float4x4& m, const float3& v) {
    return glm::translate(m, v);
}

float4x4 rotate(const float4x4& m, float angle, const float3& axis) {
    return glm::rotate(m, angle, axis);
}

float4x4 scale(const float4x4& m, const float3& v) {
    return glm::scale(m, v);
}

#endif

} // namespace Math
} // namespace FinalStorm