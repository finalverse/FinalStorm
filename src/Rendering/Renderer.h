// src/Rendering/Renderer.h
// Abstract renderer interface
// Defines the common interface for all rendering backends

#pragma once
#include "Core/MathTypes.h"
#include <memory>

namespace FinalStorm {

class RenderContext;
class Camera;

class Renderer {
public:
    virtual ~Renderer() = default;
    
    // Frame management
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    
    // Context creation
    virtual std::unique_ptr<RenderContext> createRenderContext() = 0;
    
    // Configuration
    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual void setClearColor(const float4& color) = 0;
    
    // View/Projection matrices
    virtual void setViewProjectionMatrix(const float4x4& view, const float4x4& projection) = 0;
    
    // Accessors
    virtual float4x4 getViewMatrix() const { return viewMatrix; }
    virtual float4x4 getProjectionMatrix() const { return projectionMatrix; }
    
protected:
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

} // namespace FinalStorm