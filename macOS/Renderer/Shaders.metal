//
//  Shaders.metal
//  FinalStorm macOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

#include "MetalShaderTypes.h"

/*
typedef struct
{
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
} Vertex;
*/

typedef struct
{
    float4 position [[position]];
    float3 worldPosition;
    float3 worldNormal;
    float2 texCoord;
} ColorInOut;

vertex ColorInOut vertexShader(Vertex in [[stage_in]],
                               constant Uniforms & uniforms [[buffer(BufferIndexUniforms)]])
{
    ColorInOut out;
    
    float4 position = float4(in.position, 1.0);
    out.worldPosition = (uniforms.modelMatrix * position).xyz;
    out.position = uniforms.viewProjectionMatrix * float4(out.worldPosition, 1.0);
    out.worldNormal = uniforms.normalMatrix * in.normal;
    out.texCoord = in.texCoord;
    
    return out;
}

fragment float4 fragmentShader(ColorInOut in [[stage_in]],
                               constant Uniforms & uniforms [[buffer(BufferIndexUniforms)]])
{
    float3 normal = normalize(in.worldNormal);
    
    // Simple directional light
    float3 lightDirection = normalize(float3(1, -1, -1));
    float3 lightColor = float3(1.0, 1.0, 1.0);
    
    // Ambient
    float3 ambient = 0.2 * uniforms.color.rgb;
    
    // Diffuse
    float diff = max(dot(normal, -lightDirection), 0.0);
    float3 diffuse = diff * lightColor * uniforms.color.rgb;
    
    // Result
    float3 result = ambient + diffuse;
    
    return float4(result, uniforms.color.a);
}

