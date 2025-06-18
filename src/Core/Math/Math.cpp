//
//  Math.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"

namespace FinalStorm {

// Transform implementation
float4x4 Transform::getMatrix() const {
    float4x4 T = matrix_translation(position);
    float4x4 R = matrix_rotation(rotation);
    float4x4 S = matrix_scale(scale);
    return simd_mul(simd_mul(T, R), S);
}

void Transform::setRotationFromEuler(float pitch, float yaw, float roll) {
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);
    
    rotation.w = cr * cp * cy + sr * sp * sy;
    rotation.x = sr * cp * cy - cr * sp * sy;
    rotation.y = cr * sp * cy + sr * cp * sy;
    rotation.z = cr * cp * sy - sr * sp * cy;
}

float3 Transform::getForward() const {
    float4x4 rotMatrix = matrix_rotation(rotation);
    return -simd_make_float3(rotMatrix.columns[2].x, rotMatrix.columns[2].y, rotMatrix.columns[2].z);
}

float3 Transform::getRight() const {
    float4x4 rotMatrix = matrix_rotation(rotation);
    return simd_make_float3(rotMatrix.columns[0].x, rotMatrix.columns[0].y, rotMatrix.columns[0].z);
}

float3 Transform::getUp() const {
    float4x4 rotMatrix = matrix_rotation(rotation);
    return simd_make_float3(rotMatrix.columns[1].x, rotMatrix.columns[1].y, rotMatrix.columns[1].z);
}

Transform Transform::lerp(const Transform& a, const Transform& b, float t) {
    Transform result;
    result.position = simd_mix(a.position, b.position, t);
    result.scale = simd_mix(a.scale, b.scale, t);
    
    // Quaternion slerp
    float dot = simd_dot(a.rotation, b.rotation);
    float4 q2 = b.rotation;
    
    if (dot < 0.0f) {
        q2 = -b.rotation;
        dot = -dot;
    }
    
    if (dot > 0.9995f) {
        result.rotation = simd_normalize(simd_mix(a.rotation, q2, t));
    } else {
        float theta = acosf(dot);
        float sinTheta = sinf(theta);
        float wa = sinf((1.0f - t) * theta) / sinTheta;
        float wb = sinf(t * theta) / sinTheta;
        result.rotation = a.rotation * wa + q2 * wb;
    }
    
    return result;
}

// Camera implementation
Camera::Camera() 
    : m_position(simd_make_float3(0.0f, 0.0f, 5.0f)),
      m_target(simd_make_float3(0.0f, 0.0f, 0.0f)),
      m_up(simd_make_float3(0.0f, 1.0f, 0.0f)) {
    updateViewMatrix();
    setPerspective(60.0f * M_PI / 180.0f, 16.0f/9.0f, 0.1f, 1000.0f);
}

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane) {
    m_projectionMatrix = matrix_perspective_right_hand(fov, aspect, nearPlane, farPlane);
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    m_projectionMatrix = matrix_orthographic(left, right, bottom, top, nearPlane, farPlane);
}

void Camera::updateViewMatrix() {
    m_viewMatrix = matrix_look_at_right_hand(m_position, m_target, m_up);
}

// Matrix functions
float4x4 matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ) {
    float ys = 1 / tanf(fovyRadians * 0.5);
    float xs = ys / aspect;
    float zs = farZ / (nearZ - farZ);
    
    return simd_matrix_from_rows(
        simd_make_float4(xs, 0.0, 0.0, 0.0),
        simd_make_float4(0.0, ys, 0.0, 0.0),
        simd_make_float4(0.0, 0.0, zs, nearZ * zs),
        simd_make_float4(0.0, 0.0, -1.0, 0.0)
    );
}

float4x4 matrix_look_at_right_hand(float3 eye, float3 center, float3 up) {
    float3 z = simd_normalize(eye - center);
    float3 x = simd_normalize(simd_cross(up, z));
    float3 y = simd_cross(z, x);
    
    return simd_matrix_from_rows(
        simd_make_float4(x.x, x.y, x.z, -simd_dot(x, eye)),
        simd_make_float4(y.x, y.y, y.z, -simd_dot(y, eye)),
        simd_make_float4(z.x, z.y, z.z, -simd_dot(z, eye)),
        simd_make_float4(0.0, 0.0, 0.0, 1.0)
    );
}

float4x4 matrix_identity() {
    return matrix_float4x4{{
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
}

float4x4 matrix_translation(float3 translation) {
    float4x4 m = matrix_identity();
    m.columns[3] = simd_make_float4(translation.x, translation.y, translation.z, 1.0);
    return m;
}

float4x4 matrix_rotation(float4 quaternion) {
    float qxx = quaternion.x * quaternion.x;
    float qyy = quaternion.y * quaternion.y;
    float qzz = quaternion.z * quaternion.z;
    float qxz = quaternion.x * quaternion.z;
    float qxy = quaternion.x * quaternion.y;
    float qyz = quaternion.y * quaternion.z;
    float qwx = quaternion.w * quaternion.x;
    float qwy = quaternion.w * quaternion.y;
    float qwz = quaternion.w * quaternion.z;
    
    float4x4 m;
    m.columns[0] = simd_make_float4(1 - 2 * (qyy + qzz), 2 * (qxy + qwz), 2 * (qxz - qwy), 0);
    m.columns[1] = simd_make_float4(2 * (qxy - qwz), 1 - 2 * (qxx + qzz), 2 * (qyz + qwx), 0);
    m.columns[2] = simd_make_float4(2 * (qxz + qwy), 2 * (qyz - qwx), 1 - 2 * (qxx + qyy), 0);
    m.columns[3] = simd_make_float4(0, 0, 0, 1);
    
    return m;
}

float4x4 matrix_scale(float3 scale) {
    return matrix_float4x4{{
        {scale.x, 0.0, 0.0, 0.0},
        {0.0, scale.y, 0.0, 0.0},
        {0.0, 0.0, scale.z, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    }};
}

float3x3 matrix3x3_upper_left(const float4x4& m) {
    return simd_matrix(
        simd_make_float3(m.columns[0].x, m.columns[0].y, m.columns[0].z),
        simd_make_float3(m.columns[1].x, m.columns[1].y, m.columns[1].z),
        simd_make_float3(m.columns[2].x, m.columns[2].y, m.columns[2].z)
    );
}

float4x4 matrix_orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    return simd_matrix_from_rows(
        simd_make_float4(2.0f / rsl, 0.0f, 0.0f, -ral / rsl),
        simd_make_float4(0.0f, 2.0f / tsb, 0.0f, -tab / tsb),
        simd_make_float4(0.0f, 0.0f, -2.0f / fsn, -fan / fsn),
        simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f)
    );
}

} // namespace FinalStorm