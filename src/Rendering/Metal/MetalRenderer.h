// src/Rendering/Metal/MetalRenderer.h
// Metal renderer interface
// Metal-specific renderer implementation

#pragma once
#include "Rendering/Renderer.h"
#include <memory>

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLRenderCommandEncoder;
@protocol MTLRenderPipelineState;
#else
typedef void* id;
#endif

namespace FinalStorm {

struct MetalRendererImpl;

class MetalRenderer : public Renderer {
public:
    explicit MetalRenderer(void* metalView);
    ~MetalRenderer() override;
    
    // Renderer interface
    void beginFrame() override;
    void endFrame() override;
    std::unique_ptr<RenderContext> createRenderContext() override;
    void resize(uint32_t width, uint32_t height) override;
    void setClearColor(const float4& color) override;
    void setViewProjectionMatrix(const float4x4& view, const float4x4& projection) override;
    
    // Metal-specific
    id getDevice() const;
    id getCurrentEncoder() const;
    id getMeshPipeline() const;
    void updateUniforms(const float4x4& model, const float4x4& view, const float4x4& proj);
    
private:
    bool initialize();
    bool createPipelines();
    
    std::unique_ptr<MetalRendererImpl> impl;
    
    // Uniform structure matching shader
    struct Uniforms {
        float4x4 modelMatrix;
        float4x4 viewMatrix;
        float4x4 projectionMatrix;
        float4x4 normalMatrix;
        float3 cameraPosition;
        float time;
    };
};

} // namespace FinalStorm