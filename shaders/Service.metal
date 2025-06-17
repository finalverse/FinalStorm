// ServiceVisualization.metal - Enhanced shaders for FinalStorm service visualization
// This extends the existing Shaders.metal with advanced visual effects

#include <metal_stdlib>
#include <simd/simd.h>
#include "MetalShaderTypes.h"

using namespace metal;

// Enhanced vertex structure for service visualization
struct ServiceVertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
    float3 tangent [[attribute(3)]];
    float3 bitangent [[attribute(4)]];
};

struct ServiceVertexOut {
    float4 position [[position]];
    float3 worldPos;
    float3 worldNormal;
    float2 texCoord;
    float3 tangentViewPos;
    float3 tangentLightPos;
    float3 tangentFragPos;
    float4 shadowCoord;
    float fogFactor;
};

// Service-specific uniforms
struct ServiceUniforms {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 normalMatrix;
    float4x4 shadowMatrix;
    
    float3 cameraPosition;
    float time;
    
    float4 baseColor;
    float4 glowColor;
    float activityLevel;
    float glowIntensity;
    float pulsePhase;
    
    int serviceType; // Enum value for service type
    float metallic;
    float roughness;
    float emission;
};

// Light structure
struct Light {
    float3 position;
    float3 color;
    float intensity;
    float range;
    int type; // 0: directional, 1: point, 2: spot
};

