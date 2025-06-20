//
// src/Scene/CameraController.h
// 3D Camera management for FinalStorm with platform-agnostic math
// Handles view/projection matrices, movement, and screen-to-world projection
//

#pragma once

#include "../Core/Math/MathTypes.h"

namespace FinalStorm {

struct Ray {
    vec3 origin;
    vec3 direction;
    
    Ray(const vec3& o, const vec3& d) : origin(o), direction(normalize(d)) {}
};

class CameraController {
public:
    CameraController();
    ~CameraController();
    
    // Position and orientation using MathTypes
    void setPosition(const vec3& position);
    void setTarget(const vec3& target);
    void setUpVector(const vec3& up);
    
    vec3 getPosition() const { return m_position; }
    vec3 getTarget() const { return m_target; }
    vec3 getUpVector() const { return m_up; }
    
    // Projection parameters
    void setFieldOfView(float fov);
    void setAspectRatio(float aspect);
    void setNearPlane(float near);
    void setFarPlane(float far);
    
    float getFieldOfView() const { return m_fov; }
    float getAspectRatio() const { return m_aspect; }
    float getNearPlane() const { return m_near; }
    float getFarPlane() const { return m_far; }
    
    // Matrix generation using MathTypes
    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;
    mat4 getViewProjectionMatrix() const;
    
    // Camera movement
    void orbit(float azimuth, float elevation);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);
    void dolly(float delta);
    
    // Fly camera controls
    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void yaw(float angle);
    void pitch(float angle);
    void roll(float angle);
    
    // Updates
    void update(float deltaTime);
    
    // Utilities using MathTypes
    vec3 getForwardVector() const;
    vec3 getRightVector() const;
    Ray screenToWorldRay(float screenX, float screenY, int screenWidth, int screenHeight) const;
    
private:
    // Position and orientation using MathTypes
    vec3 m_position;
    vec3 m_target;
    vec3 m_up;
    
    // Projection parameters
    float m_fov;        // Field of view in radians
    float m_aspect;     // Aspect ratio (width/height)
    float m_near;       // Near plane distance
    float m_far;        // Far plane distance
    
    // Cached matrices using MathTypes
    mutable bool m_viewDirty;
    mutable bool m_projectionDirty;
    mutable mat4 m_viewMatrix;
    mutable mat4 m_projectionMatrix;
    
    void markViewDirty() const { m_viewDirty = true; }
    void markProjectionDirty() const { m_projectionDirty = true; }
    
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;
};

} // namespace FinalStorm