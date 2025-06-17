// UI.metal - Shaders for UI rendering

#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

// Holographic UI panel shader
fragment float4 ui_panel_fragment(VertexOut in [[stage_in]],
                                 constant CameraData& camera [[buffer(0)]],
                                 constant ModelData& model [[buffer(1)]],
                                 texture2d<float> uiTexture [[texture(0)]],
                                 sampler uiSampler [[sampler(0)]]) {
    
    float4 texColor = uiTexture.sample(uiSampler, in.texCoord);
    
    // Scanline effect
    float scanline = sin(in.worldPos.y * 100.0 + camera.time * 5.0) * 0.03 + 0.97;
    
    // Edge glow
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(camera.position - in.worldPos);
    float rim = 1.0 - saturate(dot(normal, viewDir));
    rim = pow(rim, 2.0);
    
    // Combine effects
    float3 color = texColor.rgb * model.color.rgb * scanline;
    color += model.color.rgb * rim * model.emission;
    
    float alpha = texColor.a * model.opacity * (0.7 + rim * 0.3);
    
    return float4(color, alpha);
}
