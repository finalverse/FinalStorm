//
// shaders/Common.metal
// Enhanced Metal shader library for FinalStorm with SIMD-optimized structures
// Uses native Metal/SIMD types for optimal GPU performance on Apple platforms
//

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

// Vertex input structure - matches our C++ vertex layout
struct VertexIn {
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
    float3 color    [[attribute(3)]];
};

// Vertex output / Fragment input with optimized layout
struct VertexOut {
    float4 position [[position]];
    float3 worldPosition;
    float3 worldNormal;
    float2 texCoord;
    float3 color;
    float3 viewDirection;
    float depth; // For depth-based effects
};

// Uniform data structures using SIMD-aligned types
struct RenderConstants {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 normalMatrix;
    float3 cameraPosition;
    float time;
    float3 lightDirection;
    float _padding1;
    float3 lightColor;
    float lightIntensity;
    float4 screenParams; // width, height, 1/width, 1/height
};

struct MaterialConstants {
    float3 albedo;
    float metallic;
    float3 emission;
    float roughness;
    float ao;
    float3 _padding;
};

struct ParticleConstants {
    float3 position;
    float size;
    float3 velocity;
    float life;
    float4 color;
    float fadeRate;
    float3 _padding2;
};

// Enhanced vertex shader with better optimization
vertex VertexOut default_vertex(VertexIn in [[stage_in]],
                               constant RenderConstants& constants [[buffer(1)]]) {
    VertexOut out;
    
    // Transform position to world space using SIMD operations
    float4 worldPosition = constants.modelMatrix * float4(in.position, 1.0);
    out.worldPosition = worldPosition.xyz;
    
    // Transform to clip space
    float4 viewPosition = constants.viewMatrix * worldPosition;
    out.position = constants.projectionMatrix * viewPosition;
    
    // Store depth for effects
    out.depth = out.position.z / out.position.w;
    
    // Transform normal to world space using normal matrix
    out.worldNormal = normalize((constants.normalMatrix * float4(in.normal, 0.0)).xyz);
    
    // Pass through texture coordinates and color
    out.texCoord = in.texCoord;
    out.color = in.color;
    
    // Calculate view direction in world space
    out.viewDirection = normalize(constants.cameraPosition - worldPosition.xyz);
    
    return out;
}

// Enhanced PBR fragment shader with improved lighting model
fragment float4 default_fragment(VertexOut in [[stage_in]],
                                constant RenderConstants& constants [[buffer(1)]],
                                constant MaterialConstants& material [[buffer(2)]],
                                texture2d<float> albedoTexture [[texture(0)]],
                                texture2d<float> normalTexture [[texture(1)]],
                                texture2d<float> metallicRoughnessTexture [[texture(2)]],
                                texture2d<float> aoTexture [[texture(3)]],
                                sampler textureSampler [[sampler(0)]]) {
    
    // Sample textures
    float3 baseColor = material.albedo;
    if (!is_null_texture(albedoTexture)) {
        baseColor *= albedoTexture.sample(textureSampler, in.texCoord).rgb;
    }
    baseColor *= in.color;
    
    // Sample normal map
    float3 normal = normalize(in.worldNormal);
    if (!is_null_texture(normalTexture)) {
        float3 normalSample = normalTexture.sample(textureSampler, in.texCoord).rgb * 2.0 - 1.0;
        // For simplicity, just perturb the normal slightly
        normal = normalize(normal + normalSample * 0.1);
    }
    
    // Sample metallic/roughness
    float metallic = material.metallic;
    float roughness = material.roughness;
    if (!is_null_texture(metallicRoughnessTexture)) {
        float3 mrSample = metallicRoughnessTexture.sample(textureSampler, in.texCoord).rgb;
        metallic *= mrSample.b; // Blue channel
        roughness *= mrSample.g; // Green channel
    }
    
    // Sample ambient occlusion
    float ao = material.ao;
    if (!is_null_texture(aoTexture)) {
        ao *= aoTexture.sample(textureSampler, in.texCoord).r;
    }
    
    // Lighting calculations
    float3 N = normal;
    float3 V = normalize(in.viewDirection);
    float3 L = normalize(-constants.lightDirection);
    float3 H = normalize(L + V);
    
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);
    
    // Fresnel-Schlick approximation
    float3 F0 = mix(float3(0.04), baseColor, metallic);
    float3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    
    // Distribution (GGX/Trowbridge-Reitz)
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    float D = alpha2 / (M_PI_F * denom * denom);
    
    // Geometry function (Smith model)
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float G1L = NdotL / (NdotL * (1.0 - k) + k);
    float G1V = NdotV / (NdotV * (1.0 - k) + k);
    float G = G1L * G1V;
    
    // BRDF
    float3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.001;
    float3 specular = numerator / denominator;
    
    // Energy conservation
    float3 kS = F;
    float3 kD = float3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    // Diffuse
    float3 diffuse = kD * baseColor / M_PI_F;
    
    // Combine direct lighting
    float3 color = (diffuse + specular) * constants.lightColor * constants.lightIntensity * NdotL;
    
    // Add ambient lighting with AO
    float3 ambient = float3(0.03) * baseColor * ao;
    color += ambient;
    
    // Add emission
    color += material.emission;
    
    // Tone mapping (simple Reinhard)
    color = color / (color + float3(1.0));
    
    // Gamma correction
    color = pow(color, float3(1.0/2.2));
    
    return float4(color, 1.0);
}

