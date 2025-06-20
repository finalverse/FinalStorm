// src/Platform/macOS/main.mm
// macOS platform entry point
// Creates the application window and initializes the Metal renderer

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>
#include "FinalStormApp.h"
#include "Rendering/Metal/MetalRenderer.h"
#include "Core/Input/InputTypes.h"

@interface GameViewController : NSViewController<MTKViewDelegate>
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
    self.metalView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    
    [self.view addSubview:self.metalView];
    
    // Create renderer
    self.renderer = std::make_shared<FinalStorm::MetalRenderer>((__bridge void*)self.metalView);
    
    // Create app
    self.app = std::make_unique<FinalStorm::FinalStormApp>();
    if (!self.app->initialize(self.renderer)) {
        NSLog(@"Failed to initialize FinalStorm app!");
        [NSApp terminate:nil];
    }
    
    // Connect to local server
    self.app->connectToServer("ws://localhost:3000/ws");
    
    self.startTime = CACurrentMediaTime();
    
    // Setup input handling
    [self setupInputHandling];
}

- (void)setupInputHandling {
    // Mouse tracking
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:self.view.bounds
                                                                 options:(NSTrackingMouseMoved | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect)
                                                                   owner:self
                                                                userInfo:nil];
    [self.view addTrackingArea:trackingArea];
    
    // Accept first responder for keyboard events
    [self.view.window makeFirstResponder:self.view];
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

#pragma mark - Input Handling

- (void)mouseDown:(NSEvent *)event {
    NSPoint locationInView = [self.view convertPoint:event.locationInWindow fromView:nil];
    
    FinalStorm::InputEvent inputEvent;
    inputEvent.type = FinalStorm::InputEventType::MouseDown;
    inputEvent.mouseButton = FinalStorm::MouseButton::Left;
    inputEvent.position = FinalStorm::float2(locationInView.x, self.view.bounds.size.height - locationInView.y);
    
    self.app->handleInput(inputEvent);
}

- (void)mouseDragged:(NSEvent *)event {
    NSPoint locationInView = [self.view convertPoint:event.locationInWindow fromView:nil];
    
    FinalStorm::InputEvent inputEvent;
    inputEvent.type = FinalStorm::InputEventType::MouseMove;
    inputEvent.position = FinalStorm::float2(locationInView.x, self.view.bounds.size.height - locationInView.y);
    inputEvent.delta = FinalStorm::float2(event.deltaX, -event.deltaY);
    
    self.app->handleInput(inputEvent);
}

- (void)mouseUp:(NSEvent *)event {
    NSPoint locationInView = [self.view convertPoint:event.locationInWindow fromView:nil];
    
    FinalStorm::InputEvent inputEvent;
    inputEvent.type = FinalStorm::InputEventType::MouseUp;
    inputEvent.mouseButton = FinalStorm::MouseButton::Left;
    inputEvent.position = FinalStorm::float2(locationInView.x, self.view.bounds.size.height - locationInView.y);
    
    self.app->handleInput(inputEvent);
}

- (void)keyDown:(NSEvent *)event {
    FinalStorm::InputEvent inputEvent;
    inputEvent.type = FinalStorm::InputEventType::KeyDown;
    inputEvent.keyCode = event.keyCode;
    
    self.app->handleInput(inputEvent);
}

- (void)keyUp:(NSEvent *)event {
    FinalStorm::InputEvent inputEvent;
    inputEvent.type = FinalStorm::InputEventType::KeyUp;
    inputEvent.keyCode = event.keyCode;
    
    self.app->handleInput(inputEvent);
}

@end

@interface AppDelegate : NSObject<NSApplicationDelegate>
@property (nonatomic, strong) NSWindow* window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    // Create window
    NSRect frame = NSMakeRect(0, 0, 1280, 720);
    NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:style
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
    
    self.window.title = @"FinalStorm - Immersive 3D Finalverse Client";
    [self.window center];
    
    // Create view controller
    GameViewController* viewController = [[GameViewController alloc] init];
    self.window.contentViewController = viewController;
    
    [self.window makeKeyAndOrderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;
        [app run];
    }
    return 0;
}