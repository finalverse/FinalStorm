// src/Platform/iOS/GameViewController.mm
// iOS game view controller implementation
// Handles rendering and input on iOS

#import "GameViewController.h"
#include "FinalStormApp.h"
#include "Rendering/Metal/MetalRenderer.h"
#include "Core/Input/InputTypes.h"
#include <memory>

@interface GameViewController ()
@property (nonatomic, strong) MTKView* metalView;
@property (nonatomic) std::unique_ptr<FinalStorm::FinalStormApp> app;
@property (nonatomic) std::shared_ptr<FinalStorm::MetalRenderer> renderer;
@property (nonatomic) CFTimeInterval startTime;
@end

@implementation GameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Create Metal view
    self.metalView = [[MTKView alloc] initWithFrame:self.view.bounds];
    self.metalView.device = MTLCreateSystemDefaultDevice();
    self.metalView.delegate = self;
    self.metalView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    
    [self.view addSubview:self.metalView];
    
    // Create renderer
    self.renderer = std::make_shared<FinalStorm::MetalRenderer>((__bridge void*)self.metalView);
    
    // Create app
    self.app = std::make_unique<FinalStorm::FinalStormApp>();
    if (!self.app->initialize(self.renderer)) {
        NSLog(@"Failed to initialize FinalStorm app!");
    }
    
    // Connect to local server
    self.app->connectToServer("ws://localhost:3000/ws");
    
    self.startTime = CACurrentMediaTime();
    
    // Setup gesture recognizers
    [self setupGestureRecognizers];
}

- (void)setupGestureRecognizers {
    // Tap gesture
    UITapGestureRecognizer* tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
    [self.view addGestureRecognizer:tapGesture];
    
    // Pan gesture
    UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    [self.view addGestureRecognizer:panGesture];
    
    // Pinch gesture
    UIPinchGestureRecognizer* pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
    [self.view addGestureRecognizer:pinchGesture];
}

#pragma mark - MTKViewDelegate

- (void)drawInMTKView:(MTKView *)view {
    @autoreleasepool {
        CFTimeInterval currentTime = CACurrentMediaTime() - self.startTime;
        
        // Update
        self.app->update(currentTime);
        
        // Render
        self.app->render();
    }
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    self.app->resize(size.width, size.height);
}

#pragma mark - Gesture Handlers

- (void)handleTap:(UITapGestureRecognizer*)gesture {
    CGPoint location = [gesture locationInView:self.view];
    
    FinalStorm::InputEvent event;
    event.type = FinalStorm::InputEventType::TouchBegin;
    event.position = FinalStorm::float2(location.x, location.y);
    
    self.app->handleInput(event);
}

- (void)handlePan:(UIPanGestureRecognizer*)gesture {
    CGPoint location = [gesture locationInView:self.view];
    CGPoint velocity = [gesture velocityInView:self.view];
    
    FinalStorm::InputEvent event;
    event.type = FinalStorm::InputEventType::TouchMove;
    event.position = FinalStorm::float2(location.x, location.y);
    event.delta = FinalStorm::float2(velocity.x, -velocity.y) * 0.001f;
    
    self.app->handleInput(event);
}

- (void)handlePinch:(UIPinchGestureRecognizer*)gesture {
    FinalStorm::InputEvent event;
    event.type = FinalStorm::InputEventType::Gesture;
    event.gesture = FinalStorm::GestureType::Pinch;
    event.gestureValue = gesture.scale;
    
    self.app->handleInput(event);
}

@end