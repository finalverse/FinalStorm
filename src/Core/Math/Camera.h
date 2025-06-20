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
    void setPosition(const float3& position);
    void setTarget(const float3& target);
    void setUp(const float3& up);
    void lookAt(const float3& position, const float3& target, const float3& up);
    
    // Projection parameters
    void setFOV(float fovRadians);
    void setAspectRatio(float aspectRatio);
    void setNearFar(float nearPlane, float farPlane);
    
    // Getters
    float3 getPosition() const { return position; }
    float3 getTarget() const { return target; }
    float3 getUp() const { return up; }
    float getFOV() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    
    // Matrix getters
    float4x4 getViewMatrix() const;
    float4x4 getProjectionMatrix() const;
    float4x4 getViewProjectionMatrix() const;
    
    // Utility
    float3 getForward() const;
    float3 getRight() const;
    float3 getUp() const;
    
    // Movement
    void move(const float3& delta);
    void rotate(float yaw, float pitch);
    void zoom(float delta);
    
    // Frustum culling
    bool isInFrustum(const float3& point) const;
    
private:
    void updateViewMatrix() const;
    void updateProjectionMatrix() const;
    
    // Camera properties
    float3 position;
    float3 target;
    float3 up;
    
    // Projection properties
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    // Cached matrices
    mutable float4x4 viewMatrix;
    mutable float4x4 projectionMatrix;
    mutable bool viewDirty;
    mutable bool projectionDirty;
};

} // namespace FinalStorm