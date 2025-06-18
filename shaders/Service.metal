// Service.metal - Service visualization shaders for FinalStorm

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// Service-specific uniforms
struct ServiceUniforms {
    float activityLevel;
    float pulsePhase;
    int serviceType;
    float connectionStrength;
};

// Service orb vertex shader
vertex VertexOut service_vertex(VertexIn in [[stage_in]],
                               constant ModelData& model [[buffer(0)]],
                               constant CameraData& camera [[buffer(1)]],
                               constant ServiceUniforms& service [[buffer(2)]]) {
    VertexOut out;
    
    // Animate vertex position for pulsing effect
    float pulse = sin(service.pulsePhase) * 0.1 * service.activityLevel;
    float3 animatedPos = in.position * (1.0 + pulse);
    
    float4 worldPos = model.modelMatrix * float4(animatedPos, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = camera.viewProjMatrix * worldPos;
    out.worldNormal = normalize((model.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    
    return out;
}

// Service orb fragment shader
fragment float4 service_fragment(VertexOut in [[stage_in]],
                                constant ModelData& model [[buffer(0)]],
                                constant CameraData& camera [[buffer(1)]],
                                constant ServiceUniforms& service [[buffer(2)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(camera.position - in.worldPos);
    
    // Base color
    float3 color = model.color.rgb;
    
    // Service-specific patterns
    float pattern = 0.0;
    switch (service.serviceType) {
        case 0: // API Gateway
            pattern = step(0.9, sin(in.texCoord.x * 30.0) * sin(in.texCoord.y * 30.0));
            break;
        case 1: // AI Service
            pattern = noise(in.texCoord * 10.0 + camera.time * 0.5);
            pattern = smoothstep(0.4, 0.6, pattern);
            break;
        case 2: // Audio Service
            pattern = sin(in.texCoord.x * 20.0 + sin(in.texCoord.y * 5.0 + camera.time * 2.0));
            pattern = smoothstep(-0.5, 0.5, pattern);
            break;
        default:
            pattern = 0.0;
    }
    
    color = mix(color, color * 1.5, pattern * 0.3);
    
    // Fresnel rim
    float fresnel = pow(1.0 - saturate(dot(normal, viewDir)), 2.0);
    color += model.color.rgb * fresnel * 2.0;
    
    // Activity glow
    color += model.color.rgb * model.emission * service.activityLevel;
    
    // Holographic scanline
    float scanline = sin(in.worldPos.y * 50.0 + camera.time * 2.0) * 0.04 + 0.96;
    color *= scanline;
    
    float alpha = model.opacity * (0.8 + fresnel * 0.2);
    
    return float4(color, alpha);
}

// Connection beam shader
fragment float4 connection_fragment(VertexOut in [[stage_in]],
                                   constant ModelData& model [[buffer(0)]],
                                   constant CameraData& camera [[buffer(1)]],
                                   constant ServiceUniforms& service [[buffer(2)]]) {
    
    // Energy flow
    float flow = fract(in.texCoord.y - camera.time * 0.5);
    flow = smoothstep(0.0, 0.1, flow) * smoothstep(1.0, 0.9, flow);
    
    // Beam core
    float beam = 1.0 - smoothstep(0.0, 0.5, abs(in.texCoord.x - 0.5) * 2.0);
    
    float3 color = model.color.rgb * beam;
    color += color * flow * 2.0;
    
    float alpha = (beam + flow * 0.5) * service.connectionStrength * model.opacity;
    
    return float4(color, alpha);
}