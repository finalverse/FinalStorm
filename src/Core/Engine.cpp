#include "Core/Engine.h"
#include "Core/Logger.h"
#include "Rendering/Renderer.h"
#include "Scene/Scene.h"
#include "Network/NetworkManager.h"
#include "Input/InputManager.h"
#include <chrono>

namespace FinalStorm {

Engine* Engine::instance = nullptr;

Engine::Engine() 
    : running(false)
    , targetFPS(60.0f)
    , currentFPS(0.0f)
    , deltaTime(0.0f) {
    instance = this;
}

Engine::~Engine() {
    shutdown();
    instance = nullptr;
}

Engine* Engine::getInstance() {
    return instance;
}

bool Engine::initialize(const EngineConfig& config) {
    Logger::info("Initializing FinalStorm Engine...");
    
    // Initialize subsystems
    renderer = std::make_unique<Renderer>();
    if (!renderer->initialize(config.windowWidth, config.windowHeight, config.windowTitle)) {
        Logger::error("Failed to initialize renderer");
        return false;
    }
    
    scene = std::make_unique<Scene>();
    networkManager = std::make_unique<NetworkManager>();
    inputManager = std::make_unique<InputManager>();
    
    // Initialize input manager with the renderer's window
    inputManager->initialize(renderer->getWindow());
    
    // Connect to Finalverse server if URL provided
    if (!config.serverUrl.empty()) {
        networkManager->connect(config.serverUrl);
    }
    
    running = true;
    Logger::info("FinalStorm Engine initialized successfully");
    return true;
}

void Engine::shutdown() {
    if (running) {
        running = false;
        
        if (networkManager) {
            networkManager->disconnect();
        }
        
        if (renderer) {
            renderer->shutdown();
        }
        
        Logger::info("FinalStorm Engine shut down");
    }
}

void Engine::run() {
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    
    while (running) {
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;
        
        // Update FPS
        currentFPS = 1.0f / deltaTime;
        
        // Process input
        inputManager->update();
        if (inputManager->shouldQuit()) {
            running = false;
            break;
        }
        
        // Update systems
        update(deltaTime);
        
        // Render frame
        render();
        
        // Frame rate limiting
        limitFrameRate();
    }
}

void Engine::update(float deltaTime) {
    // Update network
    if (networkManager) {
        networkManager->update();
    }
    
    // Update scene
    if (scene) {
        scene->update(deltaTime);
    }
}

void Engine::render() {
    if (renderer && scene) {
        renderer->beginFrame();
        renderer->render(scene.get());
        renderer->endFrame();
    }
}

void Engine::limitFrameRate() {
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto targetFrameDuration = std::chrono::duration<float>(1.0f / targetFPS);
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = currentTime - lastFrameTime;
    
    if (elapsed < targetFrameDuration) {
        std::this_thread::sleep_for(targetFrameDuration - elapsed);
    }
    
    lastFrameTime = std::chrono::high_resolution_clock::now();
}

} // namespace FinalStorm