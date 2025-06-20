// src/Core/Math/Camera.cpp
// Camera implementation for 3D viewing
// Handles view and projection matrices with frustum culling support

#include "Core/Math/Camera.h"
#include "Core/Math/Math.h"
#include <cmath>

namespace FinalStorm {

Camera::Camera()
    : position(make_vec3(0.0f, 0.0f, 5.0f))
    , target(make_vec3(0.0f, 0.0f, 0.0f))
    , up(make_vec3(0.0f, 1.0f, 0.0f))
    , fov(radians(45.0f))
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , viewDirty(true)
    , projectionDirty(true) {
}

void Camera::setPosition(const vec3& pos) {
    position = pos;
    viewDirty = true;
}

void Camera::setTarget(const vec3& tgt) {
    target = tgt;
    viewDirty = true;
}

void Camera::setUp(const vec3& upVec) {
    up = upVec;
    viewDirty = true;
}

void Camera::lookAt(const vec3& pos, const vec3& tgt, const vec3& upVec) {
    position = pos;
    target = tgt;
    up = upVec;
    viewDirty = true;
}

void Camera::setFOV(float fovRadians) {
    fov = fovRadians;
    projectionDirty = true;
}

void Camera::setAspectRatio(float ratio) {
    aspectRatio = ratio;
    projectionDirty = true;
}

void Camera::setNearFar(float near, float far) {
    nearPlane = near;
    farPlane = far;
    projectionDirty = true;
}

mat4 Camera::getViewMatrix() const {
    if (viewDirty) {
        updateViewMatrix();
    }
    return viewMatrix;
}

mat4 Camera::getProjectionMatrix() const {
    if (projectionDirty) {
        updateProjectionMatrix();
    }
    return projectionMatrix;
}

mat4 Camera::getViewProjectionMatrix() const {
    return simd_mul(getProjectionMatrix(), getViewMatrix());
}

vec3 Camera::getForward() const {
    return normalize(target - position);
}

vec3 Camera::getRight() const {
    return normalize(cross(getForward(), up));
}

vec3 Camera::getUp() const {
    return normalize(cross(getRight(), getForward()));
}

void Camera::move(const vec3& delta) {
    position += delta;
    target += delta;
    viewDirty = true;
}

void Camera::rotate(float yaw, float pitch) {
    // Get the current forward and right vectors
    vec3 forward = getForward();
    vec3 right = getRight();
    
    // Create rotation quaternions
    quat yawRotation = simd_quaternion(yaw, make_vec3(0, 1, 0));
    quat pitchRotation = simd_quaternion(pitch, right);
    
    // Apply rotations
    quat totalRotation = simd_mul(yawRotation, pitchRotation);
    
    // Rotate forward vector
    mat4 rotMatrix = simd_matrix4x4(totalRotation);
    vec4 rotatedForward = simd_mul(rotMatrix, make_vec4(forward.x, forward.y, forward.z, 0.0f));
    
    // Update target
    target = position + make_vec3(rotatedForward.x, rotatedForward.y, rotatedForward.z);
    viewDirty = true;
}

void Camera::zoom(float delta) {
    vec3 forward = getForward();
    position += forward * delta;
    viewDirty = true;
}

bool Camera::isInFrustum(const vec3& point) const {
    // Simple frustum test - can be expanded for full frustum culling
    mat4 vp = getViewProjectionMatrix();
    vec4 clipSpace = simd_mul(vp, make_vec4(point.x, point.y, point.z, 1.0f));
    
    // Perspective divide
    if (clipSpace.w != 0) {
        clipSpace.x /= clipSpace.w;
        clipSpace.y /= clipSpace.w;
        clipSpace.z /= clipSpace.w;
    }
    
    // Check if point is in NDC space
    return clipSpace.x >= -1.0f && clipSpace.x <= 1.0f &&
           clipSpace.y >= -1.0f && clipSpace.y <= 1.0f &&
           clipSpace.z >= 0.0f && clipSpace.z <= 1.0f;
}

void Camera::updateViewMatrix() const {
    viewMatrix = lookAt(position, target, up);
    viewDirty = false;
}

void Camera::updateProjectionMatrix() const {
    projectionMatrix = perspective(fov, aspectRatio, nearPlane, farPlane);
    projectionDirty = false;
}

} // namespace FinalStorm