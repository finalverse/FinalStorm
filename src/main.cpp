//
// main.cpp - Application Entry Point
// FinalStorm 3D Client for Finalverse
//

#include "FinalStormApp.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include "Platform/macOS/GameViewController.h"
#elif TARGET_OS_IOS
#include "Platform/iOS/GameViewController.h"
#endif
#endif

// Global application instance
std::unique_ptr<FinalStormApp> g_app;

// Platform-specific main function for console/testing
int console_main() {
    std::cout << "FinalStorm - Finalverse 3D Client" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Create application
    g_app = std::make_unique<FinalStormApp>();
    
    // Initialize
    if (!g_app->initialize()) {
        std::cerr << "Failed to initialize FinalStorm!" << std::endl;
        return -1;
    }
    
    // Connect to Finalverse (default local server)
    const std::string serverUrl = "ws://localhost:3000/ws";
    if (!g_app->connectToFinalverse(serverUrl)) {
        std::cout << "Warning: Could not connect to Finalverse server at " << serverUrl << std::endl;
        std::cout << "Running in offline mode..." << std::endl;
    }
    
    // Main loop for console mode (basic timing)
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float targetFPS = 60.0f;
    const auto targetFrameTime = std::chrono::microseconds(static_cast<long>(1000000.0f / targetFPS));
    
    std::cout << "Entering main loop (console mode)..." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Update application
        g_app->update(deltaTime);
        
        // Render (this will be a no-op in console mode without proper window)
        g_app->render();
        
        // Sleep to maintain target framerate
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto frameTime = frameEndTime - currentTime;
        if (frameTime < targetFrameTime) {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }
    }
    
    // Cleanup
    g_app->shutdown();
    g_app.reset();
    
    return 0;
}

#ifdef __APPLE__
#if TARGET_OS_OSX || TARGET_OS_IOS

// For macOS/iOS, the main function is handled by the platform-specific code
// The actual application lifecycle is managed by GameViewController
int main(int argc, char* argv[]) {
    std::cout << "FinalStorm starting on Apple platform..." << std::endl;
    
    // Check for console mode flag
    bool consoleMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--console") {
            consoleMode = true;
            break;
        }
    }
    
    if (consoleMode) {
        return console_main();
    }
    
#if TARGET_OS_OSX
    // On macOS, use AppKit main
    return NSApplicationMain(argc, (const char**)argv);
#elif TARGET_OS_IOS
    // On iOS, use UIKit main
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
#endif
}

#else
// Non-Apple platforms
int main(int argc, char* argv[]) {
    return console_main();
}
#endif

#else
// Non-Apple platforms
int main(int argc, char* argv[]) {
    return console_main();
}
#endif

// Global functions for platform integration
FinalStormApp* GetGlobalApp() {
    return g_app.get();
}

void CreateGlobalApp() {
    if (!g_app) {
        g_app = std::make_unique<FinalStormApp>();
    }
}

void DestroyGlobalApp() {
    if (g_app) {
        g_app->shutdown();
        g_app.reset();
    }
}