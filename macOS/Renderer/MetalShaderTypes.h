//
//  MetalShaderTypes.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#ifndef MetalShaderTypes_h
#define MetalShaderTypes_h

#include <simd/simd.h>

typedef enum BufferIndex
{
    BufferIndexMeshPositions = 0,
    BufferIndexUniforms      = 1,
} BufferIndex;

typedef enum VertexAttribute
{
    VertexAttributePosition  = 0,
    VertexAttributeNormal    = 1,
    VertexAttributeTexcoord  = 2,
} VertexAttribute;

typedef struct
{
    matrix_float4x4 viewProjectionMatrix;
    matrix_float4x4 modelMatrix;
    matrix_float3x3 normalMatrix;
    vector_float4 color;
} Uniforms;

typedef struct
{
    vector_float3 position;
    vector_float3 normal;
    vector_float2 texCoord;
} Vertex;

#endif /* MetalShaderTypes_h */
