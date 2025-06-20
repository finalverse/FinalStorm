//
//  Transform.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include "Core/Math/Math.h"
#include "Core/Math/MathTypes.h"

namespace FinalStorm {

// Simple transform component storing position, orientation and scale.
// This mirrors the implementation in Transform.cpp which provides
// basic manipulation helpers and matrix caching.

struct Transform {
    // Local transform state
    vec3 position;
    quat rotation;
    vec3 scale;

    // Cached matrix and dirty flag
    mutable mat4 matrix;
    mutable bool isDirty;

    Transform();
    Transform(const vec3& pos, const quat& rot, const vec3& scl);

    // Modification helpers
    void setPosition(const vec3& pos);
    void setRotation(const quat& rot);
    void setScale(const vec3& scl);

    void translate(const vec3& delta);
    void rotate(const quat& rot);
    void rotateEuler(const vec3& eulerAngles);
    void setRotationFromEuler(float pitch, float yaw, float roll);

    // Queries
    mat4 getMatrix() const;
    vec3 getForward() const;
    vec3 getRight() const;
    vec3 getUp() const;
    void lookAt(const vec3& target, const vec3& up = make_vec3(0, 1, 0));

    // Interpolation
    static Transform lerp(const Transform& a, const Transform& b, float t);

private:
    void updateMatrix() const;
    quat matrixToQuaternion(const mat4& m) const;
};

} // namespace FinalStorm