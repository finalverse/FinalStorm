// FirstScene.metal - Shaders for FinalStorm's First Scene

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// FirstScene-specific uniforms
struct FirstSceneUniforms {
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

// Central crystal shader for FinalStorm First Scene
fragment float4 firstscene_crystal_fragment(VertexOut in [[stage_in]],
                                          constant FirstSceneUniforms& uniforms [[buffer(0)]],
                                          texture2d<float> noiseTexture [[texture(0)]],
                                          sampler noiseSampler [[sampler(0)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Crystal refraction effect
    float3 refractDir = refract(-viewDir, normal, 0.9);
    float3 reflectDir = reflect(-viewDir, normal);
    
    // Sample noise for internal structure
    float noiseVal = noiseTexture.sample(noiseSampler, in.texCoord + uniforms.time * 0.05).r;
    
    // Base crystal color with internal light
    float3 crystalColor = uniforms.baseColor.rgb;
    crystalColor += uniforms.glowColor.rgb * noiseVal * uniforms.glowIntensity;
    
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
fragment float4 firstscene_energy_ring_fragment(VertexOut in [[stage_in]],
                                               constant FirstSceneUniforms& uniforms [[buffer(0)]]) {
    
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
fragment float4 firstscene_service_orb_fragment(VertexOut in [[stage_in]],
                                               constant FirstSceneUniforms& uniforms [[buffer(0)]],
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
    float pulse = sin(uniforms.pulsePhase + float(serviceType) * 0.5) * 0.5 + 0.5;
    color += uniforms.glowColor.rgb * pulse * uniforms.energyLevel * 0.3;
    
    float alpha = uniforms.baseColor.a * (0.8 + fresnel * 0.2);
    
    return float4(color, alpha);
}

// Environment background shader
vertex VertexOut firstscene_environment_vertex(VertexIn in [[stage_in]],
                                              constant FirstSceneUniforms& uniforms [[buffer(0)]],
                                              uint instanceID [[instance_id]]) {
    VertexOut out;
    
    // Scale and offset for different layers
    float layerScale = 1.0 + float(instanceID) * 0.2;
    float3 layerOffset = float3(
        sin(float(instanceID) * 1.234) * 10.0,
        cos(float(instanceID) * 2.345) * 5.0,
        float(instanceID) * -5.0
    );
    
    float3 pos = in.position * layerScale + layerOffset;
    float4 worldPos = uniforms.modelMatrix * float4(pos, 1.0);
    
    out.worldPos = worldPos.xyz;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    out.worldNormal = in.normal;
    out.texCoord = in.texCoord;
    out.fogFactor = 0.0; // No fog for environment
    
    // Initialize other fields
    out.tangentViewPos = float3(0.0);
    out.tangentLightPos = float3(0.0);
    out.tangentFragPos = float3(0.0);
    out.shadowCoord = float4(0.0);
    
    return out;
}

fragment float4 firstscene_environment_fragment(VertexOut in [[stage_in]],
                                               constant FirstSceneUniforms& uniforms [[buffer(0)]],
                                               texture2d<float> noiseTexture [[texture(0)]],
                                               sampler noiseSampler [[sampler(0)]]) {
    
    // Multi-octave noise for clouds
    float2 uv = in.texCoord * 2.0;
    float noise1 = noiseTexture.sample(noiseSampler, uv + uniforms.time * 0.01).r;
    float noise2 = noiseTexture.sample(noiseSampler, uv * 2.5 - uniforms.time * 0.02).r;
    float noise3 = noiseTexture.sample(noiseSampler, uv * 5.0 + uniforms.time * 0.015).r;
    
    float cloudDensity = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
    cloudDensity = smoothstep(0.2, 0.8, cloudDensity);
    
    // Color gradient
    float3 color = mix(uniforms.primaryColor, uniforms.secondaryColor, cloudDensity);
    color *= cloudDensity;
    
    // Distance fade
    float distance = length(in.worldPos - uniforms.cameraPosition);
    float fade = 1.0 - smoothstep(50.0, 200.0, distance);
    
    // Subtle animation
    float pulse = sin(uniforms.time * 0.5 + distance * 0.01) * 0.1 + 0.9;
    color *= pulse;
    
    float alpha = cloudDensity * fade * 0.3;
    
    return float4(color, alpha);
}

// Connection beam shader
fragment float4 firstscene_connection_beam_fragment(VertexOut in [[stage_in]],
                                                   constant FirstSceneUniforms& uniforms [[buffer(0)]]) {
    
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
fragment float4 firstscene_holographic_ui_fragment(VertexOut in [[stage_in]],
                                                   constant FirstSceneUniforms& uniforms [[buffer(0)]],
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

// Particle shaders for environment effects
vertex ParticleVertexOut firstscene_particle_vertex(ParticleVertexIn in [[stage_in]],
                                                   constant FirstSceneUniforms& camera [[buffer(0)]]) {
    ParticleVertexOut out;
    
    float4 viewPos = camera.viewMatrix * float4(in.position, 1.0);
    out.position = camera.projectionMatrix * viewPos;
    
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

fragment float4 firstscene_particle_fragment(ParticleVertexOut in [[stage_in]],
                                           float2 pointCoord [[point_coord]]) {
    
    // Rotate point coordinates
    float2 coord = pointCoord * 2.0 - 1.0;
    float cosR = cos(in.rotation);
    float sinR = sin(in.rotation);
    float2 rotated = float2(
        coord.x * cosR - coord.y * sinR,
        coord.x * sinR + coord.y * cosR
    );
    
    float r = length(coord);
    if (r > 1.0) discard_fragment();
    
    // Soft particle edges
    float alpha = 1.0 - smoothstep(0.5, 1.0, r);
    
    // Glow effect
    float glow = exp(-r * 3.0);
    float3 color = in.color.rgb * (1.0 + glow);
    
    return float4(color, alpha * in.color.a);
}