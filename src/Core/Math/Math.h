//
//  Math.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include <simd/simd.h>
#include <cmath>

namespace FinalStorm {

using float2 = simd_float2;
using float3 = simd_float3;
using float4 = simd_float4;
using float4x4 = simd_float4x4;
using float3x3 = simd_float3x3;
using uint4 = simd_uint4;

// Forward declaration
struct Transform;

class Camera {
public:
    Camera();
    
    void setPosition(const float3& pos) { m_position = pos; updateViewMatrix(); }
    void setTarget(const float3& target) { m_target = target; updateViewMatrix(); }
    void setUp(const float3& up) { m_up = up; updateViewMatrix(); }
    
    void setPerspective(float fov, float aspect, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    
    const float4x4& getViewMatrix() const { return m_viewMatrix; }
    const float4x4& getProjectionMatrix() const { return m_projectionMatrix; }
    float4x4 getViewProjectionMatrix() const { return simd_mul(m_projectionMatrix, m_viewMatrix); }
    
    const float3& getPosition() const { return m_position; }
    const float3& getTarget() const { return m_target; }
    
private:
    void updateViewMatrix();
    
    float3 m_position;
    float3 m_target;
    float3 m_up;
    
    float4x4 m_viewMatrix;
    float4x4 m_projectionMatrix;
};

// Math utilities
float4x4 matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ);
float4x4 matrix_look_at_right_hand(float3 eye, float3 center, float3 up);
float4x4 matrix_identity();
float4x4 matrix_translation(float3 translation);
float4x4 matrix_rotation(float4 quaternion);
float4x4 matrix_scale(float3 scale);
float3x3 matrix3x3_upper_left(const float4x4& m);
float4x4 matrix_orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

// Helper functions for cleaner initialization
inline float3 make_float3(float x, float y, float z) {
    return simd_make_float3(x, y, z);
}

inline float3 make_float3(float v) {
    return simd_make_float3(v, v, v);
}

inline float4 make_float4(float x, float y, float z, float w) {
    return simd_make_float4(x, y, z, w);
}

inline float4 make_float4(float v) {
    return simd_make_float4(v, v, v, v);
}

// Simple helper to create a quaternion from angle and axis in radians.
inline quat quaternion(float angle, const float3& axis) {
#ifdef __APPLE__
    return simd_quaternion(angle, axis);
#else
    return glm::angleAxis(angle, axis);
#endif
}

} // namespace FinalStorm
