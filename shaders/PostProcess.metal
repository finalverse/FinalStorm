// PostProcess.metal - Post-processing shaders for FinalStorm

#include <metal_stdlib>
#include <simd/simd.h>
#include "Common.metal"

using namespace metal;

// Bloom threshold extraction
kernel void bloom_threshold(texture2d<float, access::read> inTexture [[texture(0)]],
                           texture2d<float, access::write> outTexture [[texture(1)]],
                           constant float& threshold [[buffer(0)]],
                           uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= outTexture.get_width() || gid.y >= outTexture.get_height()) {
        return;
    }
    
    float4 color = inTexture.read(gid);
    float brightness = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (brightness > threshold) {
        float soft = (brightness - threshold) / (1.0 - threshold);
        color.rgb *= soft;
        outTexture.write(color, gid);
    } else {
        outTexture.write(float4(0.0), gid);
    }
}

// Gaussian blur
kernel void gaussian_blur(texture2d<float, access::read> inTexture [[texture(0)]],
                         texture2d<float, access::write> outTexture [[texture(1)]],
                         constant float2& direction [[buffer(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= outTexture.get_width() || gid.y >= outTexture.get_height()) {
        return;
    }
    
    float weights[9] = {0.013, 0.036, 0.082, 0.135, 0.161, 0.135, 0.082, 0.036, 0.013};
    float offsets[9] = {-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
    
    float4 color = float4(0.0);
    
    for (int i = 0; i < 9; i++) {
        float2 offset = direction * offsets[i];
        uint2 samplePos = uint2(float2(gid) + offset);
        
        // Clamp to texture bounds
        samplePos.x = clamp(samplePos.x, 0u, inTexture.get_width() - 1);
        samplePos.y = clamp(samplePos.y, 0u, inTexture.get_height() - 1);
        
        color += inTexture.read(samplePos) * weights[i];
    }
    
    outTexture.write(color, gid);
}

// Tone mapping and final composite
kernel void tone_mapping(texture2d<float, access::read> hdrTexture [[texture(0)]],
                        texture2d<float, access::read> bloomTexture [[texture(1)]],
                        texture2d<float, access::write> ldrTexture [[texture(2)]],
                        constant float& exposure [[buffer(0)]],
                        constant float& bloomIntensity [[buffer(1)]],
                        uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= ldrTexture.get_width() || gid.y >= ldrTexture.get_height()) {
        return;
    }
    
    float4 hdr = hdrTexture.read(gid);
    float4 bloom = bloomTexture.read(gid);
    
    // Add bloom
    hdr.rgb += bloom.rgb * bloomIntensity;
    
    // Exposure adjustment
    hdr.rgb *= exposure;
    
    // ACES tone mapping
    float3 mapped = ACESFilm(hdr.rgb);
    
    // Gamma correction
    mapped = pow(mapped, 1.0 / 2.2);
    
    ldrTexture.write(float4(mapped, hdr.a), gid);
}

// FXAA edge detection
kernel void fxaa_edge_detect(texture2d<float, access::read> inTexture [[texture(0)]],
                            texture2d<float, access::write> outTexture [[texture(1)]],
                            uint2 gid [[thread_position_in_grid]]) {
    
    if (gid.x >= outTexture.get_width() || gid.y >= outTexture.get_height()) {
        return;
    }
    
    // Sample neighboring pixels
    float3 rgbNW = inTexture.read(uint2(gid.x - 1, gid.y - 1)).rgb;
    float3 rgbNE = inTexture.read(uint2(gid.x + 1, gid.y - 1)).rgb;
    float3 rgbSW = inTexture.read(uint2(gid.x - 1, gid.y + 1)).rgb;
    float3 rgbSE = inTexture.read(uint2(gid.x + 1, gid.y + 1)).rgb;
    float3 rgbM = inTexture.read(gid).rgb;
    
    // Convert to luma
    float3 luma = float3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM = dot(rgbM, luma);
    
    // Find edge
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;
    
    // Apply FXAA if edge detected
    if (lumaRange > 0.05) {
        // Simplified FXAA implementation
        float3 color = (rgbNW + rgbNE + rgbSW + rgbSE + rgbM * 4.0) / 8.0;
        outTexture.write(float4(color, 1.0), gid);
    } else {
        outTexture.write(float4(rgbM, 1.0), gid);
    }
}