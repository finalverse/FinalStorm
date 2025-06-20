// src/Platform/iOS/GameViewController.h
// iOS game view controller header
// Manages the Metal view and game loop

#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

@interface GameViewController : UIViewController<MTKViewDelegate>
@end