//
//  Math.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include <simd/simd.h>
#include <cmath>
#include "Core/Math/MathTypes.h"

namespace FinalStorm {

// Forward declaration
struct Transform;

class Camera {
public:
    Camera();
    
    void setPosition(const vec3& pos) { m_position = pos; updateViewMatrix(); }
    void setTarget(const vec3& target) { m_target = target; updateViewMatrix(); }
    void setUp(const vec3& up) { m_up = up; updateViewMatrix(); }
    
    void setPerspective(float fov, float aspect, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    
    const mat4& getViewMatrix() const { return m_viewMatrix; }
    const mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    mat4 getViewProjectionMatrix() const { return simd_mul(m_projectionMatrix, m_viewMatrix); }
    
    const vec3& getPosition() const { return m_position; }
    const vec3& getTarget() const { return m_target; }
    
private:
    void updateViewMatrix();
    
    vec3 m_position;
    vec3 m_target;
    vec3 m_up;
    
    mat4 m_viewMatrix;
    mat4 m_projectionMatrix;
};

namespace Math {

// Math utilities
mat4 matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ);
mat4 matrix_look_at_right_hand(vec3 eye, vec3 center, vec3 up);
mat4 matrix_identity();
mat4 matrix_translation(vec3 translation);
mat4 matrix_rotation(quat quaternion);
mat4 matrix_scale(vec3 scale);
mat3 matrix3x3_upper_left(const mat4& m);
mat4 matrix_orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

// Helper functions that forward to platform-specific implementations
using ::FinalStorm::radians;
using ::FinalStorm::degrees;
using ::FinalStorm::clamp;
using ::FinalStorm::lerp;

// Additional math utilities
float smoothstep(float edge0, float edge1, float x);

} // namespace Math

} // namespace FinalStorm