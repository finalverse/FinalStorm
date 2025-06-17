// FinalStorm.metal - Shader code for immersive 3D visualization

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Vertex data structures
struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
    float4 tangent [[attribute(3)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 worldPos;
    float3 normal;
    float2 texCoord;
    float3 tangent;
    float3 bitangent;
    float4 shadowCoord;
    float3 viewDir;
};

// Uniform structures
struct CameraData {
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 viewProjMatrix;
    float3 position;
    float time;
};

struct ModelData {
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float4 color;
    float metallic;
    float roughness;
    float emission;
    float opacity;
};

struct LightData {
    float3 position;
    float range;
    float3 direction;
    float intensity;
    float3 color;
    float innerCone;
    float outerCone;
    int type; // 0=directional, 1=point, 2=spot
};

struct ShadowData {
    float4x4 lightViewProjMatrix;
    float bias;
    float strength;
};

// Main vertex shader
vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                            constant CameraData& camera [[buffer(0)]],
                            constant ModelData& model [[buffer(1)]]) {
    VertexOut out;
    
    float4 worldPos = model.modelMatrix * float4(in.position, 1.0);
    out.worldPos = worldPos.xyz;
    out.position = camera.viewProjMatrix * worldPos;
    
    out.normal = normalize((model.normalMatrix * float4(in.normal, 0.0)).xyz);
    out.texCoord = in.texCoord;
    
    // Calculate tangent space
    out.tangent = normalize((model.modelMatrix * float4(in.tangent.xyz, 0.0)).xyz);
    out.bitangent = cross(out.normal, out.tangent) * in.tangent.w;
    
    out.viewDir = normalize(camera.position - out.worldPos);
    
    return out;
}

// Shadow mapping vertex shader
vertex float4 shadow_vertex(VertexIn in [[stage_in]],
                           constant float4x4& modelMatrix [[buffer(0)]],
                           constant float4x4& lightViewProjMatrix [[buffer(1)]]) {
    float4 worldPos = modelMatrix * float4(in.position, 1.0);
    return lightViewProjMatrix * worldPos;
}

// PBR lighting calculation
float3 calculatePBR(float3 albedo, float metallic, float roughness,
                   float3 normal, float3 viewDir, float3 lightDir,
                   float3 lightColor, float lightIntensity) {
    float3 halfVector = normalize(viewDir + lightDir);
    
    // Calculate fresnel
    float3 F0 = mix(float3(0.04), albedo, metallic);
    float VdotH = saturate(dot(viewDir, halfVector));
    float3 fresnel = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
    // Calculate normal distribution
    float NdotH = saturate(dot(normal, halfVector));
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    float normalDist = alpha2 / (M_PI_F * denom * denom);
    
    // Calculate geometry
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
    
    return (diffuse + specular) * lightColor * lightIntensity * NdotL;
}

// Main fragment shader with PBR lighting
fragment float4 fragment_main(VertexOut in [[stage_in]],
                             constant CameraData& camera [[buffer(0)]],
                             constant ModelData& model [[buffer(1)]],
                             constant LightData* lights [[buffer(2)]],
                             constant int& numLights [[buffer(3)]],
                             texture2d<float> shadowMap [[texture(0)]],
                             sampler shadowSampler [[sampler(0)]]) {
    
    float3 normal = normalize(in.normal);
    float3 viewDir = normalize(in.viewDir);
    
    // Base color with emission
    float3 albedo = model.color.rgb;
    float3 emission = albedo * model.emission;
    
    // Accumulate lighting
    float3 color = float3(0.0);
    
    for (int i = 0; i < numLights; i++) {
        LightData light = lights[i];
        float3 lightDir;
        float attenuation = 1.0;
        
        if (light.type == 0) { // Directional
            lightDir = normalize(-light.direction);
        } else { // Point or Spot
            float3 lightVec = light.position - in.worldPos;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            
            // Distance attenuation
            attenuation = saturate(1.0 - (distance / light.range));
            attenuation *= attenuation;
            
            // Spot light cone
            if (light.type == 2) {
                float cosAngle = dot(-lightDir, normalize(light.direction));
                float inner = cos(light.innerCone);
                float outer = cos(light.outerCone);
                float spotFactor = smoothstep(outer, inner, cosAngle);
                attenuation *= spotFactor;
            }
        }
        
        // PBR lighting
        float3 lightContrib = calculatePBR(albedo, model.metallic, model.roughness,
                                          normal, viewDir, lightDir,
                                          light.color, light.intensity * attenuation);
        color += lightContrib;
    }
    
    // Add emission
    color += emission;
    
    // Ambient
    color += albedo * 0.03;
    
    // Tone mapping
    color = color / (color + 1.0);
    
    // Gamma correction
    color = pow(color, 1.0 / 2.2);
    
    return float4(color, model.opacity);
}

