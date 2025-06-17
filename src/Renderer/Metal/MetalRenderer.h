#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <simd/simd.h>
#import <TargetConditionals.h>
#if TARGET_OS_OSX
#import <AppKit/AppKit.h>
typedef NSPoint FSPoint;
#else
#import <UIKit/UIKit.h>
typedef CGPoint FSPoint;
#endif

#include "../../Core/Math/Math.h"
#include "../../Core/World/WorldManager.h"
#include "../../Scene/SceneNode.h"
#include <memory>

@interface MetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;
- (void)updateWithDeltaTime:(float)deltaTime;
- (void)handleMouseDown:(FSPoint)point;
- (void)handleMouseDragged:(FSPoint)point;
- (void)handleMouseUp:(FSPoint)point;
- (void)handleKeyDown:(uint16_t)keyCode;
- (void)handleKeyUp:(uint16_t)keyCode;

// Rendering passes
- (void)renderShadowPassWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer;
- (void)renderMainPassWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer
                 renderPassDescriptor:(MTLRenderPassDescriptor*)renderPassDescriptor;
- (void)renderPostProcessWithCommandBuffer:(id<MTLCommandBuffer>)commandBuffer
                     renderPassDescriptor:(MTLRenderPassDescriptor*)renderPassDescriptor;

@property (nonatomic, readonly) std::shared_ptr<FinalStorm::WorldManager> worldManager;
@property (nonatomic, readonly) std::shared_ptr<FinalStorm::Camera> camera;
@property (nonatomic, readonly) std::shared_ptr<FinalStorm::SceneNode> sceneRoot;

@end
