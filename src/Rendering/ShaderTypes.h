//
//  ShaderTypes.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include "Core/Math/Math.h"

// Shared shader data structures between C++ and Metal/Vulkan/DX12

namespace FinalStorm {

// Vertex formats
struct Vertex {
    float3 position;
    float3 normal;
    float2 texCoord;
    float4 color;
};

struct SkinnedVertex {
    float3 position;
    float3 normal;
    float2 texCoord;
    float4 boneWeights;
    uint4 boneIndices;
};

// Uniform buffer structures
struct PerFrameUniforms {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 viewProjectionMatrix;
    float3 cameraPosition;
    float time;
    float3 lightDirection;
    float padding1;
    float4 lightColor;
    float4 ambientColor;
};

struct PerObjectUniforms {
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float4 materialColor;
    float4 materialProperties; // x: metallic, y: roughness, z: ao, w: unused
};

// Buffer indices
enum BufferIndex : uint32_t {
    BufferIndexVertices = 0,
    BufferIndexPerFrame = 1,
    BufferIndexPerObject = 2,
    BufferIndexInstances = 3,
};

// Vertex attributes
enum VertexAttribute : uint32_t {
    VertexAttributePosition = 0,
    VertexAttributeNormal = 1,
    VertexAttributeTexCoord = 2,
    VertexAttributeColor = 3,
    VertexAttributeBoneWeights = 4,
    VertexAttributeBoneIndices = 5,
};

// Texture indices
enum TextureIndex : uint32_t {
    TextureIndexColor = 0,
    TextureIndexNormal = 1,
    TextureIndexMetallicRoughness = 2,
    TextureIndexAmbientOcclusion = 3,
    TextureIndexEmissive = 4,
};

} // namespace FinalStorm
