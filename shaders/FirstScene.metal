// FirstScene.metal - Shaders for The Nexus first scene

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// Nexus-specific structures
struct NexusUniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 normalMatrix;
    
    float3 cameraPosition;
    float time;
    
    float4 baseColor;
    float4 glowColor;
    float glowIntensity;
    float pulsePhase;
    float connectionStrength;
    
    float3 primaryColor;
    float3 secondaryColor;
    float energyLevel;
};

// Central Nexus crystal shader
fragment float4 nexus_crystal_fragment(VertexOut in [[stage_in]],
                                      constant NexusUniforms& uniforms [[buffer(0)]],
                                      texture2d<float> noiseTexture [[texture(0)]],
                                      sampler noiseSampler [[sampler(0)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Crystal refraction effect
    float3 refractDir = refract(-viewDir, normal, 0.9);
    float3 reflectDir = reflect(-viewDir, normal);
    
    // Sample noise for internal structure
    float noise = noiseTexture.sample(noiseSampler, in.texCoord + uniforms.time * 0.05).r;
    
    // Base crystal color with internal light
    float3 crystalColor = uniforms.baseColor.rgb;
    crystalColor += uniforms.glowColor.rgb * noise * uniforms.glowIntensity;
    
    // Fresnel effect for rim lighting
    float fresnel = pow(1.0 - saturate(dot(normal, viewDir)), 2.0);
    crystalColor += uniforms.glowColor.rgb * fresnel * 2.0;
    
    // Pulse effect
    float pulse = sin(uniforms.pulsePhase) * 0.5 + 0.5;
    crystalColor += uniforms.glowColor.rgb * pulse * uniforms.glowIntensity;
    
    // Energy veins inside crystal
    float3 veinPos = in.worldPos * 2.0 + uniforms.time * 0.1;
    float vein = sin(veinPos.x * 10.0) * sin(veinPos.y * 10.0) * sin(veinPos.z * 10.0);
    vein = smoothstep(0.7, 0.9, vein);
    crystalColor += uniforms.secondaryColor * vein * uniforms.connectionStrength;
    
    // Transparency with depth
    float alpha = uniforms.baseColor.a;
    alpha *= (1.0 - fresnel * 0.3);
    
    return float4(crystalColor, alpha);
}

// Energy ring shader
fragment float4 energy_ring_fragment(VertexOut in [[stage_in]],
                                   constant NexusUniforms& uniforms [[buffer(0)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Ring energy flow
    float flow = sin(in.texCoord.x * 20.0 - uniforms.time * 3.0) * 0.5 + 0.5;
    flow *= smoothstep(0.0, 0.1, in.texCoord.y) * smoothstep(1.0, 0.9, in.texCoord.y);
    
    // Color with energy flow
    float3 color = uniforms.glowColor.rgb * flow;
    color += uniforms.secondaryColor * (1.0 - flow) * 0.5;
    
    // Rim lighting
    float rim = 1.0 - saturate(dot(normal, viewDir));
    color += uniforms.glowColor.rgb * pow(rim, 2.0);
    
    // Connection strength modulation
    float intensity = uniforms.glowIntensity * uniforms.connectionStrength;
    color *= intensity;
    
    return float4(color, flow * intensity);
}

// Service orb shader
fragment float4 service_orb_fragment(VertexOut in [[stage_in]],
                                   constant NexusUniforms& uniforms [[buffer(0)]],
                                   constant int& serviceType [[buffer(1)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Base service color
    float3 color = uniforms.baseColor.rgb;
    
    // Inner glow based on activity
    float3 center = float3(0.5) - in.texCoord.xyx;
    float dist = length(center);
    float innerGlow = 1.0 - smoothstep(0.0, 0.5, dist);
    color += uniforms.glowColor.rgb * innerGlow * uniforms.energyLevel;
    
    // Service-specific patterns
    float pattern = 0.0;
    switch (serviceType) {
        case 0: // API Gateway - Grid pattern
            pattern = step(0.9, sin(in.texCoord.x * 30.0) * sin(in.texCoord.y * 30.0));
            break;
        case 1: // AI Service - Neural pattern
            pattern = noise(in.texCoord * 10.0 + uniforms.time * 0.5);
            pattern = smoothstep(0.4, 0.6, pattern);
            break;
        case 2: // Audio Service - Wave pattern
            pattern = sin(in.texCoord.x * 20.0 + sin(in.texCoord.y * 5.0 + uniforms.time * 2.0));
            pattern = smoothstep(-0.5, 0.5, pattern);
            break;
        default:
            pattern = 0.0;
    }
    
    color = mix(color, uniforms.secondaryColor, pattern * 0.3);
    
    // Holographic effect
    float scanline = sin(in.worldPos.y * 50.0 + uniforms.time * 2.0) * 0.04 + 0.96;
    color *= scanline;
    
    // Fresnel rim
    float fresnel = pow(1.0 - saturate(dot(normal, viewDir)), 1.5);
    color += uniforms.glowColor.rgb * fresnel;
    
    // Activity pulse
    float pulse = sin(uniforms.pulsePhase + serviceType * 0.5) * 0.5 + 0.5;
    color += uniforms.glowColor.rgb * pulse * uniforms.energyLevel * 0.3;
    
    float alpha = uniforms.baseColor.a * (0.8 + fresnel * 0.2);
    
    return float4(color, alpha);
}

// Nebula background shader
vertex VertexOut nebula_vertex(VertexIn in [[stage_in]],
                              constant NexusUniforms& uniforms [[buffer(0)]],
                              uint instanceID [[instance_id]]) {
    VertexOut out;
    
    // Scale and offset for different nebula layers
    float layerScale = 1.0 + instanceID * 0.2;
    float3 layerOffset = float3(
        sin(instanceID * 1.234) * 10.0,
        cos(instanceID * 2.345) * 5.0,
        instanceID * -5.0
    );
    
    float3 pos = in.position * layerScale + layerOffset;
    float4 worldPos = uniforms.modelMatrix * float4(pos, 1.0);
    
    out.worldPos = worldPos.xyz;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    out.worldNormal = in.normal;
    out.texCoord = in.texCoord;
    
    return out;
}

fragment float4 nebula_fragment(VertexOut in [[stage_in]],
                               constant NexusUniforms& uniforms [[buffer(0)]],
                               texture2d<float> noiseTexture [[texture(0)]],
                               sampler noiseSampler [[sampler(0)]]) {
    
    // Multi-octave noise for nebula clouds
    float2 uv = in.texCoord * 2.0;
    float noise1 = noiseTexture.sample(noiseSampler, uv + uniforms.time * 0.01).r;
    float noise2 = noiseTexture.sample(noiseSampler, uv * 2.5 - uniforms.time * 0.02).r;
    float noise3 = noiseTexture.sample(noiseSampler, uv * 5.0 + uniforms.time * 0.015).r;
    
    float nebula = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
    nebula = smoothstep(0.2, 0.8, nebula);
    
    // Color gradient
    float3 color = mix(uniforms.primaryColor, uniforms.secondaryColor, nebula);
    color *= nebula;
    
    // Distance fade
    float distance = length(in.worldPos - uniforms.cameraPosition);
    float fade = 1.0 - smoothstep(50.0, 200.0, distance);
    
    // Subtle animation
    float pulse = sin(uniforms.time * 0.5 + distance * 0.01) * 0.1 + 0.9;
    color *= pulse;
    
    float alpha = nebula * fade * 0.3;
    
    return float4(color, alpha);
}

// Connection beam shader
fragment float4 connection_beam_fragment(VertexOut in [[stage_in]],
                                       constant NexusUniforms& uniforms [[buffer(0)]]) {
    
    // Energy flow along beam
    float flow = fract(in.texCoord.y - uniforms.time * 0.5);
    flow = smoothstep(0.0, 0.1, flow) * smoothstep(1.0, 0.9, flow);
    
    // Beam core
    float beamCore = 1.0 - smoothstep(0.0, 0.5, abs(in.texCoord.x - 0.5) * 2.0);
    
    // Pulsing energy packets
    float packets = sin(in.texCoord.y * 20.0 - uniforms.time * 10.0);
    packets = smoothstep(0.5, 1.0, packets) * flow;
    
    // Color
    float3 color = uniforms.glowColor.rgb * beamCore;
    color += uniforms.secondaryColor * packets;
    
    // Connection strength affects intensity
    color *= uniforms.connectionStrength;
    
    float alpha = (beamCore + packets * 0.5) * uniforms.connectionStrength;
    
    return float4(color, alpha);
}

// Holographic UI shader
fragment float4 holographic_ui_fragment(VertexOut in [[stage_in]],
                                       constant NexusUniforms& uniforms [[buffer(0)]],
                                       texture2d<float> uiTexture [[texture(0)]],
                                       sampler uiSampler [[sampler(0)]]) {
    
    float4 texColor = uiTexture.sample(uiSampler, in.texCoord);
    
    // Holographic distortion
    float2 distortedUV = in.texCoord;
    distortedUV.x += sin(in.texCoord.y * 30.0 + uniforms.time * 5.0) * 0.002;
    
    // Scanlines
    float scanline = sin(in.worldPos.y * 100.0 + uniforms.time * 10.0) * 0.03 + 0.97;
    
    // Chromatic aberration
    float3 color;
    color.r = uiTexture.sample(uiSampler, distortedUV + float2(0.002, 0)).r;
    color.g = uiTexture.sample(uiSampler, distortedUV).g;
    color.b = uiTexture.sample(uiSampler, distortedUV - float2(0.002, 0)).b;
    
    color *= uniforms.baseColor.rgb * scanline;
    
    // Edge glow
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    float rim = 1.0 - saturate(dot(normal, viewDir));
    rim = pow(rim, 2.0);
    
    color += uniforms.glowColor.rgb * rim;
    
    // Flicker effect
    float flicker = sin(uniforms.time * 30.0) * 0.02 + 0.98;
    color *= flicker;
    
    float alpha = texColor.a * uniforms.baseColor.a * (0.8 + rim * 0.2);
    
    return float4(color, alpha);
}

// Welcome sequence fade shader
kernel void welcome_fade(texture2d<float, access::read> inTexture [[texture(0)]],
                        texture2d<float, access::write> outTexture [[texture(1)]],
                        constant float& fadeAlpha [[buffer(0)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    float4 color = inTexture.read(gid);
    
    // Vignette effect
    float2 uv = float2(gid) / float2(inTexture.get_width(), inTexture.get_height());
    float2 center = uv - 0.5;
    float vignette = 1.0 - length(center) * 1.4;
    vignette = smoothstep(0.0, 1.0, vignette);
    
    // Apply fade
    color.rgb *= (1.0 - fadeAlpha);
    color.rgb *= vignette;
    
    outTexture.write(color, gid);
}

// Particle shader for environment
struct EnvironmentParticle {
    float3 position [[attribute(0)]];
    float size [[attribute(1)]];
    float4 color [[attribute(2)]];
    float life [[attribute(3)]];
    float type [[attribute(4)]]; // 0: star, 1: dust, 2: sparkle
};

vertex ParticleVertexOut environment_particle_vertex(EnvironmentParticle in [[stage_in]],
                                                   constant CameraData& camera [[buffer(0)]]) {
    ParticleVertexOut out;
    
    float4 viewPos = camera.viewMatrix * float4(in.position, 1.0);
    out.position = camera.projMatrix * viewPos;
    
    // Size based on type and distance
    float distance = length(viewPos.xyz);
    float baseSize = in.size;
    
    switch (int(in.type)) {
        case 0: // Star - constant size
            out.pointSize = baseSize * 2.0;
            break;
        case 1: // Dust - larger, distance-based
            out.pointSize = baseSize * 1000.0 / distance;
            break;
        case 2: // Sparkle - animated size
            out.pointSize = baseSize * (1.0 + sin(camera.time * 10.0 + in.position.x) * 0.5) * 100.0 / distance;
            break;
        default:
            out.pointSize = baseSize * 100.0 / distance;
    }
    
    out.pointSize = clamp(out.pointSize, 1.0, 128.0);
    out.color = in.color;
    out.rotation = 0.0;
    
    return out;
}

fragment float4 environment_particle_fragment(ParticleVertexOut in [[stage_in]],
                                            float2 pointCoord [[point_coord]]) {
    
    float2 coord = pointCoord * 2.0 - 1.0;
    float r = length(coord);
    
    if (r > 1.0) discard_fragment();
    
    // Soft particle edges
    float alpha = 1.0 - smoothstep(0.5, 1.0, r);
    
    // Glow effect
    float glow = exp(-r * 3.0);
    float3 color = in.color.rgb * (1.0 + glow);
    
    return float4(color, alpha * in.color.a);
}