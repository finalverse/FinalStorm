//
//  MetalRenderer.cpp
//  FinalStorm macOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#import "MetalRenderer.h"
#import "MetalShaderTypes.h"
#include "../Shared/Core/World/Entity.h"
#include "../Shared/SceneGraph/SceneNode.h"
#include <vector>
#include <unordered_set>

using namespace FinalStorm;

@implementation MetalRenderer
{
    id<MTLDevice> _device;
    id<MTLCommandQueue> _commandQueue;
    id<MTLRenderPipelineState> _pipelineState;
    id<MTLDepthStencilState> _depthState;

    id<MTLTexture> _shadowMap;
    id<MTLRenderPipelineState> _shadowPipelineState;
    id<MTLTexture> _postProcessTexture;
    id<MTLRenderPipelineState> _postProcessPipelineState;
    id<MTLTexture> _environmentCubemap;
    
    // Buffers
    id<MTLBuffer> _uniformBuffer;
    id<MTLBuffer> _cubeVertexBuffer;
    id<MTLBuffer> _cubeIndexBuffer;
    id<MTLBuffer> _planeVertexBuffer;
    id<MTLBuffer> _planeIndexBuffer;
    
    // Mesh data
    NSUInteger _cubeIndexCount;
    NSUInteger _planeIndexCount;
    
    // C++ objects
    std::shared_ptr<WorldManager> _worldManager;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<SceneNode> _sceneRoot;
    
    // Input state
    FSPoint _lastMousePosition;
    bool _mouseDown;
    std::unordered_set<uint16_t> _keysPressed;
}

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        _device = mtkView.device;
        _worldManager = std::make_shared<WorldManager>();
        _camera = std::make_shared<Camera>();
        _sceneRoot = std::make_shared<SceneNode>();
        
        mtkView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        mtkView.sampleCount = 1;
        
        [self _loadMetalWithView:mtkView];
        [self _loadAssets];
        
        // Set initial camera position
        _camera->setPosition(float3{0.0f, 5.0f, 10.0f});
        _camera->setTarget(float3{0.0f, 0.0f, 0.0f});
    }
    
    return self;
}

