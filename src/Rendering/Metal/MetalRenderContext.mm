// src/Rendering/Metal/MetalRenderContext.mm
// Metal render context implementation
// Provides rendering commands for the current frame

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "Rendering/Metal/MetalRenderContext.h"
#include "Rendering/Metal/MetalRenderer.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

class MetalRenderContextImpl {
public:
    MetalRenderer* renderer;
    id<MTLRenderCommandEncoder> encoder;
    std::stack<float4x4> transformStack;
    float4 currentColor;
    
    MetalRenderContextImpl(MetalRenderer* r, id<MTLRenderCommandEncoder> e)
        : renderer(r)
        , encoder(e)
        , currentColor(1.0f, 1.0f, 1.0f, 1.0f) {
        transformStack.push(float4x4(1.0f)); // Identity
    }
};

MetalRenderContext::MetalRenderContext(MetalRenderer* renderer, void* encoder)
    : impl(std::make_unique<MetalRenderContextImpl>(renderer, (__bridge id<MTLRenderCommandEncoder>)encoder)) {
}

MetalRenderContext::~MetalRenderContext() = default;

void MetalRenderContext::setCamera(const Camera& camera) {
    // Camera is set at renderer level
}

void MetalRenderContext::pushTransform(const float4x4& transform) {
    float4x4 current = impl->transformStack.top();
    impl->transformStack.push(current * transform);
}

void MetalRenderContext::popTransform() {
    if (impl->transformStack.size() > 1) {
        impl->transformStack.pop();
    }
}

void MetalRenderContext::translate(const float3& translation) {
    float4x4 current = impl->transformStack.top();
    float4x4 translationMatrix = Math::translate(float4x4(1.0f), translation);
    impl->transformStack.top() = current * translationMatrix;
}

void MetalRenderContext::rotate(const quaternion& rotation) {
    // Convert quaternion to matrix and apply
    float4x4 current = impl->transformStack.top();
    // TODO: Implement quaternion to matrix conversion
}

void MetalRenderContext::scale(const float3& scale) {
    float4x4 current = impl->transformStack.top();
    float4x4 scaleMatrix = Math::scale(float4x4(1.0f), scale);
    impl->transformStack.top() = current * scaleMatrix;
}

void MetalRenderContext::setColor(const float4& color) {
    impl->currentColor = color;
}

void MetalRenderContext::drawMesh(Mesh* mesh) {
    if (!mesh || !impl->encoder) return;
    
    // Update uniforms with current transform
    float4x4 modelMatrix = impl->transformStack.top();
    impl->renderer->updateUniforms(modelMatrix, 
                                  impl->renderer->getViewMatrix(), 
                                  impl->renderer->getProjectionMatrix());
    
    // Set pipeline state
    [impl->encoder setRenderPipelineState:impl->renderer->getMeshPipeline()];
    
    // TODO: Draw mesh vertices
}

void MetalRenderContext::drawCube(float size) {
    // For now, draw a simple cube using immediate mode
    // In production, we'd use a cached mesh
    
    float s = size * 0.5f;
    
    // Define cube vertices
    struct Vertex {
        float3 position;
        float3 normal;
        float2 texcoord;
    };
    
    Vertex vertices[] = {
        // Front face
        {{-s, -s,  s}, {0, 0, 1}, {0, 0}},
        {{ s, -s,  s}, {0, 0, 1}, {1, 0}},
        {{ s,  s,  s}, {0, 0, 1}, {1, 1}},
        {{-s,  s,  s}, {0, 0, 1}, {0, 1}},
        
        // Back face
        {{ s, -s, -s}, {0, 0, -1}, {0, 0}},
        {{-s, -s, -s}, {0, 0, -1}, {1, 0}},
        {{-s,  s, -s}, {0, 0, -1}, {1, 1}},
        {{ s,  s, -s}, {0, 0, -1}, {0, 1}},
        
        // Add other faces...
    };
    
    uint16_t indices[] = {
        // Front face
        0, 1, 2,  2, 3, 0,
        // Back face
        4, 5, 6,  6, 7, 4,
        // Add other faces...
    };
    
    // Create temporary buffers
    id<MTLDevice> device = impl->renderer->getDevice();
    id<MTLBuffer> vertexBuffer = [device newBufferWithBytes:vertices
                                                      length:sizeof(vertices)
                                                     options:MTLResourceStorageModeShared];
    
    id<MTLBuffer> indexBuffer = [device newBufferWithBytes:indices
                                                     length:sizeof(indices)
                                                    options:MTLResourceStorageModeShared];
    
    // Update uniforms
    float4x4 modelMatrix = impl->transformStack.top();
    impl->renderer->updateUniforms(modelMatrix, 
                                  impl->renderer->getViewMatrix(), 
                                  impl->renderer->getProjectionMatrix());
    
    // Set pipeline state
    [impl->encoder setRenderPipelineState:impl->renderer->getMeshPipeline()];
    
    // Set vertex buffer
    [impl->encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
    
    // Draw
    [impl->encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                              indexCount:12 // Just front and back for now
                              indexType:MTLIndexTypeUInt16
                            indexBuffer:indexBuffer
                      indexBufferOffset:0];
}

void MetalRenderContext::drawSphere(float radius, int segments) {
    // Simplified sphere - for now just draw a cube
    drawCube(radius * 2.0f);
}

void MetalRenderContext::drawQuad(float width, float height) {
    float w = width * 0.5f;
    float h = height * 0.5f;
    
    struct Vertex {
        float3 position;
        float3 normal;
        float2 texcoord;
    };
    
    Vertex vertices[] = {
        {{-w, -h, 0}, {0, 0, 1}, {0, 0}},
        {{ w, -h, 0}, {0, 0, 1}, {1, 0}},
        {{ w,  h, 0}, {0, 0, 1}, {1, 1}},
        {{-w,  h, 0}, {0, 0, 1}, {0, 1}},
    };
    
    uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
    
    id<MTLDevice> device = impl->renderer->getDevice();
    id<MTLBuffer> vertexBuffer = [device newBufferWithBytes:vertices
                                                      length:sizeof(vertices)
                                                     options:MTLResourceStorageModeShared];
    
    id<MTLBuffer> indexBuffer = [device newBufferWithBytes:indices
                                                     length:sizeof(indices)
                                                    options:MTLResourceStorageModeShared];
    
    float4x4 modelMatrix = impl->transformStack.top();
    impl->renderer->updateUniforms(modelMatrix, 
                                  impl->renderer->getViewMatrix(), 
                                  impl->renderer->getProjectionMatrix());
    
    [impl->encoder setRenderPipelineState:impl->renderer->getMeshPipeline()];
    [impl->encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
    [impl->encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                              indexCount:6
                              indexType:MTLIndexTypeUInt16
                            indexBuffer:indexBuffer
                      indexBufferOffset:0];
}

void MetalRenderContext::drawWireframeQuad(float width, float height) {
    // TODO: Implement wireframe rendering
    drawQuad(width, height);
}

void MetalRenderContext::drawGrid(float size, float spacing) {
    // TODO: Implement grid rendering
    // For now, just draw a large quad
    drawQuad(size, size);
}

void MetalRenderContext::drawSkybox() {
    // TODO: Implement skybox rendering
}

} // namespace FinalStorm