// Service vertex shader
vertex ServiceVertexOut service_vertex(ServiceVertexIn in [[stage_in]],
                                      constant ServiceUniforms& uniforms [[buffer(0)]]) {
    ServiceVertexOut out;
    
    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
    
    // Transform normal to world space
    out.worldNormal = normalize((uniforms.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    
    // Calculate tangent space transformation
    float3 T = normalize((uniforms.modelMatrix * float4(in.tangent, 0.0)).xyz);
    float3 B = normalize((uniforms.modelMatrix * float4(in.bitangent, 0.0)).xyz);
    float3 N = out.worldNormal;
    float3x3 TBN = transpose(float3x3(T, B, N));
    
    // Transform positions to tangent space for normal mapping
    out.tangentViewPos = TBN * uniforms.cameraPosition;
    out.tangentFragPos = TBN * out.worldPos;
    
    // Shadow coordinate
    out.shadowCoord = uniforms.shadowMatrix * worldPos;
    
    // Calculate fog
    float distance = length(uniforms.cameraPosition - out.worldPos);
    out.fogFactor = 1.0 - exp(-0.02 * distance);
    
    return out;
}

// PBR lighting calculation
float3 calculatePBRLighting(float3 albedo, float metallic, float roughness,
                           float3 normal, float3 viewDir, float3 lightDir,
                           float3 lightColor, float attenuation) {
    float3 halfVector = normalize(viewDir + lightDir);
    
    // Fresnel
    float3 F0 = mix(float3(0.04), albedo, metallic);
    float VdotH = saturate(dot(viewDir, halfVector));
    float3 fresnel = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
    // Normal distribution (GGX)
    float NdotH = saturate(dot(normal, halfVector));
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    float normalDist = alpha2 / (M_PI_F * denom * denom);
    
    // Geometry
    float NdotV = saturate(dot(normal, viewDir));
    float NdotL = saturate(dot(normal, lightDir));
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float geomV = NdotV / (NdotV * (1.0 - k) + k);
    float geomL = NdotL / (NdotL * (1.0 - k) + k);
    float geometry = geomV * geomL;
    
    // BRDF
    float3 numerator = normalDist * geometry * fresnel;
    float denominator = 4.0 * NdotV * NdotL + 0.001;
    float3 specular = numerator / denominator;
    
    // Diffuse
    float3 kD = (1.0 - fresnel) * (1.0 - metallic);
    float3 diffuse = kD * albedo / M_PI_F;
    
    return (diffuse + specular) * lightColor * attenuation * NdotL;
}

// Service fragment shader with advanced effects
fragment float4 service_fragment(ServiceVertexOut in [[stage_in]],
                                constant ServiceUniforms& uniforms [[buffer(0)]],
                                constant Light* lights [[buffer(1)]],
                                constant int& numLights [[buffer(2)]],
                                texture2d<float> shadowMap [[texture(0)]],
                                texture2d<float> normalMap [[texture(1)]],
                                texture2d<float> emissionMap [[texture(2)]],
                                sampler textureSampler [[sampler(0)]]) {
    
    // Sample textures
    float3 normal = normalize(in.worldNormal);
    if (!is_null_texture(normalMap)) {
        float3 normalTex = normalMap.sample(textureSampler, in.texCoord).xyz * 2.0 - 1.0;
        normal = normalize(in.worldNormal + normalTex);
    }
    
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    
    // Base color with activity-based modification
    float3 albedo = uniforms.baseColor.rgb;
    float opacity = uniforms.baseColor.a;
    
    // Service-specific color variations
    if (uniforms.serviceType == 1) { // AI Service
        // Neural pulse effect
        float pulse = sin(uniforms.pulsePhase + in.worldPos.y * 2.0) * 0.5 + 0.5;
        albedo = mix(albedo, uniforms.glowColor.rgb, pulse * uniforms.activityLevel);
    } else if (uniforms.serviceType == 2) { // Audio Service
        // Waveform effect
        float wave = sin(uniforms.time * 5.0 + in.worldPos.x * 10.0) * 0.1;
        albedo.g += wave * uniforms.activityLevel;
    }
    
    // Lighting accumulation
    float3 color = float3(0.0);
    
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        float3 lightDir;
        float attenuation = 1.0;
        
        if (light.type == 0) { // Directional
            lightDir = normalize(-light.position);
        } else { // Point/Spot
            float3 lightVec = light.position - in.worldPos;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            
            // Distance attenuation
            attenuation = saturate(1.0 - (distance * distance) / (light.range * light.range));
        }
        
        // Shadow calculation
        float shadow = 1.0;
        if (!is_null_texture(shadowMap)) {
            float2 shadowUV = in.shadowCoord.xy / in.shadowCoord.w * 0.5 + 0.5;
            float shadowDepth = shadowMap.sample(textureSampler, shadowUV).r;
            float currentDepth = in.shadowCoord.z / in.shadowCoord.w;
            shadow = currentDepth - 0.005 > shadowDepth ? 0.3 : 1.0;
        }
        
        // PBR lighting
        float3 lightContrib = calculatePBRLighting(
            albedo, uniforms.metallic, uniforms.roughness,
            normal, viewDir, lightDir,
            light.color * light.intensity, attenuation * shadow
        );
        
        color += lightContrib;
    }
    
    // Emission
    float3 emission = albedo * uniforms.emission * uniforms.glowIntensity;
    if (!is_null_texture(emissionMap)) {
        emission += emissionMap.sample(textureSampler, in.texCoord).rgb * uniforms.glowIntensity;
    }
    
    // Holographic rim effect for UI elements
    float rim = 1.0 - saturate(dot(normal, viewDir));
    float3 rimColor = uniforms.glowColor.rgb * pow(rim, 2.0) * uniforms.glowIntensity;
    
    // Activity-based effects
    float activityGlow = uniforms.activityLevel * 0.2;
    emission += albedo * activityGlow;
    
    // Combine all lighting
    color += emission + rimColor;
    
    // Ambient
    color += albedo * 0.05;
    
    // Fog
    float3 fogColor = float3(0.02, 0.02, 0.05);
    color = mix(color, fogColor, in.fogFactor * 0.5);
    
    // Tone mapping
    color = color / (color + 1.0);
    
    // Gamma correction
    color = pow(color, 1.0 / 2.2);
    
    return float4(color, opacity);
}

// Particle vertex shader
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
    float2 texCoord;
    float rotation;
};