- (void)_loadMetalWithView:(nonnull MTKView *)view
{
    // Create command queue
    _commandQueue = [_device newCommandQueue];
    
    // Load shaders
    id<MTLLibrary> defaultLibrary = nil;
    NSError *error = nil;
    
    // First try to load the compiled metallib
    NSString *libPath = [[NSBundle mainBundle] pathForResource:@"default" ofType:@"metallib"];
    if (libPath) {
        defaultLibrary = [_device newLibraryWithFile:libPath error:&error];
        if (!defaultLibrary) {
            NSLog(@"Failed to load Metal library at %@, error %@", libPath, error);
        }
    } else {
        NSLog(@"default.metallib not found in bundle, attempting newDefaultLibrary");
        defaultLibrary = [_device newDefaultLibrary];
        if (!defaultLibrary) {
            NSLog(@"Failed to load default Metal library");
        }
    }
    
    // If metallib loading failed, try to compile from source
    if (!defaultLibrary) {
        NSLog(@"Attempting to compile shaders from source...");
        NSString *shaderPath = [[NSBundle mainBundle] pathForResource:@"Shaders" ofType:@"metal"];
        if (!shaderPath) {
            // Try alternative paths
            NSArray *possiblePaths = @[
                @"MetalRenderer/Shaders.metal",
                @"Shaders.metal",
                @"../MetalRenderer/Shaders.metal"
            ];
            
            for (NSString *path in possiblePaths) {
                NSString *fullPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:path];
                if ([[NSFileManager defaultManager] fileExistsAtPath:fullPath]) {
                    shaderPath = fullPath;
                    break;
                }
            }
        }
        
        if (shaderPath) {
            NSError *libraryError = nil;
            NSString *shaderSource = [NSString stringWithContentsOfFile:shaderPath encoding:NSUTF8StringEncoding error:&libraryError];
            if (shaderSource) {
                defaultLibrary = [_device newLibraryWithSource:shaderSource options:nil error:&libraryError];
                if (!defaultLibrary) {
                    NSLog(@"Failed to compile shader from source: %@", libraryError);
                }
            } else {
                NSLog(@"Failed to read shader source: %@", libraryError);
            }
        }
    }
    
    // As a last resort, use the default library
    if (!defaultLibrary) {
        NSLog(@"Using device default library as fallback");
        defaultLibrary = [_device newDefaultLibrary];
    }

    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
    
    if (!vertexFunction || !fragmentFunction) {
        NSLog(@"Failed to find shader functions in Metal library");
        // Don't continue if we can't find the functions
        return;
    }
    
    // Rest of the pipeline setup...
    // Configure pipeline
    MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"MyPipeline";
    pipelineStateDescriptor.rasterSampleCount = view.sampleCount;
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;
    pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
    pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;
    
    // Vertex descriptor
    MTLVertexDescriptor *vertexDescriptor = [[MTLVertexDescriptor alloc] init];
    
    // Position
    vertexDescriptor.attributes[VertexAttributePosition].format = MTLVertexFormatFloat3;
    vertexDescriptor.attributes[VertexAttributePosition].offset = 0;
    vertexDescriptor.attributes[VertexAttributePosition].bufferIndex = BufferIndexMeshPositions;
    
    // Normal
    vertexDescriptor.attributes[VertexAttributeNormal].format = MTLVertexFormatFloat3;
    vertexDescriptor.attributes[VertexAttributeNormal].offset = sizeof(float3);
    vertexDescriptor.attributes[VertexAttributeNormal].bufferIndex = BufferIndexMeshPositions;
    
    // Texture coordinates
    vertexDescriptor.attributes[VertexAttributeTexcoord].format = MTLVertexFormatFloat2;
    vertexDescriptor.attributes[VertexAttributeTexcoord].offset = sizeof(float3) * 2;
    vertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex = BufferIndexMeshPositions;
    
    vertexDescriptor.layouts[BufferIndexMeshPositions].stride = sizeof(Vertex);
    vertexDescriptor.layouts[BufferIndexMeshPositions].stepRate = 1;
    vertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction = MTLVertexStepFunctionPerVertex;
    
    pipelineStateDescriptor.vertexDescriptor = vertexDescriptor;
    
    NSError *pipelineError = NULL;
    _pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&pipelineError];
    if (!_pipelineState)
    {
        NSLog(@"Failed to created pipeline state, error %@", pipelineError);
    }
    
    // Create depth state
    MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStateDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthStateDesc.depthWriteEnabled = YES;
    _depthState = [_device newDepthStencilStateWithDescriptor:depthStateDesc];

    // Create uniform buffer
    _uniformBuffer = [_device newBufferWithLength:sizeof(Uniforms)
                                          options:MTLResourceStorageModeShared];

    // Shadow map texture
    MTLTextureDescriptor *shadowDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                                                              width:1024
                                                                                             height:1024
                                                                                          mipmapped:NO];
    shadowDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    shadowDesc.storageMode = MTLStorageModePrivate;
    _shadowMap = [_device newTextureWithDescriptor:shadowDesc];

    // Post-process texture
    MTLTextureDescriptor *ppDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:view.colorPixelFormat
                                                                                           width:view.drawableSize.width
                                                                                          height:view.drawableSize.height
                                                                                       mipmapped:NO];
    ppDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    ppDesc.storageMode = MTLStorageModePrivate;
    _postProcessTexture = [_device newTextureWithDescriptor:ppDesc];

    // Environment cube map placeholder
    MTLTextureDescriptor *envDesc = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:view.colorPixelFormat size:512 mipmapped:NO];
    envDesc.usage = MTLTextureUsageShaderRead;
    envDesc.storageMode = MTLStorageModePrivate;
    _environmentCubemap = [_device newTextureWithDescriptor:envDesc];

    _shadowPipelineState = _pipelineState;
    _postProcessPipelineState = _pipelineState;
}

