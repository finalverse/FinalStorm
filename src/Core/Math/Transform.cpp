// src/Core/Math/Transform.cpp
// Transform component implementation
// Handles position, rotation, and scale with hierarchical transformations

#include "Core/Math/Transform.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f)
    , rotation(1.0f, 0.0f, 0.0f, 0.0f)  // Identity quaternion
    , scale(1.0f, 1.0f, 1.0f)
    , isDirty(true) {
}

Transform::Transform(const float3& pos, const quaternion& rot, const float3& scl)
    : position(pos)
    , rotation(rot)
    , scale(scl)
    , isDirty(true) {
}

void Transform::setPosition(const float3& pos) {
    position = pos;
    isDirty = true;
}

void Transform::setRotation(const quaternion& rot) {
    rotation = rot;
    isDirty = true;
}

void Transform::setScale(const float3& scl) {
    scale = scl;
    isDirty = true;
}

void Transform::translate(const float3& delta) {
    position += delta;
    isDirty = true;
}

void Transform::rotate(const quaternion& rot) {
    rotation = rot * rotation;
    isDirty = true;
}

void Transform::rotateEuler(const float3& eulerAngles) {
    quaternion rotX = quaternion(Math::radians(eulerAngles.x), float3(1, 0, 0));
    quaternion rotY = quaternion(Math::radians(eulerAngles.y), float3(0, 1, 0));
    quaternion rotZ = quaternion(Math::radians(eulerAngles.z), float3(0, 0, 1));
    rotate(rotZ * rotY * rotX);
}

void Transform::setRotationFromEuler(float pitch, float yaw, float roll) {
    quaternion rotX = quaternion(Math::radians(pitch), float3(1, 0, 0));
    quaternion rotY = quaternion(Math::radians(yaw), float3(0, 1, 0));
    quaternion rotZ = quaternion(Math::radians(roll), float3(0, 0, 1));
    rotation = rotZ * rotY * rotX;
    isDirty = true;
}

float4x4 Transform::getMatrix() const {
    if (isDirty) {
        updateMatrix();
    }
    return matrix;
}

float3 Transform::getForward() const {
    float4x4 m = getMatrix();
    return -float3(m[2][0], m[2][1], m[2][2]);
}

float3 Transform::getRight() const {
    float4x4 m = getMatrix();
    return float3(m[0][0], m[0][1], m[0][2]);
}

float3 Transform::getUp() const {
    float4x4 m = getMatrix();
    return float3(m[1][0], m[1][1], m[1][2]);
}

void Transform::lookAt(const float3& target, const float3& up) {
    float3 forward = Math::normalize(target - position);
    float3 right = Math::normalize(Math::cross(forward, up));
    float3 newUp = Math::cross(right, forward);
    
    // Create rotation matrix
    float4x4 rotMatrix = float4x4(1.0f);
    rotMatrix[0] = float4(right.x, right.y, right.z, 0.0f);
    rotMatrix[1] = float4(newUp.x, newUp.y, newUp.z, 0.0f);
    rotMatrix[2] = float4(-forward.x, -forward.y, -forward.z, 0.0f);
    
    // Extract quaternion from matrix
    rotation = matrixToQuaternion(rotMatrix);
    isDirty = true;
}

void Transform::updateMatrix() const {
    // Create scale matrix
    float4x4 scaleMatrix = float4x4(1.0f);
    scaleMatrix[0][0] = scale.x;
    scaleMatrix[1][1] = scale.y;
    scaleMatrix[2][2] = scale.z;
    
    // Create rotation matrix from quaternion
    float4x4 rotationMatrix = quaternionToMatrix(rotation);
    
    // Create translation matrix
    float4x4 translationMatrix = float4x4(1.0f);
    translationMatrix[3] = float4(position.x, position.y, position.z, 1.0f);
    
    // Combine: T * R * S
    matrix = translationMatrix * rotationMatrix * scaleMatrix;
    isDirty = false;
}

float4x4 Transform::quaternionToMatrix(const quaternion& q) const {
    float xx = q.x * q.x;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float xw = q.x * q.w;
    float yy = q.y * q.y;
    float yz = q.y * q.z;
    float yw = q.y * q.w;
    float zz = q.z * q.z;
    float zw = q.z * q.w;
    
    float4x4 result;
    result[0] = float4(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0);
    result[1] = float4(2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0);
    result[2] = float4(2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0);
    result[3] = float4(0, 0, 0, 1);
    
    return result;
}

quaternion Transform::matrixToQuaternion(const float4x4& m) const {
    float trace = m[0][0] + m[1][1] + m[2][2];
    quaternion q;
    
    if (trace > 0) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        q.w = 0.25f / s;
        q.x = (m[2][1] - m[1][2]) * s;
        q.y = (m[0][2] - m[2][0]) * s;
        q.z = (m[1][0] - m[0][1]) * s;
    } else {
        if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
            float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
            q.w = (m[2][1] - m[1][2]) / s;
            q.x = 0.25f * s;
            q.y = (m[0][1] + m[1][0]) / s;
            q.z = (m[0][2] + m[2][0]) / s;
        } else if (m[1][1] > m[2][2]) {
            float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
            q.w = (m[0][2] - m[2][0]) / s;
            q.x = (m[0][1] + m[1][0]) / s;
            q.y = 0.25f * s;
            q.z = (m[1][2] + m[2][1]) / s;
        } else {
            float s = 2.0f * sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]);
            q.w = (m[1][0] - m[0][1]) / s;
            q.x = (m[0][2] + m[2][0]) / s;
            q.y = (m[1][2] + m[2][1]) / s;
            q.z = 0.25f * s;
        }
    }
    
    return q;
}

} // namespace FinalStorm