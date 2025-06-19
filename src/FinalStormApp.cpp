//
// src/FinalStormApp.cpp
// Main application implementation using platform-agnostic MathTypes
// Handles system initialization, updates, and platform-specific integration
//

#include "FinalStormApp.h"
#include "Scene/SceneManager.h"
#include "Rendering/Metal/MetalRenderer.h"
#include "Core/Networking/FinalverseClient.h"
#include "Core/Input/InteractionManager.h"
#include "Core/Audio/AudioEngine.h"
#include "Services/ServiceFactory.h"
#include "Environment/EnvironmentController.h"
#include "Scene/Scene.h"
#include "Scene/CameraController.h"

#include <iostream>
#include <chrono>
#include <algorithm>

using namespace FinalStorm;

FinalStormApp::FinalStormApp()
    : m_isInitialized(false)
    , m_isConnected(false)
    , m_lastUpdateTime(0.0f)
    , m_frameTime(0.0f)
    , m_frameRate(60.0f)
    , m_windowWidth(1920)
    , m_windowHeight(1080)
    , m_lastMousePosition(make_vec2(0.0f, 0.0f))
    , m_mouseDelta(make_vec2(0.0f, 0.0f))
    , m_mousePressed(false)
    , m_cameraControlEnabled(true)
    , m_cameraSensitivity(0.005f)
    , m_cameraZoomSpeed(1.0f)
    , m_frameTimeAccumulator(0.0f)
    , m_frameCount(0)
    , m_averageFrameTime(0.0f)
{
}

FinalStormApp::~FinalStormApp() {
    shutdown();
}

bool FinalStormApp::initialize() {
    if (m_isInitialized) {
        return true;
    }
    
    std::cout << "FinalStormApp: Initializing FinalStorm with MathTypes..." << std::endl;
    
    try {
        initializeSystems();
        setupScene();
        
        m_isInitialized = true;
        std::cout << "FinalStormApp: Initialized successfully!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "FinalStormApp: Failed to initialize: " << e.what() << std::endl;
        return false;
    }
}

void FinalStormApp::shutdown() {
    if (!m_isInitialized) {
        return;
    }
    
    std::cout << "FinalStormApp: Shutting down..." << std::endl;
    
    disconnectFromFinalverse();
    
    // Shutdown systems in reverse order
    m_environmentController.reset();
    m_serviceFactory.reset();
    m_audioEngine.reset();
    m_interactionManager.reset();
    m_networkClient.reset();
    m_renderer.reset();
    m_sceneManager.reset();
    
    m_isInitialized = false;
    std::cout << "FinalStormApp: Shutdown complete." << std::endl;
}

void FinalStormApp::update(float deltaTime) {
    if (!m_isInitialized) {
        return;
    }
    
    m_frameTime = deltaTime;
    updatePerformanceMetrics(deltaTime);
    
    // Process input
    processMouseInput();
    processKeyboardInput();
    processTouchInput();
    
    // Update all systems
    updateSystems(deltaTime);
    
    // Handle network events
    if (m_isConnected) {
        handleServiceUpdates();
    }
    
    // Update audio listener based on camera
    updateAudioListener();
    
    // Validate system states in debug builds
#ifdef DEBUG
    validateSystemStates();
#endif
}

void FinalStormApp::render() {
    if (!m_isInitialized || !m_renderer) {
        return;
    }
    
    m_renderer->beginFrame();
    
    if (m_sceneManager) {
        m_sceneManager->render(*m_renderer);
    }
    
    m_renderer->endFrame();
}

void FinalStormApp::handleMouseMove(float x, float y) {
    vec2 currentPos = make_vec2(x, y);
    m_mouseDelta = currentPos - m_lastMousePosition;
    m_lastMousePosition = currentPos;
    
    if (m_interactionManager) {
        m_interactionManager->handleMouseMove(x, y);
    }
}

void FinalStormApp::handleMouseClick(float x, float y, bool isPressed) {
    m_mousePressed = isPressed;
    
    if (m_interactionManager) {
        m_interactionManager->handleMouseClick(x, y, isPressed);
    }
    
    // Play UI feedback sound
    if (isPressed) {
        playUISound("click");
    }
}

