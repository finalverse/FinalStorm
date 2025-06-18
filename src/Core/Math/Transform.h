//
//  Transform.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include "Core/Math/Math.h"

namespace FinalStorm {

struct Transform {
    float3 position;
    float4 rotation; // Quaternion (x, y, z, w)
    float3 scale;
    
    Transform() 
        : position(make_float3(0.0f)), 
          rotation(make_float4(0.0f, 0.0f, 0.0f, 1.0f)), 
          scale(make_float3(1.0f)) {}
    
    Transform(const float3& pos, const float4& rot, const float3& scl)
        : position(pos), rotation(rot), scale(scl) {}
    
    // Get the transformation matrix
    float4x4 getMatrix() const;
    
    // Convenience methods
    void translate(const float3& translation) { position += translation; }
    void setRotationFromEuler(float pitch, float yaw, float roll);
    float3 getForward() const;
    float3 getRight() const;
    float3 getUp() const;
    
    // Interpolation
    static Transform lerp(const Transform& a, const Transform& b, float t);
};

} // namespace FinalStorm
