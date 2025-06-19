//
// src/Scene/CameraController.cpp
// Camera management implementation using platform-agnostic MathTypes
// Handles view/projection matrices and 3D navigation with SIMD optimization on Apple platforms
//

#include "CameraController.h"

namespace FinalStorm {

CameraController::CameraController()
    : m_position(make_vec3(0.0f, 0.0f, 10.0f))
    , m_target(vec3_zero())
    , m_up(vec3_up())
    , m_fov(radians(60.0f))
    , m_aspect(16.0f / 9.0f)
    , m_near(0.1f)
    , m_far(1000.0f)
    , m_viewDirty(true)
    , m_projectionDirty(true)
    , m_viewMatrix(make_mat4())
    , m_projectionMatrix(make_mat4())
{
}

CameraController::~CameraController() = default;

void CameraController::setPosition(const vec3& position) {
    m_position = position;
    markViewDirty();
}

void CameraController::setTarget(const vec3& target) {
    m_target = target;
    markViewDirty();
}

void CameraController::setUpVector(const vec3& up) {
    m_up = up;
    markViewDirty();
}

void CameraController::setFieldOfView(float fov) {
    m_fov = fov;
    markProjectionDirty();
}

void CameraController::setAspectRatio(float aspect) {
    m_aspect = aspect;
    markProjectionDirty();
}

void CameraController::setNearPlane(float near) {
    m_near = near;
    markProjectionDirty();
}

void CameraController::setFarPlane(float far) {
    m_far = far;
    markProjectionDirty();
}

mat4 CameraController::getViewMatrix() const {
    if (m_viewDirty) {
        updateViewMatrix();
    }
    return m_viewMatrix;
}

mat4 CameraController::getProjectionMatrix() const {
    if (m_projectionDirty) {
        updateProjectionMatrix();
    }
    return m_projectionMatrix;
}

mat4 CameraController::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

void CameraController::orbit(float azimuth, float elevation) {
    // Calculate the vector from target to camera
    vec3 direction = m_position - m_target;
    float radius = length(direction);
    
    if (radius < 0.001f) {
        return; // Avoid division by zero
    }
    
    // Convert to spherical coordinates
    vec3 normalized = normalize(direction);
    float currentAzimuth = atan2(normalized.z, normalized.x);
    float currentElevation = asin(normalized.y);
    
    // Apply deltas
    currentAzimuth += azimuth;
    currentElevation += elevation;
    
    // Clamp elevation to avoid gimbal lock
    currentElevation = clamp(currentElevation, -HALF_PI + 0.01f, HALF_PI - 0.01f);
    
    // Convert back to cartesian coordinates
    vec3 newDirection = make_vec3(
        cos(currentElevation) * cos(currentAzimuth),
        sin(currentElevation),
        cos(currentElevation) * sin(currentAzimuth)
    );
    
    m_position = m_target + newDirection * radius;
    markViewDirty();
}

void CameraController::pan(float deltaX, float deltaY) {
    vec3 right = getRightVector();
    vec3 up = normalize(cross(right, getForwardVector()));
    
    // Scale pan speed based on distance to target
    float distance = length(m_position - m_target);
    float panSpeed = distance * 0.001f;
    
    vec3 panOffset = right * (deltaX * panSpeed) + up * (deltaY * panSpeed);
    
    m_position = m_position + panOffset;
    m_target = m_target + panOffset;
    markViewDirty();
}

void CameraController::zoom(float delta) {
    vec3 direction = normalize(m_target - m_position);
    float distance = length(m_position - m_target);
    
    // Zoom speed proportional to distance
    float zoomSpeed = distance * 0.1f;
    float zoomAmount = delta * zoomSpeed;
    
    // Prevent zooming too close to target
    if (distance - zoomAmount > 0.1f) {
        m_position = m_position + direction * zoomAmount;
        markViewDirty();
    }
}

void CameraController::dolly(float delta) {
    vec3 forward = getForwardVector();
    m_position = m_position + forward * delta;
    m_target = m_target + forward * delta;
    markViewDirty();
}

void CameraController::moveForward(float distance) {
    vec3 forward = getForwardVector();
    m_position = m_position + forward * distance;
    markViewDirty();
}

void CameraController::moveRight(float distance) {
    vec3 right = getRightVector();
    m_position = m_position + right * distance;
    markViewDirty();
}

void CameraController::moveUp(float distance) {
    m_position = m_position + m_up * distance;
    markViewDirty();
}

void CameraController::yaw(float angle) {
    // Rotate around the up vector
    vec3 forward = getForwardVector();
    
#ifdef __APPLE__
    // Use SIMD quaternion rotation on Apple platforms
    simd_quatf rotation = simd_quaternion(angle, m_up);
    vec3 newForward = simd_act(rotation, forward);
#else
    // Use GLM quaternion rotation on other platforms
    quat rotation = glm::angleAxis(angle, m_up);
    vec3 newForward = rotation * forward;
#endif
    
    m_target = m_position + newForward * length(m_target - m_position);
    markViewDirty();
}

void CameraController::pitch(float angle) {
    // Rotate around the right vector
    vec3 right = getRightVector();
    vec3 forward = getForwardVector();
    
#ifdef __APPLE__
    // Use SIMD quaternion rotation on Apple platforms
    simd_quatf rotation = simd_quaternion(angle, right);
    vec3 newForward = simd_act(rotation, forward);
#else
    // Use GLM quaternion rotation on other platforms
    quat rotation = glm::angleAxis(angle, right);
    vec3 newForward = rotation * forward;
#endif
    
    m_target = m_position + newForward * length(m_target - m_position);
    markViewDirty();
}

void CameraController::roll(float angle) {
    // Rotate the up vector around the forward vector
    vec3 forward = getForwardVector();
    
#ifdef __APPLE__
    // Use SIMD quaternion rotation on Apple platforms
    simd_quatf rotation = simd_quaternion(angle, forward);
    m_up = simd_act(rotation, m_up);
#else
    // Use GLM quaternion rotation on other platforms
    quat rotation = glm::angleAxis(angle, forward);
    m_up = rotation * m_up;
#endif
    
    markViewDirty();
}

void CameraController::update(float deltaTime) {
    // Basic camera update - can be extended for smooth interpolation
    // For now, just ensure matrices are up to date
    if (m_viewDirty) {
        updateViewMatrix();
    }
    if (m_projectionDirty) {
        updateProjectionMatrix();
    }
}

vec3 CameraController::getForwardVector() const {
    return normalize(m_target - m_position);
}

vec3 CameraController::getRightVector() const {
    return normalize(cross(getForwardVector(), m_up));
}

Ray CameraController::screenToWorldRay(float screenX, float screenY, int screenWidth, int screenHeight) const {
    // Convert screen coordinates to normalized device coordinates (-1 to 1)
    float x = (2.0f * screenX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * screenY) / screenHeight; // Flip Y coordinate
    
    // Create points in clip space
    vec4 rayClipNear = make_vec4(x, y, -1.0f, 1.0f); // Near plane
    vec4 rayClipFar = make_vec4(x, y, 1.0f, 1.0f);   // Far plane
    
    // Transform to world space
    mat4 invViewProj = inverse(getViewProjectionMatrix());
    
#ifdef __APPLE__
    // Use SIMD matrix multiplication on Apple platforms
    vec4 rayWorldNear = invViewProj * rayClipNear;
    vec4 rayWorldFar = invViewProj * rayClipFar;
    
    // Perspective divide
    rayWorldNear = rayWorldNear / rayWorldNear.w;
    rayWorldFar = rayWorldFar / rayWorldFar.w;
    
    vec3 rayOrigin = make_vec3(rayWorldNear.x, rayWorldNear.y, rayWorldNear.z);
    vec3 rayEnd = make_vec3(rayWorldFar.x, rayWorldFar.y, rayWorldFar.z);
#else
    // Use GLM matrix multiplication on other platforms
    vec4 rayWorldNear = invViewProj * rayClipNear;
    vec4 rayWorldFar = invViewProj * rayClipFar;
    
    // Perspective divide
    rayWorldNear /= rayWorldNear.w;
    rayWorldFar /= rayWorldFar.w;
    
    vec3 rayOrigin = vec3(rayWorldNear);
    vec3 rayEnd = vec3(rayWorldFar);
#endif
    
    vec3 rayDirection = normalize(rayEnd - rayOrigin);
    
    return Ray(rayOrigin, rayDirection);
}

void CameraController::updateViewMatrix() const {
    m_viewMatrix = lookAt(m_position, m_target, m_up);
    m_viewDirty = false;
}

void CameraController::updateProjectionMatrix() const {
    m_projectionMatrix = perspective(m_fov, m_aspect, m_near, m_far);
    m_projectionDirty = false;
}

} // namespace FinalStorm