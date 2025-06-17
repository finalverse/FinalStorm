//
//  GameViewController.m
//  FinalStorm-iOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#import "GameViewController.h"
#include "../Shared/Core/Networking/FinalverseClient.h"
#include "../Shared/Core/Input/InteractionManager.h"
#import "ARMode.h"

@implementation GameViewController
{
    MTKView *_view;
    MetalRenderer *_renderer;
    std::unique_ptr<FinalStorm::FinalverseClient> _client;
    std::shared_ptr<FinalStorm::InteractionManager> _interactionManager;
    ARMode *_arMode;
    UIButton *_arToggleButton;
    BOOL _arEnabled;
    CADisplayLink *_displayLink;
    NSTimeInterval _lastTime;
}

- (void)loadView
{
    self.view = [[MTKView alloc] initWithFrame:[UIScreen mainScreen].bounds device:nil];
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

    _interactionManager = std::make_shared<FinalStorm::InteractionManager>();
    _interactionManager->setCamera(_renderer.camera);
    _interactionManager->setSceneRoot(_renderer.sceneRoot);

    _view.delegate = _renderer;

    // AR mode setup when supported
    _arEnabled = NO;
    if (@available(iOS 13.0, *)) {
        if ([ARWorldTrackingConfiguration isSupported]) {
            _arMode = [[ARMode alloc] initWithRenderer:_renderer];
            _arToggleButton = [UIButton buttonWithType:UIButtonTypeSystem];
            _arToggleButton.frame = CGRectMake(20, 40, 80, 30);
            [_arToggleButton setTitle:@"AR:Off" forState:UIControlStateNormal];
            [_arToggleButton addTarget:self action:@selector(toggleAR) forControlEvents:UIControlEventTouchUpInside];
            [self.view addSubview:_arToggleButton];
        }
    }
    
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
            // For now, send identity quaternion for rotation
            FinalStorm::float4 rotation = simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f);
            _client->sendPlayerPosition(camera->getPosition(), rotation);
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
    CGPoint location = [gesture locationInView:self.view];
    FSPoint point = CGPointMake(location.x, location.y);
    FinalStorm::float2 viewport = simd_make_float2(_view.bounds.size.width, _view.bounds.size.height);
    FinalStorm::float2 pos = simd_make_float2(point.x, point.y);

    switch (gesture.state) {
        case UIGestureRecognizerStateBegan:
            [_renderer handleMouseDown:point];
            if (_interactionManager) {
                _interactionManager->handleTouchBegan(pos, viewport);
            }
            break;
        case UIGestureRecognizerStateChanged:
            [_renderer handleMouseDragged:point];
            if (_interactionManager) {
                _interactionManager->handleTouchMoved(pos, viewport);
            }
            break;
        case UIGestureRecognizerStateEnded:
        case UIGestureRecognizerStateCancelled:
            [_renderer handleMouseUp:point];
            if (_interactionManager) {
                _interactionManager->handleTouchEnded(pos, viewport);
            }
            break;
        default:
            break;
    }

    [gesture setTranslation:CGPointZero inView:self.view];
}

- (void)handlePinch:(UIPinchGestureRecognizer *)gesture
{
    // Handle zoom
    if (_interactionManager) {
        _interactionManager->handlePinch(gesture.scale);
    }
    if (gesture.state == UIGestureRecognizerStateChanged) {
        float scale = gesture.scale;
        (void)scale;
        gesture.scale = 1.0;
    }
}

- (void)toggleAR
{
    if (!_arMode) return;
    _arEnabled = !_arEnabled;
    if (_arEnabled) {
        [_arMode startSession];
        [_arToggleButton setTitle:@"AR:On" forState:UIControlStateNormal];
        [_arMode placeServicesInRealWorld];
    } else {
        [_arMode pauseSession];
        [_arToggleButton setTitle:@"AR:Off" forState:UIControlStateNormal];
    }
}

@end
