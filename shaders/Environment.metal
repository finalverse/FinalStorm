// Environment.metal - Shaders for environment rendering

#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

// Skybox shader
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

// Grid shader
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
