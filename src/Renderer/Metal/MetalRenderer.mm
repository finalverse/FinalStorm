//
// src/Rendering/Metal/MetalRenderer.mm
// Metal-based 3D renderer implementation for Apple platforms
// Uses SIMD math types and Metal Performance Shaders for optimal GPU performance
//

#include "MetalRenderer.h"
#include "MetalMesh.h"
#include "MetalTexture.h"
#include "../../Scene/Scene.h"
#include "../../Scene/SceneNode.h"
#include "../../Scene/CameraController.h"

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <Foundation/Foundation.h>
#endif

#include <iostream>
#include <fstream>

namespace FinalStorm {

MetalRenderer::MetalRenderer()
#ifdef __APPLE__
    : m_device(nil)
    , m_commandQueue(nil)
    , m_currentEncoder(nil)
    , m_currentCommandBuffer(nil)
    , m_defaultPipeline(nil)
    , m_servicePipeline(nil)
    , m_particlePipeline(nil)
    , m_uiPipeline(nil)
    , m_skyboxPipeline(nil)
    , m_holographicPipeline(nil)
    , m_bloomCompute(nil)
    , m_postProcessCompute(nil)
    , m_depthState(nil)
    , m_noDepthState(nil)
    , m_constantsBuffer(nil)
    , m_materialBuffer(nil)
    , m_particleBuffer(nil)
    , m_colorTexture(nil)
    , m_depthTexture(nil)
    , m_bloomTexture(nil)
    , m_shaderLibrary(nil)
    , m_linearSampler(nil)
    , m_nearestSampler(nil)
    , m_anisotropicSampler(nil)
    , m_metalView(nil)
#endif
    , m_viewMatrix(make_mat4())
    , m_projectionMatrix(make_mat4())
    , m_cameraPosition(vec3_zero())
    , m_lightDirection(make_vec3(0.0f, -1.0f, 0.0f))
    , m_lightColor(vec3_one())
    , m_time(0.0f)
    , m_screenWidth(1920)
    , m_screenHeight(1080)
{
}

MetalRenderer::~MetalRenderer() {
    shutdown();
}

bool MetalRenderer::initialize() {
    std::cout << "MetalRenderer: Initializing Metal renderer..." << std::endl;
    
#ifdef __APPLE__
    if (!createDevice()) {
        std::cerr << "MetalRenderer: Failed to create Metal device!" << std::endl;
        return false;
    }
    
    if (!createCommandQueue()) {
        std::cerr << "MetalRenderer: Failed to create command queue!" << std::endl;
        return false;
    }
    
    if (!createBuffers()) {
        std::cerr << "MetalRenderer: Failed to create buffers!" << std::endl;
        return false;
    }
    
    if (!createSamplers()) {
        std::cerr << "MetalRenderer: Failed to create samplers!" << std::endl;
        return false;
    }
    
    if (!createDepthStates()) {
        std::cerr << "MetalRenderer: Failed to create depth states!" << std::endl;
        return false;
    }
    
    if (!loadShaders()) {
        std::cerr << "MetalRenderer: Failed to load shaders!" << std::endl;
        return false;
    }
    
    if (!createRenderPipelines()) {
        std::cerr << "MetalRenderer: Failed to create render pipelines!" << std::endl;
        return false;
    }
    
    if (!createComputePipelines()) {
        std::cerr << "MetalRenderer: Failed to create compute pipelines!" << std::endl;
        return false;
    }
    
    // Set default projection matrix using MathTypes
    m_projectionMatrix = perspective(radians(60.0f), 16.0f/9.0f, 0.1f, 1000.0f);
    
    std::cout << "MetalRenderer: Metal renderer initialized successfully!" << std::endl;
    return true;
#else
    std::cout << "MetalRenderer: Metal not available on this platform" << std::endl;
    return false;
#endif
}

void MetalRenderer::shutdown() {
#ifdef __APPLE__
    m_pipelineCache.clear();
    
    m_device = nil;
    m_commandQueue = nil;
    m_currentEncoder = nil;
    m_currentCommandBuffer = nil;
    m_defaultPipeline = nil;
    m_servicePipeline = nil;
    m_particlePipeline = nil;
    m_uiPipeline = nil;
    m_skyboxPipeline = nil;
    m_holographicPipeline = nil;
    m_bloomCompute = nil;
    m_postProcessCompute = nil;
    m_depthState = nil;
    m_noDepthState = nil;
    m_constantsBuffer = nil;
    m_materialBuffer = nil;
    m_particleBuffer = nil;
    m_colorTexture = nil;
    m_depthTexture = nil;
    m_bloomTexture = nil;
    m_shaderLibrary = nil;
    m_linearSampler = nil;
    m_nearestSampler = nil;
    m_anisotropicSampler = nil;
    m_metalView = nil;
#endif
}

void MetalRenderer::beginFrame() {
#ifdef __APPLE__
    if (!m_commandQueue || !m_metalView) {
        return;
    }
    
    m_currentCommandBuffer = [m_commandQueue commandBuffer];
    if (!m_currentCommandBuffer) {
        std::cerr << "MetalRenderer: Failed to create command buffer!" << std::endl;
        return;
    }
    
    setupRenderPass();
    updateConstants();
#endif
}

void MetalRenderer::endFrame() {
#ifdef __APPLE__
    if (m_currentEncoder) {
        [m_currentEncoder endEncoding];
        m_currentEncoder = nil;
    }
    
    if (m_currentCommandBuffer && m_metalView.currentDrawable) {
        [m_currentCommandBuffer presentDrawable:m_metalView.currentDrawable];
        [m_currentCommandBuffer commit];
        m_currentCommandBuffer = nil;
    }
#endif
}

void MetalRenderer::render(Scene& scene) {
#ifdef __APPLE__
    if (!m_currentEncoder || !scene.getRootNode()) {
        return;
    }
    
    // Update camera matrices from scene using MathTypes
    auto camera = scene.getCameraController();
    if (camera) {
        setViewMatrix(camera->getViewMatrix());
        setProjectionMatrix(camera->getProjectionMatrix());
        setCameraPosition(camera->getPosition());
    }
    
    // Set default render state
    setDefaultRenderState();
    
    // Render the scene graph
    renderNode(*scene.getRootNode());
#endif
}

void MetalRenderer::renderNode(SceneNode& node, const mat4& parentTransform) {
#ifdef __APPLE__
    // Calculate world transform using MathTypes
    mat4 worldTransform = parentTransform * node.getLocalTransform();
    
    // Render this node if it has renderable content
    auto renderable = node.getRenderable();
    if (renderable && renderable->isVisible()) {
        // Update model matrix in constants using SIMD
        RenderConstants* constants = static_cast<RenderConstants*>([m_constantsBuffer contents]);
        constants->modelMatrix = worldTransform;
        constants->normalMatrix = transpose(inverse(worldTransform));
        
        // Render the mesh
        renderable->render(*this);
    }
    
    // Recursively render children
    for (auto& child : node.getChildren()) {
        renderNode(*child, worldTransform);
    }
#endif
}

void MetalRenderer::handleResize(int width, int height) {
    if (width > 0 && height > 0) {
        m_screenWidth = width;
        m_screenHeight = height;
        
        float aspect = static_cast<float>(width) / height;
        // Update projection matrix using MathTypes
        m_projectionMatrix = perspective(radians(60.0f), aspect, 0.1f, 1000.0f);
        
#ifdef __APPLE__
        // Recreate render targets if needed
        createRenderTargets();
#endif
    }
}

void MetalRenderer::setView(void* metalView) {
#ifdef __APPLE__
    m_metalView = (__bridge MTKView*)metalView;
    if (m_metalView) {
        m_metalView.device = m_device;
        m_metalView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
        m_metalView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
        m_metalView.sampleCount = 1; // No MSAA for now
        
        // Enable wide color if available
        if (@available(macOS 10.12, iOS 10.0, *)) {
            m_metalView.colorspace = CGColorSpaceCreateWithName(kCGColorSpaceDisplayP3);
        }
    }
#endif
}

std::shared_ptr<MetalMesh> MetalRenderer::createMesh(const std::vector<float>& vertices, 
                                                     const std::vector<uint32_t>& indices) {
#ifdef __APPLE__
    if (!m_device) {
        return nullptr;
    }
    
    return std::make_shared<MetalMesh>(m_device, vertices, indices);
#else
    return nullptr;
#endif
}

std::shared_ptr<MetalTexture> MetalRenderer::createTexture(const std::string& filename) {
#ifdef __APPLE__
    if (!m_device) {
        return nullptr;
    }
    
    return std::make_shared<MetalTexture>(m_device, filename);
#else
    return nullptr;
#endif
}

std::shared_ptr<MetalTexture> MetalRenderer::createTexture(int width, int height, const void* data) {
#ifdef __APPLE__
    if (!m_device) {
        return nullptr;
    }
    
    return std::make_shared<MetalTexture>(m_device, width, height, data);
#else
    return nullptr;
#endif
}

bool MetalRenderer::loadShaders() {
#ifdef __APPLE__
    // Load shader library from app bundle
    NSBundle* bundle = [NSBundle mainBundle];
    NSString* libraryPath = [bundle pathForResource:@"FinalStorm" ofType:@"metallib"];
    
    if (!libraryPath) {
        // Try loading from different locations
        libraryPath = [bundle pathForResource:@"default" ofType:@"metallib"];
    }
    
    if (!libraryPath) {
        std::cerr << "MetalRenderer: Could not find shader library in app bundle!" << std::endl;
        return false;
    }
    
    NSError* error = nil;
    m_shaderLibrary = [m_device newLibraryWithFile:libraryPath error:&error];
    
    if (!m_shaderLibrary) {
        std::cerr << "MetalRenderer: Failed to load shader library: " 
                  << error.localizedDescription.UTF8String << std::endl;
        return false;
    }
    
    std::cout << "MetalRenderer: Shader library loaded successfully" << std::endl;
    return true;
#else
    return false;
#endif
}

// State setters using MathTypes
void MetalRenderer::setViewMatrix(const mat4& view) {
    m_viewMatrix = view;
}

void MetalRenderer::setProjectionMatrix(const mat4& projection) {
    m_projectionMatrix = projection;
}

void MetalRenderer::setCameraPosition(const vec3& position) {
    m_cameraPosition = position;
}

void MetalRenderer::setLightDirection(const vec3& direction) {
    m_lightDirection = normalize(direction);
}

void MetalRenderer::setLightColor(const vec3& color) {
    m_lightColor = color;
}

void MetalRenderer::setTime(float time) {
    m_time = time;
}

// Private methods
bool MetalRenderer::createDevice() {
#ifdef __APPLE__
    m_device = MTLCreateSystemDefaultDevice();
    if (m_device) {
        std::cout << "MetalRenderer: Created Metal device: " 
                  << m_device.name.UTF8String << std::endl;
    }
    return m_device != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createCommandQueue() {
#ifdef __APPLE__
    if (!m_device) {
        return false;
    }
    
    m_commandQueue = [m_device newCommandQueue];
    return m_commandQueue != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createRenderPipelines() {
#ifdef __APPLE__
    if (!m_device || !m_shaderLibrary) {
        return false;
    }
    
    // Create default pipeline
    m_defaultPipeline = (__bridge id<MTLRenderPipelineState>)createRenderPipeline(
        "default_vertex", "default_fragment", "Default Pipeline");
    
    // Create service visualization pipeline
    m_servicePipeline = (__bridge id<MTLRenderPipelineState>)createRenderPipeline(
        "default_vertex", "service_fragment", "Service Pipeline");
    
    // Create particle pipeline
    m_particlePipeline = (__bridge id<MTLRenderPipelineState>)createRenderPipeline(
        "particle_vertex", "particle_fragment", "Particle Pipeline");
    
    // Create UI pipeline
    m_uiPipeline = (__bridge id<MTLRenderPipelineState>)createRenderPipeline(
        "unlit_vertex", "holographic_fragment", "UI Pipeline");
    
    // Create skybox pipeline
    m_skyboxPipeline = (__bridge id<MTLRenderPipelineState>)createRenderPipeline(
        "skybox_vertex", "skybox_fragment", "Skybox Pipeline");
    
    return m_defaultPipeline != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createComputePipelines() {
#ifdef __APPLE__
    if (!m_device || !m_shaderLibrary) {
        return false;
    }
    
    // Create bloom compute pipeline
    m_bloomCompute = (__bridge id<MTLComputePipelineState>)createComputePipeline(
        "bloom_compute", "Bloom Compute");
    
    // Create post-process compute pipeline
    m_postProcessCompute = (__bridge id<MTLComputePipelineState>)createComputePipeline(
        "post_process_compute", "Post Process Compute");
    
    return m_bloomCompute != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createDepthStates() {
#ifdef __APPLE__
    if (!m_device) {
        return false;
    }
    
    // Create depth test enabled state
    MTLDepthStencilDescriptor* depthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
    depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthDescriptor.depthWriteEnabled = YES;
    m_depthState = [m_device newDepthStencilStateWithDescriptor:depthDescriptor];
    
    // Create depth test disabled state
    MTLDepthStencilDescriptor* noDepthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
    noDepthDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
    noDepthDescriptor.depthWriteEnabled = NO;
    m_noDepthState = [m_device newDepthStencilStateWithDescriptor:noDepthDescriptor];
    
    return m_depthState != nil && m_noDepthState != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createBuffers() {
#ifdef __APPLE__
    if (!m_device) {
        return false;
    }
    
    // Create constants buffer
    m_constantsBuffer = [m_device newBufferWithLength:sizeof(RenderConstants) 
                                              options:MTLResourceStorageModeShared];
    
    // Create material buffer
    m_materialBuffer = [m_device newBufferWithLength:sizeof(MaterialConstants) 
                                             options:MTLResourceStorageModeShared];
    
    // Create particle buffer
    m_particleBuffer = [m_device newBufferWithLength:sizeof(ParticleConstants) * 10000 
                                             options:MTLResourceStorageModeShared];
    
    return m_constantsBuffer != nil && m_materialBuffer != nil && m_particleBuffer != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createSamplers() {
#ifdef __APPLE__
    if (!m_device) {
        return false;
    }
    
    // Linear sampler
    MTLSamplerDescriptor* linearDesc = [[MTLSamplerDescriptor alloc] init];
    linearDesc.minFilter = MTLSamplerMinMagFilterLinear;
    linearDesc.magFilter = MTLSamplerMinMagFilterLinear;
    linearDesc.mipFilter = MTLSamplerMipFilterLinear;
    linearDesc.sAddressMode = MTLSamplerAddressModeRepeat;
    linearDesc.tAddressMode = MTLSamplerAddressModeRepeat;
    m_linearSampler = [m_device newSamplerStateWithDescriptor:linearDesc];
    
    // Nearest sampler
    MTLSamplerDescriptor* nearestDesc = [[MTLSamplerDescriptor alloc] init];
    nearestDesc.minFilter = MTLSamplerMinMagFilterNearest;
    nearestDesc.magFilter = MTLSamplerMinMagFilterNearest;
    nearestDesc.mipFilter = MTLSamplerMipFilterNearest;
    nearestDesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
    nearestDesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
    m_nearestSampler = [m_device newSamplerStateWithDescriptor:nearestDesc];
    
    // Anisotropic sampler
    MTLSamplerDescriptor* anisotropicDesc = [[MTLSamplerDescriptor alloc] init];
    anisotropicDesc.minFilter = MTLSamplerMinMagFilterLinear;
    anisotropicDesc.magFilter = MTLSamplerMinMagFilterLinear;
    anisotropicDesc.mipFilter = MTLSamplerMipFilterLinear;
    anisotropicDesc.maxAnisotropy = 16;
    anisotropicDesc.sAddressMode = MTLSamplerAddressModeRepeat;
    anisotropicDesc.tAddressMode = MTLSamplerAddressModeRepeat;
    m_anisotropicSampler = [m_device newSamplerStateWithDescriptor:anisotropicDesc];
    
    return m_linearSampler != nil && m_nearestSampler != nil && m_anisotropicSampler != nil;
#else
    return false;
#endif
}

bool MetalRenderer::createRenderTargets() {
#ifdef __APPLE__
    if (!m_device || m_screenWidth <= 0 || m_screenHeight <= 0) {
        return false;
    }
    
    // Create color texture for post-processing
    MTLTextureDescriptor* colorDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                         width:m_screenWidth
                                                                                        height:m_screenHeight
                                                                                     mipmapped:NO];
    colorDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    m_colorTexture = [m_device newTextureWithDescriptor:colorDesc];
    
    // Create depth texture
    MTLTextureDescriptor* depthDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                                                         width:m_screenWidth
                                                                                        height:m_screenHeight
                                                                                     mipmapped:NO];
    depthDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    m_depthTexture = [m_device newTextureWithDescriptor:depthDesc];
    
    // Create bloom texture (half resolution)
    MTLTextureDescriptor* bloomDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA16Float
                                                                                          width:m_screenWidth / 2
                                                                                         height:m_screenHeight / 2
                                                                                      mipmapped:YES];
    bloomDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    m_bloomTexture = [m_device newTextureWithDescriptor:bloomDesc];
    
    return m_colorTexture != nil && m_depthTexture != nil && m_bloomTexture != nil;
#else
    return false;
#endif
}

void MetalRenderer::updateConstants() {
#ifdef __APPLE__
    if (!m_constantsBuffer) {
        return;
    }
    
    // Update render constants using SIMD types
    RenderConstants* constants = static_cast<RenderConstants*>([m_constantsBuffer contents]);
    constants->viewMatrix = m_viewMatrix;
    constants->projectionMatrix = m_projectionMatrix;
    constants->cameraPosition = m_cameraPosition;
    constants->time = m_time;
    constants->lightDirection = m_lightDirection;
    constants->lightColor = m_lightColor;
    constants->lightIntensity = 1.0f;
    constants->screenParams = make_vec4(
        static_cast<float>(m_screenWidth), 
        static_cast<float>(m_screenHeight),
        1.0f / static_cast<float>(m_screenWidth),
        1.0f / static_cast<float>(m_screenHeight)
    );
#endif
}

void MetalRenderer::setupRenderPass() {
#ifdef __APPLE__
    if (!m_metalView || !m_currentCommandBuffer) {
        return;
    }
    
    MTLRenderPassDescriptor* renderPassDescriptor = m_metalView.currentRenderPassDescriptor;
    if (!renderPassDescriptor) {
        return;
    }
    
    // Clear to a dark blue color for space-like environment
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.05, 0.05, 0.15, 1.0);
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    
    // Set up depth buffer
    renderPassDescriptor.depthAttachment.clearDepth = 1.0;
    renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    
    m_currentEncoder = [m_currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    if (m_currentEncoder) {
        m_currentEncoder.label = @"Main Render Pass";
    }
#endif
}

void MetalRenderer::setDefaultRenderState() {
#ifdef __APPLE__
    if (!m_currentEncoder) {
        return;
    }
    
    // Set default pipeline
    if (m_defaultPipeline) {
        [m_currentEncoder setRenderPipelineState:m_defaultPipeline];
    }
    
    // Set depth state
    if (m_depthState) {
        [m_currentEncoder setDepthStencilState:m_depthState];
    }
    
    // Bind constant buffers
    if (m_constantsBuffer) {
        [m_currentEncoder setVertexBuffer:m_constantsBuffer offset:0 atIndex:1];
        [m_currentEncoder setFragmentBuffer:m_constantsBuffer offset:0 atIndex:1];
    }
    
    if (m_materialBuffer) {
        [m_currentEncoder setFragmentBuffer:m_materialBuffer offset:0 atIndex:2];
    }
    
    // Bind default samplers
    bindSamplers();
#endif
}

void MetalRenderer::bindSamplers() {
#ifdef __APPLE__
    if (!m_currentEncoder) {
        return;
    }
    
    if (m_linearSampler) {
        [m_currentEncoder setFragmentSamplerState:m_linearSampler atIndex:0];
    }
    
    if (m_anisotropicSampler) {
        [m_currentEncoder setFragmentSamplerState:m_anisotropicSampler atIndex:1];
    }
    
    if (m_nearestSampler) {
        [m_currentEncoder setFragmentSamplerState:m_nearestSampler atIndex:2];
    }
#endif
}

void* MetalRenderer::createRenderPipeline(const std::string& vertexFunc, const std::string& fragmentFunc,
                                         const std::string& label) {
#ifdef __APPLE__
    if (!m_device || !m_shaderLibrary) {
        return nil;
    }
    
    NSString* vertexFuncName = [NSString stringWithUTF8String:vertexFunc.c_str()];
    NSString* fragmentFuncName = [NSString stringWithUTF8String:fragmentFunc.c_str()];
    
    id<MTLFunction> vertexFunction = [m_shaderLibrary newFunctionWithName:vertexFuncName];
    id<MTLFunction> fragmentFunction = [m_shaderLibrary newFunctionWithName:fragmentFuncName];
    
    if (!vertexFunction || !fragmentFunction) {
        std::cerr << "MetalRenderer: Could not find shader functions: " 
                  << vertexFunc << ", " << fragmentFunc << std::endl;
        return nil;
    }
    
    MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.label = [NSString stringWithUTF8String:label.c_str()];
    descriptor.vertexFunction = vertexFunction;
    descriptor.fragmentFunction = fragmentFunction;
    descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    descriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    
    // Set up vertex descriptor for standard vertex layout
    MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
    
    // Position (float3)
    vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
    vertexDescriptor.attributes[0].offset = 0;
    vertexDescriptor.attributes[0].bufferIndex = 0;
    
    // Normal (float3)
    vertexDescriptor.attributes[1].format = MTLVertexFormatFloat3;
    vertexDescriptor.attributes[1].offset = 12;
    vertexDescriptor.attributes[1].bufferIndex = 0;
    
    // TexCoord (float2)
    vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
    vertexDescriptor.attributes[2].offset = 24;
    vertexDescriptor.attributes[2].bufferIndex = 0;
    
    // Color (float3)
    vertexDescriptor.attributes[3].format = MTLVertexFormatFloat3;
    vertexDescriptor.attributes[3].offset = 32;
    vertexDescriptor.attributes[3].bufferIndex = 0;
    
    // Buffer layout
    vertexDescriptor.layouts[0].stride = 44; // 3+3+2+3 floats * 4 bytes
    vertexDescriptor.layouts[0].stepRate = 1;
    vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
    descriptor.vertexDescriptor = vertexDescriptor;
    
    // Enable blending for transparent objects
    descriptor.colorAttachments[0].blendingEnabled = YES;
    descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    
    NSError* error = nil;
    id<MTLRenderPipelineState> pipeline = [m_device newRenderPipelineStateWithDescriptor:descriptor error:&error];
    
    if (!pipeline) {
        std::cerr << "MetalRenderer: Failed to create render pipeline '" << label << "': " 
                  << error.localizedDescription.UTF8String << std::endl;
        return nil;
    }
    
    std::cout << "MetalRenderer: Created render pipeline: " << label << std::endl;
    return (__bridge void*)pipeline;
#else
    return nullptr;
#endif
}

void* MetalRenderer::createComputePipeline(const std::string& kernelFunc, const std::string& label) {
#ifdef __APPLE__
    if (!m_device || !m_shaderLibrary) {
        return nil;
    }
    
    NSString* kernelFuncName = [NSString stringWithUTF8String:kernelFunc.c_str()];
    id<MTLFunction> kernelFunction = [m_shaderLibrary newFunctionWithName:kernelFuncName];
    
    if (!kernelFunction) {
        std::cerr << "MetalRenderer: Could not find compute kernel: " << kernelFunc << std::endl;
        return nil;
    }
    
    NSError* error = nil;
    id<MTLComputePipelineState> pipeline = [m_device newComputePipelineStateWithFunction:kernelFunction error:&error];
    
    if (!pipeline) {
        std::cerr << "MetalRenderer: Failed to create compute pipeline '" << label << "': " 
                  << error.localizedDescription.UTF8String << std::endl;
        return nil;
    }
    
    std::cout << "MetalRenderer: Created compute pipeline: " << label << std::endl;
    return (__bridge void*)pipeline;
#else
    return nullptr;
#endif
}

void MetalRenderer::setCurrentPipeline(const std::string& pipelineName) {
#ifdef __APPLE__
    if (!m_currentEncoder) {
        return;
    }
    
    // Set the appropriate pipeline based on name
    if (pipelineName == "default" && m_defaultPipeline) {
        [m_currentEncoder setRenderPipelineState:m_defaultPipeline];
    }
    else if (pipelineName == "service" && m_servicePipeline) {
        [m_currentEncoder setRenderPipelineState:m_servicePipeline];
    }
    else if (pipelineName == "particle" && m_particlePipeline) {
        [m_currentEncoder setRenderPipelineState:m_particlePipeline];
    }
    else if (pipelineName == "ui" && m_uiPipeline) {
        [m_currentEncoder setRenderPipelineState:m_uiPipeline];
    }
    else if (pipelineName == "skybox" && m_skyboxPipeline) {
        [m_currentEncoder setRenderPipelineState:m_skyboxPipeline];
        [m_currentEncoder setDepthStencilState:m_noDepthState]; // Skybox renders without depth
    }
    else if (pipelineName == "holographic" && m_holographicPipeline) {
        [m_currentEncoder setRenderPipelineState:m_holographicPipeline];
    }
#endif
}

} // namespace FinalStorm