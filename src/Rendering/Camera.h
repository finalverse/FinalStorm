// Rendering/Camera.h
#pragma once

#include "Core/Math/MathTypes.h"
#include "Core/Transform.h"

namespace FinalStorm {

enum class ProjectionType {
    Perspective,
    Orthographic
};

class Camera {
public:
    Camera();
    virtual ~Camera() = default;
    
    // Transform
    Transform& getTransform() { return m_transform; }
    const Transform& getTransform() const { return m_transform; }
    
    void setPosition(const vec3& position) { m_transform.setPosition(position); }
    vec3 getPosition() const { return m_transform.getPosition(); }
    
    void lookAt(const vec3& target, const vec3& up = vec3(0, 1, 0)) {
        m_transform.lookAt(target, up);
    }
    
    // Projection
    void setPerspective(float fov, float aspect, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far);
    
    ProjectionType getProjectionType() const { return m_projectionType; }
    
    // Matrices
    const mat4& getViewMatrix() const;
    const mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    mat4 getViewProjectionMatrix() const { return m_projectionMatrix * getViewMatrix(); }
    
    // Projection parameters
    float getFieldOfView() const { return m_fov; }
    float getAspectRatio() const { return m_aspectRatio; }
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }
    
    // Viewport
    void setViewport(int x, int y, int width, int height);
    void getViewport(int& x, int& y, int& width, int& height) const;
    
    // Utility
    vec3 screenToWorld(const vec2& screenPos, float depth = 0.5f) const;
    vec2 worldToScreen(const vec3& worldPos) const;
    
    // Frustum for culling
    struct Frustum {
        vec4 planes[6]; // Left, Right, Bottom, Top, Near, Far
    };
    
    Frustum getFrustum() const;
    bool isInFrustum(const vec3& point, float radius = 0.0f) const;
    
private:
    Transform m_transform;
    
    // Projection
    ProjectionType m_projectionType = ProjectionType::Perspective;
    mat4 m_projectionMatrix = mat4(1.0f);
    mutable mat4 m_viewMatrix = mat4(1.0f);
    mutable bool m_viewMatrixDirty = true;
    
    // Perspective parameters
    float m_fov = 60.0f;
    float m_aspectRatio = 1.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    
    // Orthographic parameters
    float m_orthoLeft = -1.0f;
    float m_orthoRight = 1.0f;
    float m_orthoBottom = -1.0f;
    float m_orthoTop = 1.0f;
    
    // Viewport
    int m_viewportX = 0;
    int m_viewportY = 0;
    int m_viewportWidth = 1;
    int m_viewportHeight = 1;
    
    void updateProjectionMatrix();
};

// Common camera controllers
class OrbitCamera : public Camera {
public:
    OrbitCamera();
    
    void setTarget(const vec3& target);
    void setDistance(float distance);
    void setAngles(float azimuth, float elevation);
    
    void orbit(float deltaAzimuth, float deltaElevation);
    void zoom(float delta);
    void pan(const vec2& delta);
    
    void update();
    
private:
    vec3 m_target = vec3(0.0f);
    float m_distance = 10.0f;
    float m_azimuth = 0.0f;
    float m_elevation = 30.0f;
    
    float m_minDistance = 1.0f;
    float m_maxDistance = 100.0f;
    float m_minElevation = -89.0f;
    float m_maxElevation = 89.0f;
};

class FlyCamera : public Camera {
public:
    FlyCamera();
    
    void move(const vec3& delta);
    void rotate(float yaw, float pitch);
    
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    
    void update(float deltaTime);
    
private:
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_speed = 10.0f;
    
    vec3 m_velocity = vec3(0.0f);
};

} // namespace FinalStorm