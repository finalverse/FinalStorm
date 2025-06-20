// src/Core/Math/Camera.h
// Camera class for 3D viewing
// Manages view and projection matrices

#pragma once
#include "Core/Math/MathTypes.h"
#include "Core/Math/Transform.h"

namespace FinalStorm {

class Camera {
public:
    Camera();
    ~Camera() = default;
    
    // Position and orientation
    void setPosition(const vec3& position);
    void setTarget(const vec3& target);
    void setUp(const vec3& up);
    void lookAt(const vec3& position, const vec3& target, const vec3& up);
    
    // Projection parameters
    void setFOV(float fovRadians);
    void setAspectRatio(float aspectRatio);
    void setNearFar(float nearPlane, float farPlane);
    
    // Getters
    vec3 getPosition() const { return position; }
    vec3 getTarget() const { return target; }
    vec3 getUp() const { return up; }
    float getFOV() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    
    // Matrix getters
    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;
    mat4 getViewProjectionMatrix() const;
    
    // Utility
    vec3 getForward() const;
    vec3 getRight() const;
    vec3 getUp() const;
    
    // Movement
    void move(const vec3& delta);
    void rotate(float yaw, float pitch);
    void zoom(float delta);
    
    // Frustum culling
    bool isInFrustum(const vec3& point) const;
    
private:
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;
    
    // Camera properties
    vec3 position;
    vec3 target;
    vec3 up;
    
    // Projection properties
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    // Cached matrices
    mutable mat4 viewMatrix;
    mutable mat4 projectionMatrix;
    mutable bool viewDirty;
    mutable bool projectionDirty;
};

} // namespace FinalStorm