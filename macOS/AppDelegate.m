//
//  AppDelegate.m
//  FinalStorm macOS
//
//  Created by Wenyan Qin on 2025-06-15.
//

#import "AppDelegate.h"
#import "GameViewController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];

    NSRect frame = NSMakeRect(0, 0, 800, 600);
    NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                        NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    self.window.title = @"FinalStorm";

    GameViewController *vc = [[GameViewController alloc] initWithNibName:nil bundle:nil];
    self.window.contentViewController = vc;
    [self.window makeKeyAndOrderFront:nil];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}


@end
