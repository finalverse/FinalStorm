// CameraController.cpp - Camera control implementation
#include "Scene/CameraController.h"
#include "Core/World/Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace FinalStorm {

CameraController::CameraController(Entity* cameraEntity) 
    : m_camera(cameraEntity),
      m_position(0, 0, 0),
      m_lookAt(0, 0, 0),
      m_fov(60.0f) {
    
    if (m_camera) {
        m_position = m_camera->transform.position;
    }
}

void CameraController::update(float deltaTime) {
    if (!m_camera) return;
    
    updateTransition(deltaTime);
}

void CameraController::setPosition(const glm::vec3& position) {
    m_position = position;
    if (m_camera) {
        m_camera->transform.position = position;
    }
}

void CameraController::setTarget(const glm::vec3& position, const glm::vec3& lookAt, float fov) {
    m_position = position;
    m_lookAt = lookAt;
    m_fov = fov;
    
    if (m_camera) {
        m_camera->transform.position = position;
        // Calculate rotation to look at target
        glm::vec3 forward = glm::normalize(lookAt - position);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::cross(right, forward);
        
        glm::mat4 lookMatrix(1.0f);
        lookMatrix[0] = glm::vec4(right, 0.0f);
        lookMatrix[1] = glm::vec4(up, 0.0f);
        lookMatrix[2] = glm::vec4(-forward, 0.0f);
        
        m_camera->transform.rotation = glm::quat_cast(lookMatrix);
    }
}

void CameraController::transitionTo(const glm::vec3& position, const glm::vec3& lookAt, float duration, float fov) {
    m_targetPosition = position;
    m_targetLookAt = lookAt;
    m_targetFov = fov;
    m_transitionTime = 0.0f;
    m_transitionDuration = duration;
    m_isTransitioning = true;
}

void CameraController::orbitAround(const glm::vec3& center, float radius, float height) {
    // TODO: Implement orbital movement
}

void CameraController::followPath(const std::vector<glm::vec3>& path, float duration) {
    if (path.empty()) return;
    
    m_path = path;
    m_currentPathIndex = 0;
    m_pathProgress = 0.0f;
    m_pathDuration = duration;
}

void CameraController::handleMouseDrag(float deltaX, float deltaY) {
    m_orbitYaw += deltaX;
    m_orbitPitch += deltaY;
    m_orbitPitch = glm::clamp(m_orbitPitch, -1.5f, 1.5f);
}

void CameraController::handleScroll(float delta) {
    m_orbitDistance *= (1.0f - delta * 0.1f);
    m_orbitDistance = glm::clamp(m_orbitDistance, 2.0f, 50.0f);
}

void CameraController::updateTransition(float deltaTime) {
    if (!m_isTransitioning) return;
    
    m_transitionTime += deltaTime;
    float t = m_transitionTime / m_transitionDuration;
    
    if (t >= 1.0f) {
        t = 1.0f;
        m_isTransitioning = false;
    }
    
    // Ease in-out cubic
    t = easeInOutCubic(t);
    
    // Interpolate position
    glm::vec3 newPos = glm::mix(m_position, m_targetPosition, t);
    glm::vec3 newLookAt = glm::mix(m_lookAt, m_targetLookAt, t);
    float newFov = glm::mix(m_fov, m_targetFov, t);
    
    setTarget(newPos, newLookAt, newFov);
}

float CameraController::easeInOutCubic(float t) {
    return t < 0.5f 
        ? 4.0f * t * t * t 
        : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

} // namespace FinalStorm