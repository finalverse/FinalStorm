// PostProcess.metal - Post-processing effects

#include <metal_stdlib>
#include "Common.metal"

using namespace metal;

// Bloom threshold extraction
kernel void bloom_threshold(texture2d<float, access::read> inTexture [[texture(0)]],
                           texture2d<float, access::write> outTexture [[texture(1)]],
                           constant float& threshold [[buffer(0)]],
                           uint2 gid [[thread_position_in_grid]]) {
    
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
    
    float weights[9] = {0.013, 0.036, 0.082, 0.135, 0.161, 0.135, 0.082, 0.036, 0.013};
    float offsets[9] = {-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0};
    
    float4 color = float4(0.0);
    
    for (int i = 0; i < 9; i++) {
        float2 offset = direction * offsets[i];
        color += inTexture.read(gid + uint2(offset)) * weights[i];
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