- (void)_loadAssets
{
    // Create cube mesh
    static const Vertex cubeVertices[] =
    {
        // Front face
        {{-0.5, -0.5,  0.5}, { 0.0,  0.0,  1.0}, {0.0, 1.0}},
        {{ 0.5, -0.5,  0.5}, { 0.0,  0.0,  1.0}, {1.0, 1.0}},
        {{ 0.5,  0.5,  0.5}, { 0.0,  0.0,  1.0}, {1.0, 0.0}},
        {{-0.5,  0.5,  0.5}, { 0.0,  0.0,  1.0}, {0.0, 0.0}},
        
        // Back face
        {{ 0.5, -0.5, -0.5}, { 0.0,  0.0, -1.0}, {0.0, 1.0}},
        {{-0.5, -0.5, -0.5}, { 0.0,  0.0, -1.0}, {1.0, 1.0}},
        {{-0.5,  0.5, -0.5}, { 0.0,  0.0, -1.0}, {1.0, 0.0}},
        {{ 0.5,  0.5, -0.5}, { 0.0,  0.0, -1.0}, {0.0, 0.0}},
        
        // Top face
        {{-0.5,  0.5,  0.5}, { 0.0,  1.0,  0.0}, {0.0, 1.0}},
        {{ 0.5,  0.5,  0.5}, { 0.0,  1.0,  0.0}, {1.0, 1.0}},
        {{ 0.5,  0.5, -0.5}, { 0.0,  1.0,  0.0}, {1.0, 0.0}},
        {{-0.5,  0.5, -0.5}, { 0.0,  1.0,  0.0}, {0.0, 0.0}},
        
        // Bottom face
        {{-0.5, -0.5, -0.5}, { 0.0, -1.0,  0.0}, {0.0, 1.0}},
        {{ 0.5, -0.5, -0.5}, { 0.0, -1.0,  0.0}, {1.0, 1.0}},
        {{ 0.5, -0.5,  0.5}, { 0.0, -1.0,  0.0}, {1.0, 0.0}},
        {{-0.5, -0.5,  0.5}, { 0.0, -1.0,  0.0}, {0.0, 0.0}},
        
        // Right face
        {{ 0.5, -0.5,  0.5}, { 1.0,  0.0,  0.0}, {0.0, 1.0}},
        {{ 0.5, -0.5, -0.5}, { 1.0,  0.0,  0.0}, {1.0, 1.0}},
        {{ 0.5,  0.5, -0.5}, { 1.0,  0.0,  0.0}, {1.0, 0.0}},
        {{ 0.5,  0.5,  0.5}, { 1.0,  0.0,  0.0}, {0.0, 0.0}},
        
        // Left face
        {{-0.5, -0.5, -0.5}, {-1.0,  0.0,  0.0}, {0.0, 1.0}},
        {{-0.5, -0.5,  0.5}, {-1.0,  0.0,  0.0}, {1.0, 1.0}},
        {{-0.5,  0.5,  0.5}, {-1.0,  0.0,  0.0}, {1.0, 0.0}},
        {{-0.5,  0.5, -0.5}, {-1.0,  0.0,  0.0}, {0.0, 0.0}}
    };
    
    static const uint16_t cubeIndices[] =
    {
        0,  1,  2,  2,  3,  0,   // front
        4,  5,  6,  6,  7,  4,   // back
        8,  9, 10, 10, 11,  8,   // top
        12, 13, 14, 14, 15, 12,  // bottom
        16, 17, 18, 18, 19, 16,  // right
        20, 21, 22, 22, 23, 20   // left
    };
    
    _cubeVertexBuffer = [_device newBufferWithBytes:cubeVertices
                                             length:sizeof(cubeVertices)
                                            options:MTLResourceStorageModeShared];
    
    _cubeIndexBuffer = [_device newBufferWithBytes:cubeIndices
                                            length:sizeof(cubeIndices)
                                           options:MTLResourceStorageModeShared];
    
    _cubeIndexCount = sizeof(cubeIndices) / sizeof(uint16_t);
    
    // Create plane mesh
    const float planeSize = 50.0f;
    const Vertex planeVertices[] =
    {
        {{-planeSize, 0, -planeSize}, {0, 1, 0}, {0, 0}},
        {{ planeSize, 0, -planeSize}, {0, 1, 0}, {10, 0}},
        {{ planeSize, 0,  planeSize}, {0, 1, 0}, {10, 10}},
        {{-planeSize, 0,  planeSize}, {0, 1, 0}, {0, 10}}
    };
    
    const uint16_t planeIndices[] = { 0, 1, 2, 2, 3, 0 };
    
    _planeVertexBuffer = [_device newBufferWithBytes:planeVertices
                                             length:sizeof(planeVertices)
                                            options:MTLResourceStorageModeShared];
    
    _planeIndexBuffer = [_device newBufferWithBytes:planeIndices
                                            length:sizeof(planeIndices)
                                           options:MTLResourceStorageModeShared];
    
    _planeIndexCount = sizeof(planeIndices) / sizeof(uint16_t);
    
    // Add some test entities
    auto testEntity = std::make_shared<Entity>(1, EntityType::Object);
    testEntity->setMeshName("cube");
    testEntity->getTransform().position = float3{0.0f, 1.0f, 0.0f};
    _worldManager->addEntity(testEntity);
    
    auto testEntity2 = std::make_shared<Entity>(2, EntityType::Object);
    testEntity2->setMeshName("cube");
    testEntity2->getTransform().position = float3{3.0f, 1.0f, -2.0f};
    _worldManager->addEntity(testEntity2);
}

