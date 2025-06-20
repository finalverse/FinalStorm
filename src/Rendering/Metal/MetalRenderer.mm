// src/Rendering/Metal/MetalRenderer.mm
// Metal renderer implementation for macOS/iOS
// Provides the main rendering interface using Metal API

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <simd/simd.h>
#include "Rendering/Metal/MetalRenderer.h"
#include "Rendering/Metal/MetalRenderContext.h"
#include "Core/Math/Math.h"
#include <iostream>

namespace FinalStorm {

struct MetalRendererImpl {
    id<MTLDevice> device;
    id<MTLCommandQueue> commandQueue;
    id<MTLLibrary> defaultLibrary;
    
    // Pipeline states
    id<MTLRenderPipelineState> meshPipeline;
    id<MTLRenderPipelineState> particlePipeline;
    id<MTLRenderPipelineState> uiPipeline;
    
    // Depth stencil state
    id<MTLDepthStencilState> depthStencilState;
    
    // Buffers
    id<MTLBuffer> uniformBuffer;
    
    // Current frame data
    id<MTLCommandBuffer> currentCommandBuffer;
    id<MTLRenderCommandEncoder> currentEncoder;
    MTKView* view;
    
    uint32_t frameIndex;
    static const int MaxFramesInFlight = 3;
};

MetalRenderer::MetalRenderer(void* metalView)
    : impl(std::make_unique<MetalRendererImpl>()) {
    
    impl->view = (__bridge MTKView*)metalView;
    impl->device = impl->view.device;
    impl->frameIndex = 0;
    
    if (!impl->device) {
        impl->device = MTLCreateSystemDefaultDevice();
        impl->view.device = impl->device;
    }
    
    initialize();
}

MetalRenderer::~MetalRenderer() = default;

bool MetalRenderer::initialize() {
    @autoreleasepool {
        // Create command queue
        impl->commandQueue = [impl->device newCommandQueue];
        
        // Load shaders
        NSError* error = nil;
        
        // Try to load compiled metallib first
        NSString* libraryPath = [[NSBundle mainBundle] pathForResource:@"default" ofType:@"metallib"];
        if (libraryPath) {
            impl->defaultLibrary = [impl->device newLibraryWithFile:libraryPath error:&error];
        }
        
        // Fallback to source
        if (!impl->defaultLibrary) {
            NSString* shaderSource = @R"(
                #include <metal_stdlib>
                #include <simd/simd.h>
                using namespace metal;
                
                struct VertexIn {
                    float3 position [[attribute(0)]];
                    float3 normal [[attribute(1)]];
                    float2 texcoord [[attribute(2)]];
                };
                
                struct VertexOut {
                    float4 position [[position]];
                    float3 worldPos;
                    float3 normal;
                    float2 texcoord;
                };
                
                struct Uniforms {
                    float4x4 modelMatrix;
                    float4x4 viewMatrix;
                    float4x4 projectionMatrix;
                    float4x4 normalMatrix;
                    float3 cameraPosition;
                    float time;
                };
                
                vertex VertexOut vertex_main(VertexIn in [[stage_in]],
                                           constant Uniforms& uniforms [[buffer(1)]]) {
                    VertexOut out;
                    float4 worldPos = uniforms.modelMatrix * float4(in.position, 1.0);
                    out.worldPos = worldPos.xyz;
                    out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
                    out.normal = normalize((uniforms.normalMatrix * float4(in.normal, 0.0)).xyz);
                    out.texcoord = in.texcoord;
                    return out;
                }
                
                fragment float4 fragment_main(VertexOut in [[stage_in]],
                                            constant Uniforms& uniforms [[buffer(1)]]) {
                    // Simple lit shader
                    float3 lightDir = normalize(float3(1, 1, 1));
                    float3 viewDir = normalize(uniforms.cameraPosition - in.worldPos);
                    float3 normal = normalize(in.normal);
                    
                    // Diffuse
                    float diff = max(dot(normal, lightDir), 0.0);
                    
                    // Specular
                    float3 reflectDir = reflect(-lightDir, normal);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
                    
                    // Combine
                    float3 ambient = float3(0.1, 0.1, 0.15);
                    float3 diffuse = float3(0.5, 0.8, 1.0) * diff;
                    float3 specular = float3(1.0) * spec;
                    
                    float3 result = ambient + diffuse + specular;
                    return float4(result, 1.0);
                }
            )";
            
            impl->defaultLibrary = [impl->device newLibraryWithSource:shaderSource options:nil error:&error];
        }
        
        if (!impl->defaultLibrary) {
            std::cerr << "Failed to create shader library: " << error.localizedDescription.UTF8String << std::endl;
            return false;
        }
        
        // Create pipeline states
        if (!createPipelines()) {
            return false;
        }
        
        // Create depth stencil state
        MTLDepthStencilDescriptor* depthDescriptor = [[MTLDepthStencilDescriptor alloc] init];
        depthDescriptor.depthCompareFunction = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled = YES;
        impl->depthStencilState = [impl->device newDepthStencilStateWithDescriptor:depthDescriptor];
        
        // Create uniform buffer
        impl->uniformBuffer = [impl->device newBufferWithLength:sizeof(Uniforms) * impl->MaxFramesInFlight
                                                        options:MTLResourceStorageModeShared];
        
        // Configure view
        impl->view.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        impl->view.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
        impl->view.clearColor = MTLClearColorMake(0.05, 0.05, 0.1, 1.0);
        
        return true;
    }
}

