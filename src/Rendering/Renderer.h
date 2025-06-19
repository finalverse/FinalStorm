// Rendering/Renderer.h
#pragma once

#include "Core/MathTypes.h"
#include <memory>
#include <vector>

namespace FinalStorm {

// Forward declarations
class Mesh;
class Material;
class Texture;
class Camera;
class Light;
class RenderTarget;

enum class CullMode {
    None,
    Front,
    Back
};

enum class BlendMode {
    Opaque,
    Alpha,
    Additive,
    Multiply
};

enum class DepthTest {
    Always,
    Never,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Equal,
    NotEqual
};

struct RenderState {
    CullMode cullMode = CullMode::Back;
    BlendMode blendMode = BlendMode::Opaque;
    DepthTest depthTest = DepthTest::Less;
    bool depthWrite = true;
    bool wireframe = false;
};

class Renderer {
public:
    Renderer() = default;
    virtual ~Renderer() = default;
    
    // Initialization
    virtual bool initialize(void* nativeHandle, int width, int height) = 0;
    virtual void shutdown() = 0;
    
    // Frame rendering
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void present() = 0;
    
    // Render targets
    virtual void setRenderTarget(RenderTarget* target) = 0;
    virtual void clearRenderTarget(const vec4& clearColor = vec4(0, 0, 0, 1)) = 0;
    
    // Camera and transforms
    virtual void setCamera(Camera* camera) = 0;
    virtual void setTransform(const mat4& transform) = 0;
    virtual void pushTransform() = 0;
    virtual void popTransform() = 0;
    
    // Material and state
    virtual void setMaterial(std::shared_ptr<Material> material) = 0;
    virtual void setRenderState(const RenderState& state) = 0;
    
    // Drawing
    virtual void drawMesh(std::shared_ptr<Mesh> mesh) = 0;
    virtual void drawMeshInstanced(std::shared_ptr<Mesh> mesh, const std::vector<mat4>& transforms) = 0;
    
    // Lighting
    virtual void setAmbientLight(const vec3& color) = 0;
    virtual void addLight(Light* light) = 0;
    virtual void clearLights() = 0;
    
    // Debug rendering
    virtual void drawLine(const vec3& start, const vec3& end, const vec4& color) = 0;
    virtual void drawBox(const vec3& min, const vec3& max, const vec4& color) = 0;
    virtual void drawSphere(const vec3& center, float radius, const vec4& color) = 0;
    
    // Viewport
    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void getViewport(int& x, int& y, int& width, int& height) const = 0;
    
    // Capabilities
    virtual bool supportsInstancing() const = 0;
    virtual bool supportsTessellation() const = 0;
    virtual bool supportsComputeShaders() const = 0;
    
    // Statistics
    struct Stats {
        int drawCalls = 0;
        int triangles = 0;
        int vertices = 0;
        float frameTime = 0.0f;
    };
    
    virtual const Stats& getStats() const { return m_stats; }
    virtual void resetStats() { m_stats = Stats(); }
    
protected:
    mat4 m_viewMatrix = mat4(1.0f);
    mat4 m_projectionMatrix = mat4(1.0f);
    mat4 m_modelMatrix = mat4(1.0f);
    
    std::vector<mat4> m_transformStack;
    
    Stats m_stats;
    
    int m_viewportX = 0;
    int m_viewportY = 0;
    int m_viewportWidth = 0;
    int m_viewportHeight = 0;
};

// Platform-specific renderer implementations
#ifdef __APPLE__
class MetalRenderer : public Renderer {
public:
    MetalRenderer();
    ~MetalRenderer();
    
    bool initialize(void* nativeHandle, int width, int height) override;
    void shutdown() override;
    
    void beginFrame() override;
    void endFrame() override;
    void present() override;
    
    void setRenderTarget(RenderTarget* target) override;
    void clearRenderTarget(const vec4& clearColor) override;
    
    void setCamera(Camera* camera) override;
    void setTransform(const mat4& transform) override;
    void pushTransform() override;
    void popTransform() override;
    
    void setMaterial(std::shared_ptr<Material> material) override;
    void setRenderState(const RenderState& state) override;
    
    void drawMesh(std::shared_ptr<Mesh> mesh) override;
    void drawMeshInstanced(std::shared_ptr<Mesh> mesh, const std::vector<mat4>& transforms) override;
    
    void setAmbientLight(const vec3& color) override;
    void addLight(Light* light) override;
    voi