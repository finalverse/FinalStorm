//
//  GameViewController.m
//  FinalStorm-iOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#import "GameViewController.h"
#include "../Shared/Core/Networking/FinalverseClient.h"

@implementation GameViewController
{
    MTKView *_view;
    MetalRenderer *_renderer;
    std::unique_ptr<FinalStorm::FinalverseClient> _client;
    CADisplayLink *_displayLink;
    NSTimeInterval _lastTime;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _view = (MTKView *)self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    _view.backgroundColor = UIColor.blackColor;

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        return;
    }

    _renderer = [[MetalRenderer alloc] initWithMetalKitView:_view];
    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];
    
    _view.delegate = _renderer;
    
    // Setup client
    _client = std::make_unique<FinalStorm::FinalverseClient>();
    _client->setWorldManager(_renderer.worldManager.get());
    
    // Connect to server
    _client->connect("10.0.0.100", 50051, [](bool success) {
        if (success) {
            NSLog(@"Connected to Finalverse server!");
        } else {
            NSLog(@"Failed to connect to server");
        }
    });
    
    // Setup game loop
    [self setupGameLoop];
    
    // Setup touch gestures
    [self setupGestures];
}

- (void)setupGameLoop
{
    _lastTime = CACurrentMediaTime();
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop)];
    [_displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)gameLoop
{
    NSTimeInterval currentTime = CACurrentMediaTime();
    float deltaTime = currentTime - _lastTime;
    _lastTime = currentTime;
    
    [_renderer updateWithDeltaTime:deltaTime];
    
    if (_client) {
        _client->update();
        
        auto camera = _renderer.camera;
        if (camera) {
            _client->sendPlayerPosition(camera->getPosition());
        }
    }
}

- (void)setupGestures
{
    // Pan gesture for camera rotation
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    [self.view addGestureRecognizer:panGesture];
    
    // Pinch gesture for zoom
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
    [self.view addGestureRecognizer:pinchGesture];
}

- (void)handlePan:(UIPanGestureRecognizer *)gesture
{
    CGPoint translation = [gesture translationInView:self.view];
    // Convert to Metal coordinates and pass to renderer
    [_renderer handleMouseDragged:NSMakePoint(translation.x, translation.y)];
    [gesture setTranslation:CGPointZero inView:self.view];
}

- (void)handlePinch:(UIPinchGestureRecognizer *)gesture
{
    // Handle zoom
    if (gesture.state == UIGestureRecognizerStateChanged) {
        float scale = gesture.scale;
        // Implement zoom logic
        gesture.scale = 1.0;
    }
}

@end
