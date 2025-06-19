//
// src/FinalStormApp.h
// Main application class for FinalStorm 3D client using platform-agnostic MathTypes
// Manages all core systems and provides the main application lifecycle
//

#pragma once

#include "Core/Math/MathTypes.h"
#include <memory>
#include <string>
#include <map>

// Forward declarations
namespace FinalStorm {
    class SceneManager;
    class MetalRenderer;
    class FinalverseClient;
    class InteractionManager;
    class AudioEngine;
    class ServiceFactory;
    class EnvironmentController;
}

class FinalStormApp {
public:
    FinalStormApp();
    ~FinalStormApp();
    
    // Application lifecycle
    bool initialize();
    void shutdown();
    void update(float deltaTime);
    void render();
    
    // Event handling using MathTypes for positions
    void handleMouseMove(float x, float y);
    void handleMouseClick(float x, float y, bool isPressed);
    void handleKeyPress(int keyCode, bool isPressed);
    void handleResize(int width, int height);
    void handleScroll(float deltaX, float deltaY);
    
    // Touch events for iOS
    void handleTouchBegan(float x, float y, int touchId);
    void handleTouchMoved(float x, float y, int touchId);
    void handleTouchEnded(float x, float y, int touchId);
    
    // Service connection
    bool connectToFinalverse(const std::string& serverUrl);
    void disconnectFromFinalverse();
    bool isConnectedToFinalverse() const;
    
    // Camera controls using MathTypes
    void setCameraPosition(const FinalStorm::vec3& position);
    void setCameraTarget(const FinalStorm::vec3& target);
    void orbitCamera(float azimuth, float elevation);
    void panCamera(float deltaX, float deltaY);
    void zoomCamera(float delta);
    
    // Getters
    FinalStorm::SceneManager* getSceneManager() const { return m_sceneManager.get(); }
    FinalStorm::MetalRenderer* getRenderer() const { return m_renderer.get(); }
    FinalStorm::AudioEngine* getAudioEngine() const { return m_audioEngine.get(); }
    
    // Application state
    bool isInitialized() const { return m_isInitialized; }
    float getFrameRate() const { return m_frameRate; }
    
private:
    // Core systems
    std::unique_ptr<FinalStorm::SceneManager> m_sceneManager;
    std::unique_ptr<FinalStorm::MetalRenderer> m_renderer;
    std::unique_ptr<FinalStorm::FinalverseClient> m_networkClient;
    std::unique_ptr<FinalStorm::InteractionManager> m_interactionManager;
    std::unique_ptr<FinalStorm::AudioEngine> m_audioEngine;
    std::unique_ptr<FinalStorm::ServiceFactory> m_serviceFactory;
    std::unique_ptr<FinalStorm::EnvironmentController> m_environmentController;
    
    // Application state
    bool m_isInitialized;
    bool m_isConnected;
    float m_lastUpdateTime;
    float m_frameTime;
    float m_frameRate;
    
    // Window properties
    int m_windowWidth;
    int m_windowHeight;
    
    // Input state using MathTypes
    FinalStorm::vec2 m_lastMousePosition;
    FinalStorm::vec2 m_mouseDelta;
    bool m_mousePressed;
    std::map<int, bool> m_keyStates;
    
    // Touch state for iOS
    struct TouchState {
        FinalStorm::vec2 position;
        FinalStorm::vec2 lastPosition;
        bool active;
    };
    std::map<int, TouchState> m_touchStates;
    
    // Camera interaction state
    bool m_cameraControlEnabled;
    float m_cameraSensitivity;
    float m_cameraZoomSpeed;
    
    // Performance metrics
    float m_frameTimeAccumulator;
    int m_frameCount;
    float m_averageFrameTime;
    
    // Helper methods
    void initializeSystems();
    void setupScene();
    void updateSystems(float deltaTime);
    void updatePerformanceMetrics(float deltaTime);
    void handleServiceUpdates();
    
    // Input processing
    void processMouseInput();
    void processKeyboardInput();
    void processTouchInput();
    
    // Camera control helpers using MathTypes
    void updateCameraFromMouse(const FinalStorm::vec2& mouseDelta);
    void updateCameraFromTouch();
    FinalStorm::vec2 screenToNormalizedCoords(float x, float y) const;
    
    // Event callbacks
    void onServiceDiscovered(const std::string& serviceName, const std::string& serviceType);
    void onServiceMetricsUpdated(const std::string& serviceName, const std::map<std::string, float>& metrics);
    void onNetworkEvent(const std::string& eventType, const std::string& data);
    
    // Audio integration
    void updateAudioListener();
    void playUISound(const std::string& soundName);
    
    // Debug and diagnostics
    void logPerformanceStats();
    void validateSystemStates();
    
#ifdef __APPLE__
    // Metal-specific initialization
    bool initializeMetalView(void* metalView);
    void setupMetalRenderer();
#endif
};

// Global helper functions for platform integration
extern FinalStormApp* GetGlobalApp();
extern void CreateGlobalApp();
extern void DestroyGlobalApp();