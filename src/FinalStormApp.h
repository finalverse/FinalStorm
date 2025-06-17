// FinalStormApp.h - Main application class for FinalStorm

#pragma once

#include <memory>
#include <string>

namespace FinalStorm {

// Forward declarations
class WorldManager;
class SceneManager;
class MetalRenderer;
class FinalverseClient;
class InputManager;

class FinalStormApp {
public:
    FinalStormApp();
    ~FinalStormApp();
    
    // Run the application
    int run(int argc, char* argv[]);
    
    // Application lifecycle
    void initialize();
    void shutdown();
    
    // Main loop
    void update(float deltaTime);
    void render();
    
private:
    bool m_isRunning;
    
    // Core systems
    std::unique_ptr<WorldManager> m_worldManager;
    std::unique_ptr<SceneManager> m_sceneManager;
    std::unique_ptr<MetalRenderer> m_renderer;
    std::unique_ptr<FinalverseClient> m_networkClient;
    std::unique_ptr<InputManager> m_inputManager;
    
    // Timing
    float m_lastFrameTime;
    float m_deltaTime;
    
    // Configuration
    struct Config {
        int windowWidth = 1920;
        int windowHeight = 1080;
        bool fullscreen = false;
        std::string serverUrl = "ws://localhost:3000/ws";
    } m_config;
    
    void loadConfiguration(int argc, char* argv[]);
    void setupSystems();
    void connectToFinalverse();
};

} // namespace FinalStorm