bool MetalRenderer::createPipelines() {
    @autoreleasepool {
        NSError* error = nil;
        
        // Get shader functions
        id<MTLFunction> vertexFunction = [impl->defaultLibrary newFunctionWithName:@"vertex_main"];
        id<MTLFunction> fragmentFunction = [impl->defaultLibrary newFunctionWithName:@"fragment_main"];
        
        if (!vertexFunction || !fragmentFunction) {
            std::cerr << "Failed to load shader functions" << std::endl;
            return false;
        }
        
        // Create vertex descriptor
        MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
        
        // Position
        vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
        vertexDescriptor.attributes[0].offset = 0;
        vertexDescriptor.attributes[0].bufferIndex = 0;
        
        // Normal
        vertexDescriptor.attributes[1].format = MTLVertexFormatFloat3;
        vertexDescriptor.attributes[1].offset = sizeof(float) * 3;
        vertexDescriptor.attributes[1].bufferIndex = 0;
        
        // Texcoord
        vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
        vertexDescriptor.attributes[2].offset = sizeof(float) * 6;
        vertexDescriptor.attributes[2].bufferIndex = 0;
        
        // Layout
        vertexDescriptor.layouts[0].stride = sizeof(float) * 8;
        vertexDescriptor.layouts[0].stepRate = 1;
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        
        // Create pipeline state descriptor
        MTLRenderPipelineDescriptor* pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDescriptor.vertexFunction = vertexFunction;
        pipelineDescriptor.fragmentFunction = fragmentFunction;
        pipelineDescriptor.vertexDescriptor = vertexDescriptor;
        pipelineDescriptor.colorAttachments[0].pixelFormat = impl->view.colorPixelFormat;
        pipelineDescriptor.depthAttachmentPixelFormat = impl->view.depthStencilPixelFormat;
        
        // Enable blending
        pipelineDescriptor.colorAttachments[0].blendingEnabled = YES;
        pipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
        pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorZero;
        
        // Create pipeline state
        impl->meshPipeline = [impl->device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
        
        if (!impl->meshPipeline) {
            std::cerr << "Failed to create mesh pipeline state: " << error.localizedDescription.UTF8String << std::endl;
            return false;
        }
        
        // For now, use the same pipeline for particles and UI
        impl->particlePipeline = impl->meshPipeline;
        impl->uiPipeline = impl->meshPipeline;
        
        return true;
    }
}

void MetalRenderer::beginFrame() {
    @autoreleasepool {
        // Get current drawable
        impl->currentCommandBuffer = [impl->commandQueue commandBuffer];
        impl->currentCommandBuffer.label = @"Frame Command Buffer";
        
        MTLRenderPassDescriptor* renderPassDescriptor = impl->view.currentRenderPassDescriptor;
        if (!renderPassDescriptor) {
            return;
        }
        
        impl->currentEncoder = [impl->currentCommandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        impl->currentEncoder.label = @"Main Render Encoder";
        
        // Set default render state
        [impl->currentEncoder setDepthStencilState:impl->depthStencilState];
        [impl->currentEncoder setCullMode:MTLCullModeBack];
        [impl->currentEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    }
}

void MetalRenderer::endFrame() {
    @autoreleasepool {
        if (impl->currentEncoder) {
            [impl->currentEncoder endEncoding];
            impl->currentEncoder = nil;
        }
        
        if (impl->currentCommandBuffer) {
            [impl->currentCommandBuffer presentDrawable:impl->view.currentDrawable];
            [impl->currentCommandBuffer commit];
            impl->currentCommandBuffer = nil;
        }
        
        impl->frameIndex = (impl->frameIndex + 1) % impl->MaxFramesInFlight;
    }
}

std::unique_ptr<RenderContext> MetalRenderer::createRenderContext() {
    return std::make_unique<MetalRenderContext>(this, impl->currentEncoder);
}

void MetalRenderer::resize(uint32_t width, uint32_t height) {
    // Metal view handles resize automatically
}

void MetalRenderer::setClearColor(const float4& color) {
    impl->view.clearColor = MTLClearColorMake(color.x, color.y, color.z, color.w);
}

void MetalRenderer::setViewProjectionMatrix(const float4x4& view, const float4x4& projection) {
    viewMatrix = view;
    projectionMatrix = projection;
}

// Internal methods for MetalRenderContext
id<MTLDevice> MetalRenderer::getDevice() const {
    return impl->device;
}

id<MTLRenderCommandEncoder> MetalRenderer::getCurrentEncoder() const {
    return impl->currentEncoder;
}

id<MTLRenderPipelineState> MetalRenderer::getMeshPipeline() const {
    return impl->meshPipeline;
}

void MetalRenderer::updateUniforms(const float4x4& model, const float4x4& view, const float4x4& proj) {
    Uniforms uniforms;
    uniforms.modelMatrix = model;
    uniforms.viewMatrix = view;
    uniforms.projectionMatrix = proj;
    
    // Calculate normal matrix (inverse transpose of model)
    float4x4 modelView = view * model;
    uniforms.normalMatrix = Math::transpose(Math::inverse(modelView));
    
    // Camera position (inverse view matrix translation)
    float4x4 invView = Math::inverse(view);
    uniforms.cameraPosition = float3(invView[3][0], invView[3][1], invView[3][2]);
    
    uniforms.time = 0.0f; // TODO: Pass actual time
    
    // Update buffer
    uint32_t offset = impl->frameIndex * sizeof(Uniforms);
    memcpy((uint8_t*)impl->uniformBuffer.contents + offset, &uniforms, sizeof(Uniforms));
    
    [impl->currentEncoder setVertexBuffer:impl->uniformBuffer offset:offset atIndex:1];
    [impl->currentEncoder setFragmentBuffer:impl->uniformBuffer offset:offset atIndex:1];
}

} // namespace FinalStorm