vertex ParticleVertexOut particle_vertex(ParticleVertexIn in [[stage_in]],
                                        constant ServiceUniforms& uniforms [[buffer(0)]]) {
    ParticleVertexOut out;
    
    float4 viewPos = uniforms.viewMatrix * float4(in.position, 1.0);
    out.position = uniforms.projectionMatrix * viewPos;
    
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
    
    float4 texColor = float4(1.0);
    if (!is_null_texture(particleTexture)) {
        texColor = particleTexture.sample(particleSampler, rotated);
    } else {
        // Default circular particle
        float r = length(coord);
        if (r > 1.0) discard_fragment();
        texColor.a = 1.0 - smoothstep(0.5, 1.0, r);
    }
    
    // Glow effect
    float3 color = in.color.rgb * (1.0 + (1.0 - length(coord)) * 0.5);
    
    return float4(color, in.color.a * texColor.a);
}

// Holographic UI shader
fragment float4 holographic_ui_fragment(ServiceVertexOut in [[stage_in]],
                                       constant ServiceUniforms& uniforms [[buffer(0)]],
                                       texture2d<float> uiTexture [[texture(0)]],
                                       sampler uiSampler [[sampler(0)]]) {
    
    float4 texColor = uiTexture.sample(uiSampler, in.texCoord);
    
    // Holographic scanline effect
    float scanline = sin(in.worldPos.y * 100.0 + uniforms.time * 5.0) * 0.03 + 0.97;
    
    // Digital glitch effect
    float glitch = step(0.98, sin(uniforms.time * 20.0 + in.worldPos.x * 50.0)) * 0.05;
    
    // Edge glow
    float3 normal = normalize(in.worldNormal);
    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
    float rim = 1.0 - saturate(dot(normal, viewDir));
    rim = pow(rim, 2.0);
    
    // Combine effects
    float3 color = texColor.rgb * uniforms.baseColor.rgb;
    color *= scanline;
    color.g += glitch;
    color += uniforms.glowColor.rgb * rim * uniforms.glowIntensity;
    
    // Transparency with rim enhancement
    float alpha = texColor.a * uniforms.baseColor.a;
    alpha = mix(alpha, 1.0, rim * 0.5);
    
    return float4(color, alpha);
}

// Environment shader for skybox and grid
vertex ServiceVertexOut environment_vertex(ServiceVertexIn in [[stage_in]],
                                         constant ServiceUniforms& uniforms [[buffer(0)]]) {
    ServiceVertexOut out;
    
    // For skybox, position is direction
    out.worldPos = in.position;
    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * float4(in.position + uniforms.cameraPosition, 1.0);
    out.position.z = out.position.w; // Always at far plane
    
    out.worldNormal = in.normal;
    out.texCoord = in.texCoord;
    out.fogFactor = 0.0; // No fog for skybox
    
    return out;
}

fragment float4 skybox_fragment(ServiceVertexOut in [[stage_in]],
                               constant ServiceUniforms& uniforms [[buffer(0)]],
                               texturecube<float> skyTexture [[texture(0)]],
                               sampler skySampler [[sampler(0)]]) {
    
    float3 texCoord = normalize(in.worldPos);
    float4 skyColor = skyTexture.sample(skySampler, texCoord);
    
    // System health tinting
    float health = uniforms.activityLevel; // Reusing activity level as system health
    float3 healthyTint = float3(0.0, 0.1, 0.2);
    float3 warningTint = float3(0.2, 0.1, 0.0);
    float3 criticalTint = float3(0.2, 0.0, 0.0);
    
    float3 tint;
    if (health > 0.7) {
        tint = healthyTint;
    } else if (health > 0.3) {
        float t = (health - 0.3) / 0.4;
        tint = mix(warningTint, healthyTint, t);
    } else {
        float t = health / 0.3;
        tint = mix(criticalTint, warningTint, t);
    }
    
    skyColor.rgb = mix(skyColor.rgb, skyColor.rgb * (1.0 + tint), 0.5);
    
    // Add atmospheric glow at horizon
    float y = texCoord.y;
    float horizon = 1.0 - abs(y);
    horizon = pow(horizon, 3.0);
    skyColor.rgb += uniforms.glowColor.rgb * horizon * 0.3;
    
    return skyColor;
}

