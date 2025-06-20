// src/Rendering/Metal/MetalRenderContext.h
// Metal render context
// Metal-specific implementation of RenderContext

#pragma once
#include "Rendering/RenderContext.h"
#include <memory>

namespace FinalStorm {

class MetalRenderer;
class MetalRenderContextImpl;

class MetalRenderContext : public RenderContext {
public:
    MetalRenderContext(MetalRenderer* renderer, void* encoder);
    ~MetalRenderContext() override;
    
    // RenderContext interface
    void setCamera(const Camera& camera) override;
    void pushTransform(const float4x4& transform) override;
    void popTransform() override;
    void translate(const float3& translation) override;
    void rotate(const quaternion& rotation) override;
    void scale(const float3& scale) override;
    void setColor(const float4& color) override;
    void drawMesh(Mesh* mesh) override;
    void drawCube(float size) override;
    void drawSphere(float radius, int segments) override;
    void drawQuad(float width, float height) override;
    void drawWireframeQuad(float width, float height) override;
    void drawGrid(float size, float spacing) override;
    void drawSkybox() override;
    
private:
    std::unique_ptr<MetalRenderContextImpl> impl;
};

} // namespace FinalStorm