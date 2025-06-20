// src/Core/Math/Camera.cpp
// Camera implementation for 3D viewing
// Handles view and projection matrices with frustum culling support

#include "Core/Math/Camera.h"
#include "Core/Math/Math.h"
#include <cmath>

namespace FinalStorm {

Camera::Camera()
    : position(0.0f, 0.0f, 5.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , fov(Math::radians(45.0f))
    , aspectRatio(16.0f / 9.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , viewDirty(true)
    , projectionDirty(true) {
}

void Camera::setPosition(const float3& pos) {
    position = pos;
    viewDirty = true;
}

void Camera::setTarget(const float3& tgt) {
    target = tgt;
    viewDirty = true;
}

void Camera::setUp(const float3& upVec) {
    up = upVec;
    viewDirty = true;
}

void Camera::lookAt(const float3& pos, const float3& tgt, const float3& upVec) {
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

float4x4 Camera::getViewMatrix() const {
    if (viewDirty) {
        updateViewMatrix();
    }
    return viewMatrix;
}

float4x4 Camera::getProjectionMatrix() const {
    if (projectionDirty) {
        updateProjectionMatrix();
    }
    return projectionMatrix;
}

float4x4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

float3 Camera::getForward() const {
    return Math::normalize(target - position);
}

float3 Camera::getRight() const {
    return Math::normalize(Math::cross(getForward(), up));
}

float3 Camera::getUp() const {
    return Math::normalize(Math::cross(getRight(), getForward()));
}

void Camera::move(const float3& delta) {
    position += delta;
    target += delta;
    viewDirty = true;
}

void Camera::rotate(float yaw, float pitch) {
    // Get the current forward and right vectors
    float3 forward = getForward();
    float3 right = getRight();
    
    // Create rotation quaternions
    quaternion yawRotation = quaternion(yaw, float3(0, 1, 0));
    quaternion pitchRotation = quaternion(pitch, right);
    
    // Apply rotations
    quaternion totalRotation = yawRotation * pitchRotation;
    
    // Rotate forward vector
    float4x4 rotMatrix = Transform::quaternionToMatrix(totalRotation);
    float4 rotatedForward = rotMatrix * float4(forward.x, forward.y, forward.z, 0.0f);
    
    // Update target
    target = position + float3(rotatedForward.x, rotatedForward.y, rotatedForward.z);
    viewDirty = true;
}

void Camera::zoom(float delta) {
    float3 forward = getForward();
    position += forward * delta;
    viewDirty = true;
}

bool Camera::isInFrustum(const float3& point) const {
    // Simple frustum test - can be expanded for full frustum culling
    float4x4 vp = getViewProjectionMatrix();
    float4 clipSpace = vp * float4(point.x, point.y, point.z, 1.0f);
    
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
    viewMatrix = Math::lookAt(position, target, up);
    viewDirty = false;
}

void Camera::updateProjectionMatrix() const {
    projectionMatrix = Math::perspective(fov, aspectRatio, nearPlane, farPlane);
    projectionDirty = false;
}

} // namespace FinalStorm