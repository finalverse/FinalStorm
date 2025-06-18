// CameraController.h - Camera control for smooth transitions

#pragma once

#include "World/Entity.h"
#include <glm/glm.hpp>
#include <vector>

namespace FinalStorm {

class CameraController {
public:
    CameraController(Entity* cameraEntity);
    
    void update(float deltaTime);
    
    // Camera positioning
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& position, const glm::vec3& lookAt, float fov = 60.0f);
    void transitionTo(const glm::vec3& position, const glm::vec3& lookAt, float duration, float fov = 60.0f);
    
    // Camera movement
    void orbitAround(const glm::vec3& center, float radius, float height);
    void followPath(const std::vector<glm::vec3>& path, float duration);
    
    // Input handling
    void handleMouseDrag(float deltaX, float deltaY);
    void handleScroll(float delta);
    
    bool isTransitioning() const { return m_isTransitioning; }
    
private:
    Entity* m_camera;
    
    // Current state
    glm::vec3 m_position;
    glm::vec3 m_lookAt;
    float m_fov;
    
    // Target state
    glm::vec3 m_targetPosition;
    glm::vec3 m_targetLookAt;
    float m_targetFov;
    
    // Transition
    bool m_isTransitioning = false;
    float m_transitionTime = 0.0f;
    float m_transitionDuration = 0.0f;
    
    // Orbit controls
    float m_orbitDistance = 10.0f;
    float m_orbitPitch = 0.3f;
    float m_orbitYaw = 0.0f;
    
    void updateTransition(float deltaTime);
    float easeInOutCubic(float t);
};

} // namespace FinalStorm