void FinalStormApp::handleKeyPress(int keyCode, bool isPressed) {
    m_keyStates[keyCode] = isPressed;
    
    if (m_interactionManager) {
        m_interactionManager->handleKeyPress(keyCode, isPressed);
    }
    
    // Handle camera controls
    if (m_cameraControlEnabled && isPressed) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            const float moveSpeed = 5.0f;
            
            switch (keyCode) {
                case 13: // W or Forward
                    camera->moveForward(moveSpeed);
                    break;
                case 1: // S or Backward
                    camera->moveForward(-moveSpeed);
                    break;
                case 0: // A or Left
                    camera->moveRight(-moveSpeed);
                    break;
                case 2: // D or Right
                    camera->moveRight(moveSpeed);
                    break;
                case 49: // Space or Up
                    camera->moveUp(moveSpeed);
                    break;
                case 256: // Shift or Down
                    camera->moveUp(-moveSpeed);
                    break;
            }
        }
    }
}

void FinalStormApp::handleResize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    
    if (m_renderer) {
        m_renderer->handleResize(width, height);
    }
    
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->setAspectRatio(static_cast<float>(width) / height);
        }
    }
}

void FinalStormApp::handleScroll(float deltaX, float deltaY) {
    if (m_cameraControlEnabled && m_sceneManager) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->zoom(deltaY * m_cameraZoomSpeed);
        }
    }
}

void FinalStormApp::handleTouchBegan(float x, float y, int touchId) {
    TouchState& touch = m_touchStates[touchId];
    touch.position = make_vec2(x, y);
    touch.lastPosition = touch.position;
    touch.active = true;
}

void FinalStormApp::handleTouchMoved(float x, float y, int touchId) {
    auto it = m_touchStates.find(touchId);
    if (it != m_touchStates.end() && it->second.active) {
        TouchState& touch = it->second;
        touch.lastPosition = touch.position;
        touch.position = make_vec2(x, y);
    }
}

void FinalStormApp::handleTouchEnded(float x, float y, int touchId) {
    auto it = m_touchStates.find(touchId);
    if (it != m_touchStates.end()) {
        it->second.active = false;
        m_touchStates.erase(it);
    }
}

bool FinalStormApp::connectToFinalverse(const std::string& serverUrl) {
    if (!m_networkClient) {
        std::cerr << "FinalStormApp: Network client not initialized!" << std::endl;
        return false;
    }
    
    std::cout << "FinalStormApp: Connecting to Finalverse at: " << serverUrl << std::endl;
    
    // Set up callbacks
    m_networkClient->setOnServiceDiscovered([this](const std::string& name, const std::string& type) {
        onServiceDiscovered(name, type);
    });
    
    m_networkClient->setOnServiceMetricsUpdated([this](const std::string& name, const std::map<std::string, float>& metrics) {
        onServiceMetricsUpdated(name, metrics);
    });
    
    m_networkClient->setOnNetworkEvent([this](const std::string& eventType, const std::string& data) {
        onNetworkEvent(eventType, data);
    });
    
    if (m_networkClient->connect(serverUrl)) {
        m_isConnected = true;
        std::cout << "FinalStormApp: Connected to Finalverse successfully!" << std::endl;
        playUISound("connect");
        return true;
    }
    
    std::cerr << "FinalStormApp: Failed to connect to Finalverse!" << std::endl;
    return false;
}

void FinalStormApp::disconnectFromFinalverse() {
    if (m_networkClient && m_isConnected) {
        m_networkClient->disconnect();
        m_isConnected = false;
        std::cout << "FinalStormApp: Disconnected from Finalverse." << std::endl;
        playUISound("disconnect");
    }
}

bool FinalStormApp::isConnectedToFinalverse() const {
    return m_isConnected;
}

void FinalStormApp::setCameraPosition(const vec3& position) {
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->setPosition(position);
        }
    }
}

void FinalStormApp::setCameraTarget(const vec3& target) {
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->setTarget(target);
        }
    }
}

void FinalStormApp::orbitCamera(float azimuth, float elevation) {
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->orbit(azimuth, elevation);
        }
    }
}

void FinalStormApp::panCamera(float deltaX, float deltaY) {
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->pan(deltaX, deltaY);
        }
    }
}

void FinalStormApp::zoomCamera(float delta) {
    if (m_sceneManager && m_sceneManager->getCurrentScene()) {
        auto camera = m_sceneManager->getCurrentScene()->getCameraController();
        if (camera) {
            camera->zoom(delta);
        }
    }
}

