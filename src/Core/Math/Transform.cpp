// src/Core/Math/Transform.cpp
// Transform component implementation
// Handles position, rotation, and scale with hierarchical transformations

#include "Core/Math/Transform.h"
#include "Core/Math/Math.h"
#include <cmath>

namespace FinalStorm {

Transform::Transform()
    : position(make_vec3(0.0f, 0.0f, 0.0f))
    , rotation(simd_quaternion(0.0f, make_vec3(0.0f, 1.0f, 0.0f)))  // Identity quaternion
    , scale(make_vec3(1.0f, 1.0f, 1.0f))
    , isDirty(true) {
}

Transform::Transform(const vec3& pos, const quat& rot, const vec3& scl)
    : position(pos)
    , rotation(rot)
    , scale(scl)
    , isDirty(true) {
}

void Transform::setPosition(const vec3& pos) {
    position = pos;
    isDirty = true;
}

void Transform::setRotation(const quat& rot) {
    rotation = rot;
    isDirty = true;
}

void Transform::setScale(const vec3& scl) {
    scale = scl;
    isDirty = true;
}

void Transform::translate(const vec3& delta) {
    position += delta;
    isDirty = true;
}

void Transform::rotate(const quat& rot) {
    rotation = simd_mul(rot, rotation);
    isDirty = true;
}

void Transform::rotateEuler(const vec3& eulerAngles) {
    quat rotX = simd_quaternion(radians(eulerAngles.x), make_vec3(1, 0, 0));
    quat rotY = simd_quaternion(radians(eulerAngles.y), make_vec3(0, 1, 0));
    quat rotZ = simd_quaternion(radians(eulerAngles.z), make_vec3(0, 0, 1));
    rotate(simd_mul(simd_mul(rotZ, rotY), rotX));
}

void Transform::setRotationFromEuler(float pitch, float yaw, float roll) {
    quat rotX = simd_quaternion(radians(pitch), make_vec3(1, 0, 0));
    quat rotY = simd_quaternion(radians(yaw), make_vec3(0, 1, 0));
    quat rotZ = simd_quaternion(radians(roll), make_vec3(0, 0, 1));
    rotation = simd_mul(simd_mul(rotZ, rotY), rotX);
    isDirty = true;
}

mat4 Transform::getMatrix() const {
    if (isDirty) {
        updateMatrix();
    }
    return matrix;
}

vec3 Transform::getForward() const {
    mat4 m = getMatrix();
    return -make_vec3(m.columns[2].x, m.columns[2].y, m.columns[2].z);
}

vec3 Transform::getRight() const {
    mat4 m = getMatrix();
    return make_vec3(m.columns[0].x, m.columns[0].y, m.columns[0].z);
}

vec3 Transform::getUp() const {
    mat4 m = getMatrix();
    return make_vec3(m.columns[1].x, m.columns[1].y, m.columns[1].z);
}

void Transform::lookAt(const vec3& target, const vec3& up) {
    vec3 forward = normalize(target - position);
    vec3 right = normalize(cross(forward, up));
    vec3 newUp = cross(right, forward);
    
    // Create rotation matrix
    mat4 rotMatrix = matrix_identity_float4x4;
    rotMatrix.columns[0] = make_vec4(right.x, right.y, right.z, 0.0f);
    rotMatrix.columns[1] = make_vec4(newUp.x, newUp.y, newUp.z, 0.0f);
    rotMatrix.columns[2] = make_vec4(-forward.x, -forward.y, -forward.z, 0.0f);
    rotMatrix.columns[3] = make_vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Extract quaternion from matrix
    rotation = matrixToQuaternion(rotMatrix);
    isDirty = true;
}

void Transform::updateMatrix() const {
    // Create scale matrix
    mat4 scaleMatrix = matrix_identity_float4x4;
    scaleMatrix.columns[0].x = scale.x;
    scaleMatrix.columns[1].y = scale.y;
    scaleMatrix.columns[2].z = scale.z;
    
    // Create rotation matrix from quaternion
    mat4 rotationMatrix = simd_matrix4x4(rotation);
    
    // Create translation matrix
    mat4 translationMatrix = matrix_identity_float4x4;
    translationMatrix.columns[3] = make_vec4(position.x, position.y, position.z, 1.0f);
    
    // Combine: T * R * S
    matrix = simd_mul(simd_mul(translationMatrix, rotationMatrix), scaleMatrix);
    isDirty = false;
}

quat Transform::matrixToQuaternion(const mat4& m) const {
    float trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;
    quat q;
    
    if (trace > 0) {
        float s = 0.5f / sqrtf(trace + 1.0f);
        q.vector.w = 0.25f / s;
        q.vector.x = (m.columns[2].y - m.columns[1].z) * s;
        q.vector.y = (m.columns[0].z - m.columns[2].x) * s;
        q.vector.z = (m.columns[1].x - m.columns[0].y) * s;
    } else {
        if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z) {
            float s = 2.0f * sqrtf(1.0f + m.columns[0].x - m.columns[1].y - m.columns[2].z);
            q.vector.w = (m.columns[2].y - m.columns[1].z) / s;
            q.vector.x = 0.25f * s;
            q.vector.y = (m.columns[0].y + m.columns[1].x) / s;
            q.vector.z = (m.columns[0].z + m.columns[2].x) / s;
        } else if (m.columns[1].y > m.columns[2].z) {
            float s = 2.0f * sqrtf(1.0f + m.columns[1].y - m.columns[0].x - m.columns[2].z);
            q.vector.w = (m.columns[0].z - m.columns[2].x) / s;
            q.vector.x = (m.columns[0].y + m.columns[1].x) / s;
            q.vector.y = 0.25f * s;
            q.vector.z = (m.columns[1].z + m.columns[2].y) / s;
        } else {
            float s = 2.0f * sqrtf(1.0f + m.columns[2].z - m.columns[0].x - m.columns[1].y);
            q.vector.w = (m.columns[1].x - m.columns[0].y) / s;
            q.vector.x = (m.columns[0].z + m.columns[2].x) / s;
            q.vector.y = (m.columns[1].z + m.columns[2].y) / s;
            q.vector.z = 0.25f * s;
        }
    }
    
    return q;
}

} // namespace FinalStorm