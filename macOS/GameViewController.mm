//
//  GameViewController.m
//  FinalStorm macOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#import "GameViewController.h"
#include "../Shared/Core/Networking/FinalverseClient.h"
#include "../Shared/Core/Input/InteractionManager.h"

@implementation GameViewController
{
    MTKView *_view;
    MetalRenderer *_renderer;
    std::unique_ptr<FinalStorm::FinalverseClient> _client;
    std::shared_ptr<FinalStorm::InteractionManager> _interactionManager;

    CVDisplayLinkRef _displayLink;
    dispatch_source_t _displaySource;

    NSTimeInterval _lastTime;
}

- (void)loadView
{
    self.view = [[MTKView alloc] initWithFrame:NSMakeRect(0, 0, 800, 600) device:nil];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    _view.clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0); //black

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }

    _renderer = [[MetalRenderer alloc] initWithMetalKitView:_view];
    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

    _interactionManager = std::make_shared<FinalStorm::InteractionManager>();
    _interactionManager->setCamera(_renderer.camera);
    _interactionManager->setSceneRoot(_renderer.sceneRoot);
    
    _view.delegate = _renderer;
    
    // Setup client
    _client = std::make_unique<FinalStorm::FinalverseClient>();
    _client->setWorldManager(_renderer.worldManager.get());
    
    // Connect to server
    _client->connect("localhost", 50051, [](bool success) {
        if (success) {
            NSLog(@"Connected to Finalverse server!");
        } else {
            NSLog(@"Failed to connect to server");
        }
    });
    
    // Setup game loop
    [self setupGameLoop];
    
    // Make view accept key events
    [_view.window makeFirstResponder:_view];
}

- (void)setupGameLoop
{
    _lastTime = CACurrentMediaTime();
    
    // Create a display link for smooth animation
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
    CVDisplayLinkSetOutputCallback(_displayLink, displayLinkCallback, (__bridge void *)self);
    CVDisplayLinkStart(_displayLink);
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                   const CVTimeStamp* now,
                                   const CVTimeStamp* outputTime,
                                   CVOptionFlags flagsIn,
                                   CVOptionFlags* flagsOut,
                                   void* displayLinkContext)
{
    @autoreleasepool {
        GameViewController *viewController = (__bridge GameViewController*)displayLinkContext;
        [viewController gameLoop];
    }
    return kCVReturnSuccess;
}

- (void)gameLoop
{
    NSTimeInterval currentTime = CACurrentMediaTime();
    float deltaTime = currentTime - _lastTime;
    _lastTime = currentTime;
    
    // Update game logic
    dispatch_async(dispatch_get_main_queue(), ^{
        [self->_renderer updateWithDeltaTime:deltaTime];
        
        // Update network client
        if (self->_client) {
            self->_client->update();
            
            // Send player position
            auto camera = self->_renderer.camera;
            if (camera) {
                // For now, send identity quaternion for rotation
                FinalStorm::float4 rotation = simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f);
                self->_client->sendPlayerPosition(camera->getPosition(), rotation);
            }
        }
    });
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint locationInView = [_view convertPoint:[event locationInWindow] fromView:nil];
    [_renderer handleMouseDown:locationInView];
    if (_interactionManager) {
        FinalStorm::float2 viewport = simd_make_float2(_view.bounds.size.width, _view.bounds.size.height);
        FinalStorm::float2 pos = simd_make_float2(locationInView.x, locationInView.y);
        _interactionManager->handleTouchBegan(pos, viewport);
    }
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint locationInView = [_view convertPoint:[event locationInWindow] fromView:nil];
    [_renderer handleMouseDragged:locationInView];
    if (_interactionManager) {
        FinalStorm::float2 viewport = simd_make_float2(_view.bounds.size.width, _view.bounds.size.height);
        FinalStorm::float2 pos = simd_make_float2(locationInView.x, locationInView.y);
        _interactionManager->handleTouchMoved(pos, viewport);
    }
}

- (void)mouseUp:(NSEvent *)event
{
    NSPoint locationInView = [_view convertPoint:[event locationInWindow] fromView:nil];
    [_renderer handleMouseUp:locationInView];
    if (_interactionManager) {
        FinalStorm::float2 viewport = simd_make_float2(_view.bounds.size.width, _view.bounds.size.height);
        FinalStorm::float2 pos = simd_make_float2(locationInView.x, locationInView.y);
        _interactionManager->handleTouchEnded(pos, viewport);
    }
}

- (void)keyDown:(NSEvent *)event
{
    [_renderer handleKeyDown:event.keyCode];
}

- (void)keyUp:(NSEvent *)event
{
    [_renderer handleKeyUp:event.keyCode];
}

- (void)dealloc
{
    if (_displayLink) {
        CVDisplayLinkStop(_displayLink);
        CVDisplayLinkRelease(_displayLink);
    }
}

@end
