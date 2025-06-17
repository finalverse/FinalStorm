// Common.metal - Shared shader functions and structures

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Common structures
struct CameraData {
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 viewProjMatrix;
    float3 position;
    float time;
};

struct ModelData {
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float4 color;
    float metallic;
    float roughness;
    float emission;
    float opacity;
};

// Common functions
float3 unpackNormal(float3 packedNormal) {
    return normalize(packedNormal * 2.0 - 1.0);
}

float linearizeDepth(float depth, float near, float far) {
    return (2.0 * near) / (far + near - depth * (far - near));
}

// Noise functions
float hash(float2 p) {
    return fract(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float noise(float2 p) {
    float2 i = floor(p);
    float2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}