// Private methods
void FinalStormApp::initializeSystems() {
    // Initialize renderer first (Metal on Apple platforms)
    m_renderer = std::make_unique<MetalRenderer>();
    if (!m_renderer->initialize()) {
        throw std::runtime_error("Failed to initialize Metal renderer");
    }
    
    // Initialize scene manager
    m_sceneManager = std::make_unique<SceneManager>();
    
    // Initialize audio engine
    m_audioEngine = std::make_unique<AudioEngine>();
    if (!m_audioEngine->initialize()) {
        throw std::runtime_error("Failed to initialize audio engine");
    }
    
    // Initialize network client
    m_networkClient = std::make_unique<FinalverseClient>();
    
    // Initialize interaction manager
    m_interactionManager = std::make_unique<InteractionManager>();
    m_interactionManager->setSceneManager(m_sceneManager.get());
    
    // Initialize service factory
    m_serviceFactory = std::make_unique<ServiceFactory>();
    m_serviceFactory->setRenderer(m_renderer.get());
    
    // Initialize environment controller
    m_environmentController = std::make_unique<EnvironmentController>();
    m_environmentController->setRenderer(m_renderer.get());
}

void FinalStormApp::setupScene() {
    if (!m_sceneManager) {
        return;
    }
    
    // Create main scene
    auto mainScene = std::make_shared<Scene>("MainScene");
    
    // Set up camera using MathTypes
    auto cameraController = std::make_shared<CameraController>();
    cameraController->setPosition(make_vec3(0.0f, 5.0f, 10.0f));
    cameraController->setTarget(vec3_zero());
    cameraController->setAspectRatio(static_cast<float>(m_windowWidth) / m_windowHeight);
    mainScene->setCameraController(cameraController);
    
    // Add environment
    if (m_environmentController) {
        m_environmentController->setupEnvironment(mainScene.get());
    }
    
    // Set as current scene
    m_sceneManager->setCurrentScene(mainScene);
    
    std::cout << "FinalStormApp: Scene setup complete with MathTypes." << std::endl;
}

void FinalStormApp::updateSystems(float deltaTime) {
    // Update scene
    if (m_sceneManager) {
        m_sceneManager->update(deltaTime);
    }
    
    // Update environment
    if (m_environmentController) {
        m_environmentController->update(deltaTime);
    }
    
    // Update audio
    if (m_audioEngine) {
        m_audioEngine->update(deltaTime);
    }
    
    // Update network client
    if (m_networkClient) {
        m_networkClient->update(deltaTime);
    }
    
    // Update renderer time
    if (m_renderer) {
        m_renderer->setTime(m_lastUpdateTime + deltaTime);
    }
    
    m_lastUpdateTime += deltaTime;
}

void FinalStormApp::updatePerformanceMetrics(float deltaTime) {
    m_frameTimeAccumulator += deltaTime;
    m_frameCount++;
    
    // Update every second
    if (m_frameTimeAccumulator >= 1.0f) {
        m_averageFrameTime = m_frameTimeAccumulator / m_frameCount;
        m_frameRate = 1.0f / m_averageFrameTime;
        
        // Reset counters
        m_frameTimeAccumulator = 0.0f;
        m_frameCount = 0;
        
        // Log performance stats periodically
        static int logCounter = 0;
        if (++logCounter >= 10) { // Log every 10 seconds
            logPerformanceStats();
            logCounter = 0;
        }
    }
}

void FinalStormApp::processMouseInput() {
    if (m_cameraControlEnabled && m_mousePressed && length(m_mouseDelta) > 0.001f) {
        updateCameraFromMouse(m_mouseDelta);
    }
}

void FinalStormApp::processKeyboardInput() {
    // Process continuous key states for smooth camera movement
    // This is handled in handleKeyPress for now
}

void FinalStormApp::processTouchInput() {
    // Handle multi-touch gestures for camera control on iOS
    updateCameraFromTouch();
}

void FinalStormApp::updateCameraFromMouse(const vec2& mouseDelta) {
    if (!m_sceneManager || !m_sceneManager->getCurrentScene()) {
        return;
    }
    
    auto camera = m_sceneManager->getCurrentScene()->getCameraController();
    if (!camera) {
        return;
    }
    
    // Convert mouse delta to camera rotation
    float azimuth = -mouseDelta.x * m_cameraSensitivity;
    float elevation = -mouseDelta.y * m_cameraSensitivity;
    
    camera->orbit(azimuth, elevation);
}

