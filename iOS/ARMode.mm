#import "ARMode.h"

@implementation ARMode {
    ARSession *_session;
    MetalRenderer *_renderer;
    NSMutableDictionary<NSUUID *, NSValue *> *_serviceAnchors;
}

- (instancetype)initWithRenderer:(MetalRenderer *)renderer {
    if ((self = [super init])) {
        _renderer = renderer;
        _session = [ARSession new];
        _session.delegate = self;
        _serviceAnchors = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)startSession {
    if (@available(iOS 13.0, *)) {
        ARWorldTrackingConfiguration *config = [ARWorldTrackingConfiguration new];
        [_session runWithConfiguration:config];
    }
}

- (void)pauseSession {
    [_session pause];
}

- (void)placeServicesInRealWorld {
    if (!_renderer) return;
    const auto &children = _renderer.sceneRoot->getChildren();
    for (const auto &child : children) {
        auto service = std::dynamic_pointer_cast<FinalStorm::ServiceRepresentation>(child);
        if (service) {
            [self anchorServiceToSurface:service];
        }
    }
}

- (void)anchorServiceToSurface:(std::shared_ptr<FinalStorm::ServiceRepresentation>)service {
    if (!service) return;
    if (@available(iOS 13.0, *)) {
        FinalStorm::float3 pos = service->getWorldPosition();
        matrix_float4x4 transform = matrix_identity_float4x4;
        transform.columns[3].x = pos.x;
        transform.columns[3].y = pos.y;
        transform.columns[3].z = pos.z;
        ARAnchor *anchor = [[ARAnchor alloc] initWithTransform:transform];
        [_session addAnchor:anchor];
        _serviceAnchors[anchor.identifier] = [NSValue valueWithPointer:service.get()];
    }
}

@end
