// src/FinalStormApp.h
// Main application class header
// Manages the application lifecycle

#pragma once
#include <memory>
#include <string>

namespace FinalStorm {

class Scene;
class SceneManager;
class Renderer;
class FinalverseClient;
class InteractionManager;
struct InputEvent;

class FinalStormApp {
public:
    FinalStormApp();
    ~FinalStormApp();
    
    bool initialize(std::shared_ptr<Renderer> renderer);
    void shutdown();
    
    void update(float currentTime);
    void render();
    
    void handleInput(const InputEvent& event);
    void resize(uint32_t width, uint32_t height);
    
    bool connectToServer(const std::string& url);
    
    bool isRunning() const { return isRunning; }
    
private:
    bool initializeScene();
    void createTestServices();
    void setupCamera();
    
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Scene> scene;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<FinalverseClient> networkClient;
    std::unique_ptr<InteractionManager> inputManager;
    
    bool isRunning;
    float currentTime;
    float deltaTime;
    float lastFrameTime;
};

} // namespace FinalStorm