fragment float4 grid_fragment(ServiceVertexOut in [[stage_in]],
                             constant ServiceUniforms& uniforms [[buffer(0)]]) {
    
    // Grid lines
    float2 grid = abs(fract(in.worldPos.xz * 0.5) - 0.5) * 2.0;
    float line = min(grid.x, grid.y);
    line = 1.0 - smoothstep(0.0, 0.02, line);
    
    // Major grid lines every 10 units
    float2 majorGrid = abs(fract(in.worldPos.xz * 0.05) - 0.5) * 2.0;
    float majorLine = min(majorGrid.x, majorGrid.y);
    majorLine = 1.0 - smoothstep(0.0, 0.01, majorLine);
    
    // Distance fade
    float distance = length(in.worldPos - uniforms.cameraPosition);
    float fade = 1.0 - smoothstep(20.0, 100.0, distance);
    
    // Pulse effect based on activity
    float pulse = sin(uniforms.time * 2.0 - distance * 0.1) * 0.2 + 0.8;
    pulse *= uniforms.activityLevel;
    
    // Combine lines
    float finalLine = max(line * 0.5, majorLine);
    
    float3 color = uniforms.baseColor.rgb * finalLine * fade * pulse;
    float alpha = finalLine * fade * uniforms.baseColor.a;
    
    return float4(color, alpha);
}

// Post-processing compute shaders
kernel void bloom_threshold(texture2d<float, access::read> inTexture [[texture(0)]],
                           texture2d<float, access::write> outTexture [[texture(1)]],
                           constant float& threshold [[buffer(0)]],
                           uint2 gid [[thread_position_in_grid]]) {
    
    float4 color = inTexture.read(gid);
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (brightness > threshold) {
        // Soft threshold
        float soft = (brightness - threshold) / (1.0 - threshold);
        color.rgb *= soft;
        outTexture.write(color, gid);
    } else {
        outTexture.write(float4(0.0), gid);
    }
}

kernel void gaussian_blur(texture2d<float, access::read> inTexture [[texture(0)]],
                         texture2d<float, access::write> outTexture [[texture(1)]],
                         constant float2& direction [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    // 9-tap Gaussian blur
    float weights[9] = {0.013, 0.036, 0.082, 0.135, 0.161, 0.135, 0.082, 0.036, 0.013};
    float offsets[9] = {-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
    
    float4 color = float4(0.0);
    
    for (int i = 0; i < 9; i++) {
        float2 offset = direction * offsets[i];
        color += inTexture.read(gid + uint2(offset)) * weights[i];
    }
    
    outTexture.write(color, gid);
}

kernel void tone_mapping(texture2d<float, access::read> hdrTexture [[texture(0)]],
                        texture2d<float, access::read> bloomTexture [[texture(1)]],
                        texture2d<float, access::write> ldrTexture [[texture(2)]],
                        constant float& exposure [[buffer(0)]],
                        constant float& bloomIntensity [[buffer(1)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    float4 hdr = hdrTexture.read(gid);
    float4 bloom = bloomTexture.read(gid);
    
    // Add bloom
    hdr.rgb += bloom.rgb * bloomIntensity;
    
    // Exposure adjustment
    hdr.rgb *= exposure;
    
    // Reinhard tone mapping
    float3 mapped = hdr.rgb / (hdr.rgb + 1.0);
    
    // Gamma correction
    mapped = pow(mapped, 1.0 / 2.2);
    
    ldrTexture.write(float4(mapped, hdr.a), gid);
}