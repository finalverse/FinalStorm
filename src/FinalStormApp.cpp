// src/FinalStormApp.cpp
// Main application class implementation
// Manages the renderer, scene, and game loop

#include "FinalStormApp.h"
#include "Scene/Scene.h"
#include "Scene/SceneLoader.h"
#include "World/WorldManager.h"
#include "Rendering/Metal/MetalRenderer.h"
#include <iostream>

namespace FinalStorm {

FinalStormApp::FinalStormApp()
    : isRunning(false)
    , currentTime(0.0f)
    , deltaTime(0.0f)
    , lastFrameTime(0.0f) {
}

FinalStormApp::~FinalStormApp() {
    shutdown();
}

bool FinalStormApp::initialize(std::shared_ptr<Renderer> rend) {
    std::cout << "Initializing FinalStorm App..." << std::endl;
    
    renderer = rend;
    if (!renderer) {
        std::cerr << "Error: No renderer provided!" << std::endl;
        return false;
    }
    
    // Create world manager
    worldManager = std::make_unique<WorldManager>();

    // Create networking client
    networkClient = std::make_unique<FinalverseClient>();

    // Create scene loader
    auto* metalRenderer = dynamic_cast<MetalRenderer*>(renderer.get());
    sceneLoader = std::make_unique<SceneLoader>(worldManager.get(), metalRenderer, networkClient.get());

    // Load the first scene and set it active
    sceneLoader->loadFirstScene();
    scene = std::shared_ptr<Scene>(sceneLoader->getCurrentScene(), [](Scene*){});

    // Create scene manager
    sceneManager = std::make_unique<SceneManager>(scene, renderer);

    // Create input manager
    inputManager = std::make_unique<InteractionManager>();
    
    isRunning = true;
    return true;
}

void FinalStormApp::shutdown() {
    isRunning = false;
    
    if (networkClient) {
        networkClient->disconnect();
    }
    
    scene.reset();
    sceneManager.reset();
    networkClient.reset();
    inputManager.reset();
    renderer.reset();
}

void FinalStormApp::update(float currentTimeSeconds) {
    // Calculate delta time
    currentTime = currentTimeSeconds;
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    
    // Clamp delta time to prevent large jumps
    deltaTime = std::min(deltaTime, 0.1f);
    
    // Update subsystems
    if (scene) {
        scene->update(deltaTime);
    }
    
    if (sceneManager) {
        sceneManager->update(deltaTime);
    }
    
    if (networkClient && networkClient->isConnected()) {
        networkClient->update();
    }
    
    if (inputManager) {
        inputManager->update(deltaTime);
    }
}

void FinalStormApp::render() {
    if (!renderer || !scene) {
        return;
    }
    
    // Begin frame
    renderer->beginFrame();
    
    // Render scene
    sceneManager->render();
    
    // End frame
    renderer->endFrame();
}

void FinalStormApp::handleInput(const InputEvent& event) {
    if (inputManager) {
        inputManager->handleEvent(event);
    }
}

void FinalStormApp::resize(uint32_t width, uint32_t height) {
    if (renderer) {
        renderer->resize(width, height);
    }
    
    if (sceneManager) {
        sceneManager->onResize(width, height);
    }
}

bool FinalStormApp::connectToServer(const std::string& url) {
    if (!networkClient) {
        return false;
    }
    
    return networkClient->connect(url);
}


} // namespace FinalStorm