// Service visualization fragment shader with dynamic effects
fragment float4 service_fragment(VertexOut in [[stage_in]],
                                constant RenderConstants& constants [[buffer(1)]],
                                constant MaterialConstants& material [[buffer(2)]]) {
    
    float3 color = material.albedo * in.color;
    
    // Service activity pulse effect
    float pulse = sin(constants.time * 3.0) * 0.5 + 0.5;
    color *= (0.7 + 0.3 * pulse);
    
    // Distance-based glow
    float distanceToCenter = length(in.texCoord - 0.5);
    float glow = 1.0 - smoothstep(0.0, 0.5, distanceToCenter);
    
    // Network data flow effect
    float2 flowCoord = in.texCoord + float2(constants.time * 0.1, 0.0);
    float flow = sin(flowCoord.x * 10.0) * 0.1 + 0.9;
    
    color *= glow * flow;
    color += material.emission;
    
    return float4(color, glow * 0.8);
}

// Enhanced particle vertex shader with physics simulation
struct ParticleVertexIn {
    float3 position [[attribute(0)]];
    float3 velocity [[attribute(1)]];
    float size [[attribute(2)]];
    float life [[attribute(3)]];
    float4 color [[attribute(4)]];
    float startTime [[attribute(5)]];
};

struct ParticleVertexOut {
    float4 position [[position]];
    float size [[point_size]];
    float4 color;
    float life;
    float2 texCoord;
};

vertex ParticleVertexOut particle_vertex(ParticleVertexIn in [[stage_in]],
                                        constant RenderConstants& constants [[buffer(1)]]) {
    ParticleVertexOut out;
    
    // Calculate elapsed time for this particle
    float elapsed = constants.time - in.startTime;
    
    // Physics simulation
    float3 gravity = float3(0.0, -9.8, 0.0);
    float3 animatedPosition = in.position + in.velocity * elapsed + 0.5 * gravity * elapsed * elapsed;
    
    // Transform to world space
    float4 worldPosition = constants.modelMatrix * float4(animatedPosition, 1.0);
    float4 viewPosition = constants.viewMatrix * worldPosition;
    out.position = constants.projectionMatrix * viewPosition;
    
    // Size attenuation based on distance
    float distance = length(viewPosition.xyz);
    out.size = in.size * (100.0 / (1.0 + distance));
    
    // Color and life
    out.color = in.color;
    out.life = max(0.0, in.life - elapsed);
    
    return out;
}

fragment float4 particle_fragment(ParticleVertexOut in [[stage_in]],
                                 float2 pointCoord [[point_coord]]) {
    
    // Create circular particles with soft edges
    float2 center = float2(0.5);
    float dist = distance(pointCoord, center);
    
    if (dist > 0.5) {
        discard_fragment();
    }
    
    // Soft edge with life-based fade
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
    alpha *= in.life;
    alpha *= in.color.a;
    
    // Inner glow effect
    float glow = 1.0 - smoothstep(0.0, 0.3, dist);
    float3 color = in.color.rgb * (1.0 + glow);
    
    return float4(color, alpha);
}

