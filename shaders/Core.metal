// Core.metal - Core rendering shaders for FinalStorm

#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

// Basic vertex shader
vertex VertexOut basic_vertex(VertexIn in [[stage_in]],
                             constant ModelData& model [[buffer(0)]],
                             constant CameraData& camera [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = camera.viewProjMatrix * worldPos;
    out.worldNormal = normalize((model.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    
    // Calculate fog
    float distance = length(camera.position - out.worldPos);
    out.fogFactor = 1.0 - exp(-0.01 * distance);
    
    // Initialize unused fields
    out.tangentViewPos = float3(0.0);
    out.tangentLightPos = float3(0.0);
    out.tangentFragPos = float3(0.0);
    out.shadowCoord = float4(0.0);
    
    return out;
}

// Basic fragment shader with lighting
fragment float4 basic_fragment(VertexOut in [[stage_in]],
                              constant ModelData& model [[buffer(0)]],
                              constant CameraData& camera [[buffer(1)]],
                              constant Light* lights [[buffer(2)]],
                              constant int& numLights [[buffer(3)]]) {
    
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(camera.position - in.worldPos);
    
    // Base color
    float3 albedo = model.color.rgb;
    float alpha = model.color.a * model.opacity;
    
    // Lighting accumulation
    float3 color = float3(0.0);
    
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        float3 lightDir;
        float attenuation = 1.0;
        
        if (light.type == 0) { // Directional
            lightDir = normalize(-light.position);
        } else { // Point
            float3 lightVec = light.position - in.worldPos;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            attenuation = saturate(1.0 - (distance / light.range));
        }
        
        // Simple Phong lighting
        float NdotL = max(dot(normal, lightDir), 0.0);
        float3 diffuse = albedo * NdotL;
        
        float3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        float3 specular = float3(0.5) * spec * (1.0 - model.roughness);
        
        color += (diffuse + specular) * light.color * light.intensity * attenuation;
    }
    
    // Ambient
    color += albedo * 0.1;
    
    // Emission
    color += albedo * model.emission;
    
    // Fog
    float3 fogColor = float3(0.02, 0.02, 0.05);
    color = mix(color, fogColor, in.fogFactor * 0.5);
    
    return float4(color, alpha);
}

// Unlit shader for UI and effects
fragment float4 unlit_fragment(VertexOut in [[stage_in]],
                              constant ModelData& model [[buffer(0)]]) {
    return model.color;
}