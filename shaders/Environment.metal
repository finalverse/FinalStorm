// Environment.metal - Shaders for environment rendering

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// Skybox vertex shader
vertex VertexOut skybox_vertex(VertexIn in [[stage_in]],
                              constant ModelData& model [[buffer(0)]],
                              constant CameraData& camera [[buffer(1)]]) {
    VertexOut out;
    
    // Remove translation from view matrix for skybox
    float4x4 viewNoTranslation = camera.viewMatrix;
    viewNoTranslation[3][0] = 0.0;
    viewNoTranslation[3][1] = 0.0;
    viewNoTranslation[3][2] = 0.0;
    
    out.worldPos = in.position;
    out.position = camera.projMatrix * viewNoTranslation * model.modelMatrix * float4(in.position, 1.0);
    out.position.z = out.position.w; // Always at far plane
    out.worldNormal = in.normal;
    out.texCoord = in.texCoord;
    out.fogFactor = 0.0;
    
    // Initialize unused fields
    out.tangentViewPos = float3(0.0);
    out.tangentLightPos = float3(0.0);
    out.tangentFragPos = float3(0.0);
    out.shadowCoord = float4(0.0);
    
    return out;
}

// Skybox fragment shader
fragment float4 skybox_fragment(VertexOut in [[stage_in]],
                               constant CameraData& camera [[buffer(0)]],
                               constant ModelData& model [[buffer(1)]],
                               texturecube<float> skyTexture [[texture(0)]],
                               sampler skySampler [[sampler(0)]]) {
    
    float3 texCoord = normalize(in.worldPos);
    float4 skyColor = skyTexture.sample(skySampler, texCoord);
    
    // Apply tint
    skyColor.rgb = mix(skyColor.rgb, skyColor.rgb * model.color.rgb, 0.5);
    
    // Atmospheric glow
    float y = texCoord.y;
    float horizon = 1.0 - abs(y);
    horizon = pow(horizon, 3.0);
    skyColor.rgb += model.color.rgb * horizon * 0.3;
    
    return skyColor;
}

// Grid vertex shader
vertex VertexOut grid_vertex(VertexIn in [[stage_in]],
                            constant ModelData& model [[buffer(0)]],
                            constant CameraData& camera [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = camera.viewProjMatrix * worldPos;
    out.worldNormal = normalize((model.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    out.fogFactor = 0.0; // No fog for grid
    
    // Initialize unused fields
    out.tangentViewPos = float3(0.0);
    out.tangentLightPos = float3(0.0);
    out.tangentFragPos = float3(0.0);
    out.shadowCoord = float4(0.0);
    
    return out;
}

// Grid fragment shader
fragment float4 grid_fragment(VertexOut in [[stage_in]],
                             constant CameraData& camera [[buffer(0)]],
                             constant ModelData& model [[buffer(1)]]) {
    
    // Grid lines
    float2 grid = abs(fract(in.worldPos.xz * 0.5) - 0.5) * 2.0;
    float line = min(grid.x, grid.y);
    line = 1.0 - smoothstep(0.0, 0.02, line);
    
    // Major grid lines
    float2 majorGrid = abs(fract(in.worldPos.xz * 0.05) - 0.5) * 2.0;
    float majorLine = min(majorGrid.x, majorGrid.y);
    majorLine = 1.0 - smoothstep(0.0, 0.01, majorLine);
    
    // Distance fade
    float distance = length(in.worldPos - camera.position);
    float fade = 1.0 - smoothstep(20.0, 100.0, distance);
    
    // Pulse effect
    float pulse = sin(camera.time * 2.0 - distance * 0.1) * 0.2 + 0.8;
    
    float finalLine = max(line * 0.5, majorLine);
    float3 color = model.color.rgb * finalLine * fade * pulse;
    float alpha = finalLine * fade * model.opacity;
    
    return float4(color, alpha);
}