// UI.metal - UI rendering shaders for FinalStorm

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// UI panel vertex shader
vertex VertexOut ui_vertex(VertexIn in [[stage_in]],
                          constant ModelData& model [[buffer(0)]],
                          constant CameraData& camera [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = camera.viewProjMatrix * worldPos;
    out.worldNormal = normalize((model.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    out.fogFactor = 0.0; // No fog for UI
    
    return out;
}

// Holographic UI panel shader
fragment float4 ui_panel_fragment(VertexOut in [[stage_in]],
                                 constant ModelData& model [[buffer(0)]],
                                 constant CameraData& camera [[buffer(1)]],
                                 texture2d<float> uiTexture [[texture(0)]],
                                 sampler uiSampler [[sampler(0)]]) {
    
    float4 texColor = uiTexture.sample(uiSampler, in.texCoord);
    
    // Holographic distortion
    float2 distorted = in.texCoord;
    distorted.x += sin(in.texCoord.y * 30.0 + camera.time * 5.0) * 0.002;
    
    // Scanline effect
    float scanline = sin(in.worldPos.y * 100.0 + camera.time * 10.0) * 0.03 + 0.97;
    
    // Edge glow
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(camera.position - in.worldPos);
    float rim = 1.0 - saturate(dot(normal, viewDir));
    rim = pow(rim, 2.0);
    
    float3 color = texColor.rgb * model.color.rgb * scanline;
    color += model.color.rgb * rim * model.emission;
    
    float alpha = texColor.a * model.opacity * (0.7 + rim * 0.3);
    
    return float4(color, alpha);
}

// Text rendering shader
fragment float4 ui_text_fragment(VertexOut in [[stage_in]],
                                constant ModelData& model [[buffer(0)]],
                                texture2d<float> fontTexture [[texture(0)]],
                                sampler fontSampler [[sampler(0)]]) {
    
    float alpha = fontTexture.sample(fontSampler, in.texCoord).r;
    
    // Glow effect for text
    float glow = fontTexture.sample(fontSampler, in.texCoord, level(1.0)).r;
    
    float3 color = model.color.rgb;
    color += model.color.rgb * glow * model.emission;
    
    return float4(color, alpha * model.opacity);
}