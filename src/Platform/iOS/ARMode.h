#import <Foundation/Foundation.h>
#import <ARKit/ARKit.h>
#import "Rendering/Metal/MetalRenderer.h"
#include "Services/ServiceRepresentation.h"
#include <memory>

@class MetalRenderer;

// Simple helper that manages an ARSession and anchors service nodes
@interface ARMode : NSObject <ARSessionDelegate>

- (instancetype)initWithRenderer:(MetalRenderer *)renderer;

- (void)startSession;
- (void)pauseSession;

/// Iterate over the renderer's service nodes and place them as AR anchors
- (void)placeServicesInRealWorld;

/// Create an ARAnchor for the given service at its current world position
- (void)anchorServiceToSurface:(std::shared_ptr<FinalStorm::ServiceRepresentation>)service;

@end
