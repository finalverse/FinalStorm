// Particles.metal - Particle system shaders

#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

struct ParticleVertexIn {
    float3 position [[attribute(0)]];
    float size [[attribute(1)]];
    float4 color [[attribute(2)]];
    float life [[attribute(3)]];
    float rotation [[attribute(4)]];
};

struct ParticleVertexOut {
    float4 position [[position]];
    float pointSize [[point_size]];
    float4 color;
    float rotation;
};

vertex ParticleVertexOut particle_vertex(ParticleVertexIn in [[stage_in]],
                                        constant CameraData& camera [[buffer(0)]]) {
    ParticleVertexOut out;
    
    float4 viewPos = camera.viewMatrix * float4(in.position, 1.0);
    out.position = camera.projMatrix * viewPos;
    
    // Point size with distance attenuation
    float distance = length(viewPos.xyz);
    out.pointSize = in.size * 500.0 / distance;
    out.pointSize = clamp(out.pointSize, 1.0, 128.0);
    
    // Fade based on life
    out.color = in.color;
    out.color.a *= smoothstep(0.0, 0.1, in.life);
    
    out.rotation = in.rotation;
    
    return out;
}

fragment float4 particle_fragment(ParticleVertexOut in [[stage_in]],
                                 float2 pointCoord [[point_coord]],
                                 texture2d<float> particleTexture [[texture(0)]],
                                 sampler particleSampler [[sampler(0)]]) {
    
    // Rotate point coordinates
    float2 coord = pointCoord * 2.0 - 1.0;
    float cosR = cos(in.rotation);
    float sinR = sin(in.rotation);
    float2 rotated = float2(
        coord.x * cosR - coord.y * sinR,
        coord.x * sinR + coord.y * cosR
    );
    rotated = rotated * 0.5 + 0.5;
    
    float4 texColor = particleTexture.sample(particleSampler, rotated);
    
    // Glow effect
    float3 color = in.color.rgb * (1.0 + (1.0 - length(coord)) * 0.5);
    
    return float4(color, in.color.a * texColor.a);
}