void FinalStormApp::updateCameraFromTouch() {
    if (m_touchStates.size() == 1) {
        // Single touch - orbit camera
        auto& touch = m_touchStates.begin()->second;
        if (touch.active) {
            vec2 delta = touch.position - touch.lastPosition;
            updateCameraFromMouse(delta);
        }
    }
    else if (m_touchStates.size() == 2) {
        // Two finger gesture - zoom or pan
        auto it1 = m_touchStates.begin();
        auto it2 = std::next(it1);
        
        if (it1->second.active && it2->second.active) {
            // Calculate distance change for zoom
            float currentDist = length(it1->second.position - it2->second.position);
            float lastDist = length(it1->second.lastPosition - it2->second.lastPosition);
            float distDelta = currentDist - lastDist;
            
            if (abs(distDelta) > 1.0f) {
                zoomCamera(-distDelta * 0.01f);
            }
        }
    }
}

vec2 FinalStormApp::screenToNormalizedCoords(float x, float y) const {
    return make_vec2(
        (2.0f * x) / m_windowWidth - 1.0f,
        1.0f - (2.0f * y) / m_windowHeight
    );
}

void FinalStormApp::handleServiceUpdates() {
    if (!m_networkClient) {
        return;
    }
    
    // Process any pending network messages
    m_networkClient->processMessages();
}

void FinalStormApp::onServiceDiscovered(const std::string& serviceName, const std::string& serviceType) {
    std::cout << "FinalStormApp: Service discovered: " << serviceName << " (type: " << serviceType << ")" << std::endl;
    
    if (m_serviceFactory && m_sceneManager) {
        // Create visualization for the service
        auto serviceViz = m_serviceFactory->createServiceVisualization(serviceName, serviceType);
        if (serviceViz) {
            auto currentScene = m_sceneManager->getCurrentScene();
            if (currentScene) {
                currentScene->addEntity(serviceViz);
                
                // Position service visualization using MathTypes
                float angle = static_cast<float>(currentScene->getEntityCount()) * TWO_PI / 8.0f;
                vec3 position = make_vec3(cos(angle) * 10.0f, 0.0f, sin(angle) * 10.0f);
                serviceViz->setPosition(position);
            }
        }
    }
    
    playUISound("service_discovered");
}

void FinalStormApp::onServiceMetricsUpdated(const std::string& serviceName, const std::map<std::string, float>& metrics) {
    // Update service visualization with new metrics
    if (m_sceneManager) {
        auto currentScene = m_sceneManager->getCurrentScene();
        if (currentScene) {
            auto entity = currentScene->findEntityByName(serviceName);
            if (entity) {
                entity->updateMetrics(metrics);
            }
        }
    }
}

void FinalStormApp::onNetworkEvent(const std::string& eventType, const std::string& data) {
    std::cout << "FinalStormApp: Network event: " << eventType << " - " << data << std::endl;
    
    if (eventType == "connection_lost") {
        m_isConnected = false;
        playUISound("connection_lost");
    }
    else if (eventType == "world_update") {
        // Handle world state updates
        playUISound("world_update");
    }
}

void FinalStormApp::updateAudioListener() {
    if (!m_audioEngine || !m_sceneManager) {
        return;
    }
    
    auto currentScene = m_sceneManager->getCurrentScene();
    if (!currentScene) {
        return;
    }
    
    auto camera = currentScene->getCameraController();
    if (camera) {
        m_audioEngine->setListenerPosition(camera->getPosition());
        // Set listener orientation based on camera forward and up vectors
        // m_audioEngine->setListenerOrientation(camera->getForwardVector(), camera->getUpVector());
    }
}

void FinalStormApp::playUISound(const std::string& soundName) {
    if (m_audioEngine) {
        // Create and play UI sound
        auto audioSource = m_audioEngine->createAudioSource();
        if (audioSource) {
            // Load sound file based on name
            std::string filename = "assets/audio/ui/" + soundName + ".wav";
            if (audioSource->loadFromFile(filename)) {
                audioSource->setVolume(m_audioEngine->getEffectsVolume());
                audioSource->play();
            }
        }
    }
}

void FinalStormApp::logPerformanceStats() {
    std::cout << "FinalStormApp: Performance - FPS: " << m_frameRate 
              << ", Frame Time: " << (m_averageFrameTime * 1000.0f) << "ms" << std::endl;
}

void FinalStormApp::validateSystemStates() {
    // Validate that all systems are in expected states
    if (!m_isInitialized) {
        std::cerr << "FinalStormApp: Warning - Update called on uninitialized app!" << std::endl;
    }
    
    if (!m_renderer) {
        std::cerr << "FinalStormApp: Warning - Renderer is null!" << std::endl;
    }
    
    if (!m_sceneManager) {
        std::cerr << "FinalStormApp: Warning - Scene manager is null!" << std::endl;
    }
}

} // End of anonymous namespace (implementation details)

// Global app instance functions
static std::unique_ptr<FinalStormApp> g_app;

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