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

#include "../Shared/Core/Math/Math.h"
#include "../Shared/Core/World/WorldManager.h"
#include <memory>

@interface MetalRenderer : NSObject<MTKViewDelegate>

- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView;
- (void)updateWithDeltaTime:(float)deltaTime;
- (void)handleMouseDown:(FSPoint)point;
- (void)handleMouseDragged:(FSPoint)point;
- (void)handleMouseUp:(FSPoint)point;
- (void)handleKeyDown:(uint16_t)keyCode;
- (void)handleKeyUp:(uint16_t)keyCode;

@property (nonatomic, readonly) std::shared_ptr<FinalStorm::WorldManager> worldManager;
@property (nonatomic, readonly) std::shared_ptr<FinalStorm::Camera> camera;

@end
