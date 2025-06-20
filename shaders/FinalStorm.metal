// shaders/FinalStorm.metal
// Main shader library for FinalStorm
// Contains vertex and fragment shaders for rendering 3D objects

#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

// Vertex input structure
struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texcoord [[attribute(2)]];
};

// Vertex output structure
struct VertexOut {
    float4 position [[position]];
    float3 worldPos;
    float3 normal;
    float2 texcoord;
    float4 color;
};

// Uniform buffer structure
struct Uniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 normalMatrix;
    float3 cameraPosition;
    float time;
};

// Material properties
struct Material {
    float3 ambient;
    float3 diffuse;
    float3 specular;
    float3 emission;
    float shininess;
    float opacity;
};

// Basic vertex shader
vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                            constant Uniforms& uniforms [[buffer(1)]],
                            uint vertexID [[vertex_id]]) {
    VertexOut out;
    
    // Transform position to world space
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    
    // Transform to clip space
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    // Transform normal
    out.normal = normalize((uniforms.normalMatrix * float4(in.normal, 0.0)).xyz);
    
    // Pass through texcoord
    out.texcoord = in.texcoord;
    
    // Add some color variation based on position
    out.color = float4(0.5 + 0.5 * sin(worldPos.xyz * 0.1 + uniforms.time), 1.0);
    
    return out;
}

// Basic fragment shader with lighting
fragment float4 fragment_main(VertexOut in [[stage_in]],
                            constant Uniforms& uniforms [[buffer(1)]]) {
    // Normalize interpolated normal
    float3 normal = normalize(in.normal);
    
    // Calculate view direction
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Simple directional light
    float3 lightDir = normalize(float3(1, 1, 1));
    float3 lightColor = float3(1, 1, 1);
    
    // Ambient
    float3 ambient = float3(0.1, 0.1, 0.15);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diff * lightColor;
    
    // Specular
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = spec * lightColor;
    
    // Combine lighting
    float3 result = ambient + diffuse * in.color.rgb + specular * 0.5;
    
    // Add some glow effect
    float glow = pow(1.0 - dot(viewDir, normal), 2.0) * 0.5;
    result += float3(0.2, 0.5, 1.0) * glow;
    
    return float4(result, 1.0);
}

// Service visualization shaders
vertex VertexOut vertex_service(VertexIn in [[stage_in]],
                               constant Uniforms& uniforms [[buffer(1)]],
                               constant float& glowIntensity [[buffer(2)]]) {
    VertexOut out;
    
    // Animate vertex position
    float3 animatedPos = in.position;
    animatedPos += in.normal * sin(uniforms.time * 2.0 + in.position.y * 3.0) * 0.05 * glowIntensity;
    
    // Standard transformations
    float4 worldPos = uniforms.modelMatrix * float4(animatedPos, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    out.normal = normalize((uniforms.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texcoord = in.texcoord;
    
    // Service-specific coloring
    out.color = float4(0.0, 0.8, 1.0, 1.0) * glowIntensity;
    
    return out;
}

// Holographic UI shader
fragment float4 fragment_holographic(VertexOut in [[stage_in]],
                                   constant Uniforms& uniforms [[buffer(1)]]) {
    // Base color
    float3 baseColor = float3(0.1, 0.8, 1.0);
    
    // Scanline effect
    float scanline = sin(in.position.y * 0.5 + uniforms.time * 2.0) * 0.5 + 0.5;
    scanline = pow(scanline, 8.0);
    
    // Edge glow
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    float rim = 1.0 - dot(viewDir, normalize(in.normal));
    rim = pow(rim, 2.0);
    
    // Combine effects
    float3 color = baseColor * (0.5 + 0.5 * scanline) + baseColor * rim * 2.0;
    
    // Add transparency
    float alpha = 0.7 + rim * 0.3;
    
    return float4(color, alpha);
}

// Particle shader
struct ParticleVertexIn {
    float3 position [[attribute(0)]];
    float2 texcoord [[attribute(1)]];
    float3 instancePosition [[attribute(2)]];
    float4 instanceColor [[attribute(3)]];
    float instanceSize [[attribute(4)]];
};

vertex VertexOut vertex_particle(ParticleVertexIn in [[stage_in]],
                                constant Uniforms& uniforms [[buffer(1)]]) {
    VertexOut out;
    
    // Billboard the particle
    float3 cameraRight = normalize(uniforms.viewMatrix[0].xyz);
    float3 cameraUp = normalize(uniforms.viewMatrix[1].xyz);
    
    float3 worldPos = in.instancePosition + 
                     (cameraRight * in.position.x + cameraUp * in.position.y) * in.instanceSize;
    
    out.worldPos = worldPos;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * float4(worldPos, 1.0);
    out.normal = float3(0, 0, 1); // Facing camera
    out.texcoord = in.texcoord;
    out.color = in.instanceColor;
    
    return out;
}

fragment float4 fragment_particle(VertexOut in [[stage_in]]) {
    // Radial gradient
    float dist = length(in.texcoord - 0.5) * 2.0;
    float alpha = 1.0 - smoothstep(0.0, 1.0, dist);
    
    // Soft particles
    alpha *= in.color.a;
    
    return float4(in.color.rgb, alpha);
}