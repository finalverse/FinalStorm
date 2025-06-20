// src/Rendering/Metal/MetalMesh.mm
// Metal mesh implementation
// Handles vertex and index buffers for 3D meshes

#import <Metal/Metal.h>
#include "Rendering/Metal/MetalMesh.h"
#include "Rendering/Metal/MetalRenderer.h"

namespace FinalStorm {

struct MetalMeshImpl {
    id<MTLBuffer> vertexBuffer;
    id<MTLBuffer> indexBuffer;
    uint32_t vertexCount;
    uint32_t indexCount;
    MTLPrimitiveType primitiveType;
};

MetalMesh::MetalMesh()
    : impl(std::make_unique<MetalMeshImpl>()) {
    impl->vertexCount = 0;
    impl->indexCount = 0;
    impl->primitiveType = MTLPrimitiveTypeTriangle;
}

MetalMesh::~MetalMesh() = default;

void MetalMesh::uploadVertices(const void* data, size_t size, uint32_t count) {
    // TODO: Get device from renderer
    // For now, this is a placeholder
    impl->vertexCount = count;
}

void MetalMesh::uploadIndices(const void* data, size_t size, uint32_t count) {
    // TODO: Get device from renderer
    impl->indexCount = count;
}

uint32_t MetalMesh::getVertexCount() const {
    return impl->vertexCount;
}

uint32_t MetalMesh::getIndexCount() const {
    return impl->indexCount;
}

void* MetalMesh::getVertexBuffer() const {
    return (__bridge void*)impl->vertexBuffer;
}

void* MetalMesh::getIndexBuffer() const {
    return (__bridge void*)impl->indexBuffer;
}

} // namespace FinalStorm