// Holographic UI shader
fragment float4 hologram_fragment(VertexOut in [[stage_in]],
                                 constant CameraData& camera [[buffer(0)]],
                                 constant ModelData& model [[buffer(1)]],
                                 texture2d<float> uiTexture [[texture(0)]],
                                 sampler uiSampler [[sampler(0)]]) {
    
    float4 texColor = uiTexture.sample(uiSampler, in.texCoord);
    
    // Holographic effect
    float scanline = sin(in.worldPos.y * 50.0 + camera.time * 2.0) * 0.04;
    float flicker = sin(camera.time * 30.0) * 0.02 + 0.98;
    
    // Edge glow
    float3 normal = normalize(in.normal);
    float rim = 1.0 - saturate(dot(normal, in.viewDir));
    rim = pow(rim, 2.0);
    
    // Combine effects
    float3 color = texColor.rgb * model.color.rgb * flicker;
    color += model.color.rgb * rim * 0.5;
    color.g += scanline;
    
    float alpha = texColor.a * model.opacity * (0.7 + rim * 0.3);
    
    return float4(color, alpha);
}

// Particle shader
struct ParticleVertexIn {
    float3 position [[attribute(0)]];
    float size [[attribute(1)]];
    float4 color [[attribute(2)]];
};

struct ParticleVertexOut {
    float4 position [[position]];
    float pointSize [[point_size]];
    float4 color;
};

vertex ParticleVertexOut particle_vertex(ParticleVertexIn in [[stage_in]],
                                        constant CameraData& camera [[buffer(0)]]) {
    ParticleVertexOut out;
    
    float4 viewPos = camera.viewMatrix * float4(in.position, 1.0);
    out.position = camera.projMatrix * viewPos;
    
    // Point size with distance attenuation
    float distance = length(viewPos.xyz);
    out.pointSize = in.size * 100.0 / distance;
    out.pointSize = clamp(out.pointSize, 1.0, 64.0);
    
    out.color = in.color;
    
    return out;
}

fragment float4 particle_fragment(ParticleVertexOut in [[stage_in]],
                                 float2 pointCoord [[point_coord]]) {
    // Circular particle shape
    float2 coord = pointCoord * 2.0 - 1.0;
    float r = length(coord);
    if (r > 1.0) discard_fragment();
    
    // Soft edges
    float alpha = 1.0 - smoothstep(0.5, 1.0, r);
    alpha *= in.color.a;
    
    // Glow effect
    float3 color = in.color.rgb * (1.0 + (1.0 - r) * 0.5);
    
    return float4(color, alpha);
}

// Grid shader for ground plane
fragment float4 grid_fragment(VertexOut in [[stage_in]],
                             constant CameraData& camera [[buffer(0)]],
                             constant ModelData& model [[buffer(1)]]) {
    
    // Grid lines
    float2 grid = abs(fract(in.worldPos.xz * 0.5) - 0.5) * 2.0;
    float line = min(grid.x, grid.y);
    line = 1.0 - smoothstep(0.0, 0.02, line);
    
    // Distance fade
    float distance = length(in.worldPos - camera.position);
    float fade = 1.0 - smoothstep(20.0, 50.0, distance);
    
    // Pulse effect
    float pulse = sin(camera.time * 2.0) * 0.2 + 0.8;
    
    float3 color = model.color.rgb * line * fade * pulse;
    float alpha = line * fade * model.opacity;
    
    return float4(color, alpha);
}

// Post-processing bloom shader
kernel void bloom_extract(texture2d<float, access::read> inTexture [[texture(0)]],
                         texture2d<float, access::write> outTexture [[texture(1)]],
                         constant float& threshold [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    float4 color = inTexture.read(gid);
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (brightness > threshold) {
        outTexture.write(color, gid);
    } else {
        outTexture.write(float4(0.0), gid);
    }
}

kernel void gaussian_blur(texture2d<float, access::read> inTexture [[texture(0)]],
                         texture2d<float, access::write> outTexture [[texture(1)]],
                         constant float2& direction [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    float weights[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};
    
    float4 color = inTexture.read(gid) * weights[0];
    
    for (int i = 1; i < 5; i++) {
        float2 offset = direction * float(i);
        color += inTexture.read(gid + uint2(offset)) * weights[i];
        color += inTexture.read(gid - uint2(offset)) * weights[i];
    }
    
    outTexture.write(color, gid);
}

// Skybox shader with dynamic tinting
fragment float4 skybox_fragment(VertexOut in [[stage_in]],
                               constant CameraData& camera [[buffer(0)]],
                               constant ModelData& model [[buffer(1)]],
                               texturecube<float> skyTexture [[texture(0)]],
                               sampler skySampler [[sampler(0)]]) {
    
    float3 texCoord = normalize(in.worldPos - camera.position);
    float4 skyColor = skyTexture.sample(skySampler, texCoord);
    
    // Apply tint based on system health
    skyColor.rgb = mix(skyColor.rgb, model.color.rgb, 0.3);
    
    // Add atmospheric glow
    float y = texCoord.y;
    float horizon = 1.0 - abs(y);
    horizon = pow(horizon, 3.0);
    skyColor.rgb += model.color.rgb * horizon * 0.2;
    
    return skyColor;
}