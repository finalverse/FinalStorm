// src/FinalStormApp.cpp
// Main application class implementation
// Manages the renderer, scene, and game loop

#include "FinalStormApp.h"
#include "Scene/Scene.h"
#include "Services/ServiceFactory.h"
#include "Environment/EnvironmentController.h"
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
    
    // Create scene
    scene = std::make_shared<Scene>();
    
    // Create scene manager
    sceneManager = std::make_unique<SceneManager>(scene, renderer);
    
    // Create networking client
    networkClient = std::make_unique<FinalverseClient>();
    
    // Create input manager
    inputManager = std::make_unique<InteractionManager>();
    
    // Initialize subsystems
    if (!initializeScene()) {
        std::cerr << "Failed to initialize scene!" << std::endl;
        return false;
    }
    
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

bool FinalStormApp::initializeScene() {
    // Create environment
    auto environment = std::make_shared<EnvironmentController>();
    scene->getRoot()->addChild(environment);
    
    // Create service discovery UI
    auto discoveryUI = std::make_shared<ServiceDiscoveryUI>();
    discoveryUI->setPosition(float3(0, 2, -5));
    scene->getRoot()->addChild(discoveryUI);
    
    // Create some test services
    createTestServices();
    
    // Setup camera
    setupCamera();
    
    return true;
}

void FinalStormApp::createTestServices() {
    ServiceFactory factory;
    
    // Create API Gateway visualization
    auto apiGateway = factory.createService(ServiceType::APIGateway);
    apiGateway->setPosition(float3(-5, 0, 0));
    scene->getRoot()->addChild(apiGateway);
    
    // Create World Engine visualization
    auto worldEngine = factory.createService(ServiceType::WorldEngine);
    worldEngine->setPosition(float3(0, 0, 0));
    scene->getRoot()->addChild(worldEngine);
    
    // Create AI Service visualization
    auto aiService = factory.createService(ServiceType::AIService);
    aiService->setPosition(float3(5, 0, 0));
    scene->getRoot()->addChild(aiService);
}

void FinalStormApp::setupCamera() {
    if (!sceneManager) {
        return;
    }
    
    auto& camera = sceneManager->getCamera();
    camera.setPosition(float3(0, 5, 10));
    camera.lookAt(float3(0, 5, 10), float3(0, 0, 0), float3(0, 1, 0));
    camera.setFOV(Math::radians(60.0f));
    camera.setNearFar(0.1f, 1000.0f);
}

} // namespace FinalStorm