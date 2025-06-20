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
using quaternion = quat;

struct Transform {
    // Local transform state
    float3 position;
    quaternion rotation;
    float3 scale;

    // Cached matrix and dirty flag
    mutable float4x4 matrix;
    mutable bool isDirty;

    Transform();
    Transform(const float3& pos, const quaternion& rot, const float3& scl);

    // Modification helpers
    void setPosition(const float3& pos);
    void setRotation(const quaternion& rot);
    void setScale(const float3& scl);

    void translate(const float3& delta);
    void rotate(const quaternion& rot);
    void rotateEuler(const float3& eulerAngles);
    void setRotationFromEuler(float pitch, float yaw, float roll);

    // Queries
    float4x4 getMatrix() const;
    float3 getForward() const;
    float3 getRight() const;
    float3 getUp() const;
    void lookAt(const float3& target, const float3& up = float3(0, 1, 0));

    // Interpolation
    static Transform lerp(const Transform& a, const Transform& b, float t);

private:
    void updateMatrix() const;
    float4x4 quaternionToMatrix(const quaternion& q) const;
    quaternion matrixToQuaternion(const float4x4& m) const;
};

} // namespace FinalStorm