- (void)updateWithDeltaTime:(float)deltaTime
{
    // Handle keyboard input
    float moveSpeed = 5.0f * deltaTime;
    float3 cameraPos = _camera->getPosition();
    float3 cameraTarget = _camera->getTarget();
    float3 forward = simd_normalize(cameraTarget - cameraPos);
    float3 right = simd_normalize(simd_cross(forward, float3{0, 1, 0}));
    
    if (_keysPressed.count(0x0D)) // W
        cameraPos += forward * moveSpeed;
    if (_keysPressed.count(0x01)) // S
        cameraPos -= forward * moveSpeed;
    if (_keysPressed.count(0x00)) // A
        cameraPos -= right * moveSpeed;
    if (_keysPressed.count(0x02)) // D
        cameraPos += right * moveSpeed;
    
    _camera->setPosition(cameraPos);
    _camera->setTarget(cameraPos + forward);

    // Update world
    _worldManager->update(deltaTime);

    // Rebuild scene graph with visible entities
    _sceneRoot->clearChildren();
    auto visibleEntities = _worldManager->getVisibleEntities(*_camera);
    for (const auto& entity : visibleEntities) {
        auto node = std::make_shared<SceneNode>();
        node->setEntity(entity);
        node->update(deltaTime);
        _sceneRoot->addChild(node);
    }

    _sceneRoot->update(deltaTime);
}

// In the drawInMTKView method, fix the uniforms update:
- (void)drawInMTKView:(nonnull MTKView *)view
{
    @autoreleasepool {
        id<MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
        commandBuffer.label = @"MyCommand";

        MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
        [self renderShadowPassWithCommandBuffer:commandBuffer];
        if(renderPassDescriptor != nil)
        {
            [self renderMainPassWithCommandBuffer:commandBuffer renderPassDescriptor:renderPassDescriptor];
            [self renderPostProcessWithCommandBuffer:commandBuffer renderPassDescriptor:renderPassDescriptor];
            [commandBuffer presentDrawable:view.currentDrawable];
        }

        [commandBuffer commit];
    }
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
   float aspect = size.width / (float)size.height;
   _camera->setPerspective(60.0f * M_PI / 180.0f, aspect, 0.1f, 1000.0f);
}

- (void)renderShadowPassWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer
{
    (void)commandBuffer;
    // Placeholder for shadow map rendering
}

