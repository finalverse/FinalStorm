#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <simd/simd.h>

#include "../../Shared/Core/Math/Math.h"
#include "../../Shared/Core/World/WorldManager.h"
#include <memory>

@interface MetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;
- (void)updateWithDeltaTime:(float)deltaTime;
- (void)handleMouseDown:(NSPoint)point;
- (void)handleMouseDragged:(NSPoint)point;
- (void)handleMouseUp:(NSPoint)point;
- (void)handleKeyDown:(uint16_t)keyCode;
- (void)handleKeyUp:(uint16_t)keyCode;

@property (nonatomic, readonly) std::shared_ptr<FinalStorm::WorldManager> worldManager;
@property (nonatomic, readonly) std::shared_ptr<FinalStorm::Camera> camera;

@end