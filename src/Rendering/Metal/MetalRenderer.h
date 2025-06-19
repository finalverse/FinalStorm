//
// src/Rendering/Metal/MetalRenderer.h
// Metal-based 3D renderer for Apple platforms using SIMD math types
// Handles render pipelines, shaders, and GPU resource management
//

#pragma once

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#endif

#include "../../Core/Math/MathTypes.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Forward declarations
namespace FinalStorm {
    class MetalMesh;
    class MetalTexture;
    class Scene;
    class SceneNode;
}

namespace FinalStorm {

// Metal-specific shader constants using SIMD types
struct RenderConstants {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 normalMatrix;
    vec3 cameraPosition;
    float time;
    vec3 lightDirection;
    float _padding1;
    vec3 lightColor;
    float lightIntensity;
    vec4 screenParams; // width, height, 1/width, 1/height
};

struct MaterialConstants {
    vec3 albedo;
    float metallic;
    vec3 emission;
    float roughness;
    float ao;
    vec3 _padding;
};

struct ParticleConstants {
    vec3 position;
    float size;
    vec3 velocity;
    float life;
    vec4 color;
    float fadeRate;
    vec3 _padding2;
};

class MetalRenderer {
public:
    MetalRenderer();
    ~MetalRenderer();
    
    // Initialization
    bool initialize();
    void shutdown();
    
    // Frame management
    void beginFrame();
    void endFrame();
    
    // Rendering using MathTypes
    void render(Scene& scene);
    void renderNode(SceneNode& node, const mat4& parentTransform = make_mat4());
    
    // Window management
    void handleResize(int width, int height);
    void setView(void* metalView); // MTKView*
    
    // Resource management (Apple-specific Metal resources)
    std::shared_ptr<MetalMesh> createMesh(const std::vector<float>& vertices, 
                                          const std::vector<uint32_t>& indices);
    std::shared_ptr<MetalTexture> createTexture(const std::string& filename);
    std::shared_ptr<MetalTexture> createTexture(int width, int height, const void* data);
    std::shared_ptr<MetalTexture> createCubemapTexture(const std::vector<std::string>& faces);
    
    // Shader management
    bool loadShaders();
    void setCurrentPipeline(const std::string& pipelineName);
    
    // State management using MathTypes
    void setViewMatrix(const mat4& view);
    void setProjectionMatrix(const mat4& projection);
    void setCameraPosition(const vec3& position);
    void setLightDirection(const vec3& direction);
    void setLightColor(const vec3& color);
    void setTime(float time);
    
    // Getters using MathTypes
    mat4 getViewMatrix() const { return m_viewMatrix; }
    mat4 getProjectionMatrix() const { return m_projectionMatrix; }
    vec3 getCameraPosition() const { return m_cameraPosition; }
    
#ifdef __APPLE__
    // Metal-specific getters
    id<MTLDevice> getDevice() const { return m_device; }
    id<MTLCommandQueue> getCommandQueue() const { return m_commandQueue; }
    id<MTLRenderCommandEncoder> getCurrentEncoder() const { return m_currentEncoder; }
#endif
    
private:
#ifdef __APPLE__
    // Metal resources
    id<MTLDevice> m_device;
    id<MTLCommandQueue> m_commandQueue;
    id<MTLRenderCommandEncoder> m_currentEncoder;
    id<MTLCommandBuffer> m_currentCommandBuffer;
    
    // Render pipelines for different rendering modes
    id<MTLRenderPipelineState> m_defaultPipeline;
    id<MTLRenderPipelineState> m_servicePipeline;
    id<MTLRenderPipelineState> m_particlePipeline;
    id<MTLRenderPipelineState> m_uiPipeline;
    id<MTLRenderPipelineState> m_skyboxPipeline;
    id<MTLRenderPipelineState> m_holographicPipeline;
    
    // Compute pipelines for post-processing
    id<MTLComputePipelineState> m_bloomCompute;
    id<MTLComputePipelineState> m_postProcessCompute;
    
    // Depth and stencil states
    id<MTLDepthStencilState> m_depthState;
    id<MTLDepthStencilState> m_noDepthState;
    
    // Uniform buffers
    id<MTLBuffer> m_constantsBuffer;
    id<MTLBuffer> m_materialBuffer;
    id<MTLBuffer> m_particleBuffer;
    
    // Textures for post-processing
    id<MTLTexture> m_colorTexture;
    id<MTLTexture> m_depthTexture;
    id<MTLTexture> m_bloomTexture;
    
    // Shader library
    id<MTLLibrary> m_shaderLibrary;
    
    // Samplers
    id<MTLSamplerState> m_linearSampler;
    id<MTLSamplerState> m_nearestSampler;
    id<MTLSamplerState> m_anisotropicSampler;
    
    // View
    MTKView* m_metalView;
#endif
    
    // State using MathTypes
    mat4 m_viewMatrix;
    mat4 m_projectionMatrix;
    vec3 m_cameraPosition;
    vec3 m_lightDirection;
    vec3 m_lightColor;
    float m_time;
    
    // Screen parameters
    int m_screenWidth;
    int m_screenHeight;
    
    // Pipeline state cache
    std::unordered_map<std::string, void*> m_pipelineCache; // id<MTLRenderPipelineState>
    
    // Initialization helpers
    bool createDevice();
    bool createCommandQueue();
    bool createRenderPipelines();
    bool createComputePipelines();
    bool createDepthStates();
    bool createBuffers();
    bool createSamplers();
    bool createRenderTargets();
    
    // Rendering helpers
    void updateConstants();
    void setupRenderPass();
    void setDefaultRenderState();
    void bindSamplers();
    
    // Shader compilation helpers
    bool compileShaderLibrary();
    void* createRenderPipeline(const std::string& vertexFunc, const std::string& fragmentFunc,
                              const std::string& label = "");
    void* createComputePipeline(const std::string& kernelFunc, const std::string& label = "");
};

} // namespace FinalStorm