- (void)renderMainPassWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer
                 renderPassDescriptor:(MTLRenderPassDescriptor*)renderPassDescriptor
{
    id<MTLRenderCommandEncoder> renderEncoder =
        [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    renderEncoder.label = @"MainPass";

    [renderEncoder setCullMode:MTLCullModeBack];
    [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [renderEncoder setRenderPipelineState:_pipelineState];
    [renderEncoder setDepthStencilState:_depthState];

    Uniforms *uniforms = (Uniforms*)_uniformBuffer.contents;
    uniforms->viewProjectionMatrix = _camera->getViewProjectionMatrix();

    // Render ground plane
    uniforms->modelMatrix = matrix_identity();
    uniforms->normalMatrix = matrix3x3_upper_left(uniforms->modelMatrix);
    uniforms->color = simd_make_float4(0.3f, 0.3f, 0.3f, 1.0f);

    [renderEncoder setVertexBuffer:_planeVertexBuffer offset:0 atIndex:BufferIndexMeshPositions];
    [renderEncoder setVertexBuffer:_uniformBuffer offset:0 atIndex:BufferIndexUniforms];
    [renderEncoder setFragmentBuffer:_uniformBuffer offset:0 atIndex:BufferIndexUniforms];

    [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                indexCount:_planeIndexCount
                                 indexType:MTLIndexTypeUInt16
                               indexBuffer:_planeIndexBuffer
                         indexBufferOffset:0];

    // Render scene graph
    for (const auto& child : _sceneRoot->getChildren())
    {
        if (!child) continue;
        auto entity = child->getEntity();
        if (!entity || entity->getMeshName() != "cube") continue;

        uniforms->modelMatrix = child->getWorldMatrix();
        uniforms->normalMatrix = matrix3x3_upper_left(uniforms->modelMatrix);
        uniforms->color = simd_make_float4(0.8f, 0.2f, 0.2f, 1.0f);

        [renderEncoder setVertexBuffer:_cubeVertexBuffer offset:0 atIndex:BufferIndexMeshPositions];

        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                    indexCount:_cubeIndexCount
                                     indexType:MTLIndexTypeUInt16
                                   indexBuffer:_cubeIndexBuffer
                             indexBufferOffset:0];
    }

    [renderEncoder endEncoding];
}

- (void)renderPostProcessWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer
                     renderPassDescriptor:(MTLRenderPassDescriptor*)renderPassDescriptor
{
    (void)commandBuffer;
    (void)renderPassDescriptor;
    // Placeholder for post-processing
}

#pragma mark - Input Handling

- (void)handleMouseDown:(FSPoint)point
{
   _mouseDown = true;
   _lastMousePosition = point;
}

- (void)handleMouseDragged:(FSPoint)point
{
   if (_mouseDown)
   {
       float deltaX = point.x - _lastMousePosition.x;
       float deltaY = point.y - _lastMousePosition.y;
       
       // Rotate camera
       float sensitivity = 0.005f;
       float3 cameraPos = _camera->getPosition();
       float3 cameraTarget = _camera->getTarget();
       float3 toTarget = cameraTarget - cameraPos;
       
       // Horizontal rotation
       float angle = -deltaX * sensitivity;
       float cosAngle = cosf(angle);
       float sinAngle = sinf(angle);
       
       float3 newToTarget;
       newToTarget.x = toTarget.x * cosAngle - toTarget.z * sinAngle;
       newToTarget.y = toTarget.y;
       newToTarget.z = toTarget.x * sinAngle + toTarget.z * cosAngle;
       
       _camera->setTarget(cameraPos + newToTarget);
       
       _lastMousePosition = point;
   }
}

- (void)handleMouseUp:(FSPoint)point
{
   _mouseDown = false;
}

- (void)handleKeyDown:(uint16_t)keyCode
{
   _keysPressed.insert(keyCode);
}

- (void)handleKeyUp:(uint16_t)keyCode
{
   _keysPressed.erase(keyCode);
}

@synthesize worldManager = _worldManager;
@synthesize camera = _camera;
@synthesize sceneRoot = _sceneRoot;

@end