// Enhanced holographic UI fragment shader
fragment float4 holographic_fragment(VertexOut in [[stage_in]],
                                   constant RenderConstants& constants [[buffer(1)]],
                                   constant MaterialConstants& material [[buffer(2)]]) {
    
    float3 color = material.albedo * in.color;
    
    // Animated scanlines
    float scanlineFreq = 100.0;
    float scanlineSpeed = 5.0;
    float scanline = sin((in.position.y + constants.time * scanlineSpeed) * scanlineFreq * 0.01) * 0.5 + 0.5;
    scanline = smoothstep(0.3, 0.7, scanline);
    
    // Edge detection for hologram outline
    float2 screenPos = in.position.xy * constants.screenParams.zw;
    float2 edge = abs(screenPos - 0.5);
    float edgeGlow = 1.0 - smoothstep(0.0, 0.1, max(edge.x, edge.y));
    
    // Flickering effect
    float flicker = sin(constants.time * 17.0) * 0.05 + 0.95;
    
    // Depth-based transparency
    float depthFade = 1.0 - smoothstep(0.0, 1.0, in.depth);
    
    // Combine all effects
    color *= scanline * flicker * depthFade;
    color += material.emission;
    color += edgeGlow * float3(0.0, 0.5, 1.0); // Blue edge glow
    
    float alpha = (scanline * 0.7 + edgeGlow * 0.3) * flicker * depthFade;
    
    return float4(color, alpha);
}

// Skybox vertex shader
struct SkyboxVertexOut {
    float4 position [[position]];
    float3 texCoord;
};

vertex SkyboxVertexOut skybox_vertex(VertexIn in [[stage_in]],
                                    constant RenderConstants& constants [[buffer(1)]]) {
    SkyboxVertexOut out;
    
    // Remove translation from view matrix for skybox
    float4x4 rotView = constants.viewMatrix;
    rotView[3][0] = 0.0;
    rotView[3][1] = 0.0;
    rotView[3][2] = 0.0;
    
    float4 pos = constants.projectionMatrix * rotView * float4(in.position, 1.0);
    out.position = pos.xyww; // Set z = w for infinite depth
    out.texCoord = in.position;
    
    return out;
}

fragment float4 skybox_fragment(SkyboxVertexOut in [[stage_in]],
                               constant RenderConstants& constants [[buffer(1)]],
                               texturecube<float> skyboxTexture [[texture(0)]],
                               sampler skyboxSampler [[sampler(0)]]) {
    
    float3 color = skyboxTexture.sample(skyboxSampler, in.texCoord).rgb;
    
    // Dynamic time-based effects
    float timeInfluence = sin(constants.time * 0.05) * 0.1 + 0.9;
    color *= timeInfluence;
    
    // Add subtle gradient for depth
    float gradient = dot(normalize(in.texCoord), float3(0.0, 1.0, 0.0)) * 0.1 + 0.9;
    color *= gradient;
    
    return float4(color, 1.0);
}

// Compute shader for bloom post-processing
kernel void bloom_compute(texture2d<float, access::read> inputTexture [[texture(0)]],
                         texture2d<float, access::write> outputTexture [[texture(1)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= outputTexture.get_width() || gid.y >= outputTexture.get_height()) {
        return;
    }
    
    // Simple box blur for bloom
    float3 color = float3(0.0);
    float weight = 0.0;
    
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            uint2 samplePos = gid + uint2(x, y);
            if (samplePos.x < inputTexture.get_width() && samplePos.y < inputTexture.get_height()) {
                float3 sampleColor = inputTexture.read(samplePos).rgb;
                float luminance = dot(sampleColor, float3(0.299, 0.587, 0.114));
                
                // Only bloom bright pixels
                if (luminance > 1.0) {
                    color += sampleColor;
                    weight += 1.0;
                }
            }
        }
    }
    
    if (weight > 0.0) {
        color /= weight;
    }
    
    outputTexture.write(float4(color, 1.0), gid);
}

// Compute shader for post-processing
kernel void post_process_compute(texture2d<float, access::read> colorTexture [[texture(0)]],
                               texture2d<float, access::read> bloomTexture [[texture(1)]],
                               texture2d<float, access::write> outputTexture [[texture(2)]],
                               constant RenderConstants& constants [[buffer(0)]],
                               uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= outputTexture.get_width() || gid.y >= outputTexture.get_height()) {
        return;
    }
    
    float3 color = colorTexture.read(gid).rgb;
    float3 bloom = bloomTexture.read(gid / 2).rgb; // Bloom is half resolution
    
    // Combine color and bloom
    color += bloom * 0.3;
    
    // Vignette effect
    float2 uv = float2(gid) * constants.screenParams.zw;
    float2 center = uv - 0.5;
    float vignette = 1.0 - smoothstep(0.3, 0.8, length(center));
    color *= vignette;
    
    // Film grain
    float grain = fract(sin(dot(uv + constants.time, float2(12.9898, 78.233))) * 43758.5453);
    color += (grain - 0.5) * 0.02;
    
    outputTexture.write(float4(color, 1.0), gid);
}