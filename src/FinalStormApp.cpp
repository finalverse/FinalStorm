// FinalStormApp.cpp - Main application implementation

#include "Scene/SceneNode.h"
#include "Services/ServiceVisualizations.h"
#include "Renderer/Metal/MetalRenderer.h"
#include "NetworkClient.h"
#include "InputManager.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace FinalStorm {

class FinalStormApp {
public:
    FinalStormApp() : 
        isRunning(true),
        lastFrameTime(std::chrono::high_resolution_clock::now()) {
        
        initialize();
    }
    
    ~FinalStormApp() {
        shutdown();
    }
    
    void run() {
        while (isRunning) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            
            // Process input
            inputManager->processEvents();
            
            // Update network
            networkClient->update();
            
            // Update scene
            updateScene(deltaTime);
            
            // Render
            renderer->beginFrame();
            scene->render(*renderer->getRenderContext());
            renderer->endFrame();
            
            // Frame rate limiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    }
    
private:
    void initialize() {
        std::cout << "Initializing FinalStorm..." << std::endl;
        
        // Create renderer
        renderer = std::make_unique<MetalRenderer>();
        renderer->initialize(1920, 1080);
        
        // Create scene
        scene = std::make_unique<Scene>();
        
        // Set up camera
        setupCamera();
        
        // Create environment
        setupEnvironment();
        
        // Initialize network client
        setupNetworking();
        
        // Set up input handling
        setupInput();
        
        // Create UI
        setupUI();
        
        std::cout << "FinalStorm initialized successfully!" << std::endl;
    }
    
    void setupCamera() {
        camera = std::make_shared<CameraNode>();
        camera->setPerspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition(glm::vec3(0, 5, 15));
        camera->lookAt(glm::vec3(0, 0, 0));
        
        scene->getRoot()->addChild(camera);
        renderer->setCamera(camera);
        
        // Camera controller
        cameraController = std::make_unique<CameraController>(camera);
    }
    
    void setupEnvironment() {
        // Create immersive environment
        environment = std::make_shared<FinalverseEnvironment>();
        scene->getRoot()->addChild(environment);
        
        // Add main lighting
        auto mainLight = std::make_shared<LightNode>(LightNode::DIRECTIONAL);
        mainLight->setColor(glm::vec3(1.0f, 0.95f, 0.8f));
        mainLight->setIntensity(0.8f);
        mainLight->setRotation(glm::angleAxis(glm::radians(-45.0f), glm::vec3(1, 0, 0)));
        scene->getRoot()->addChild(mainLight);
        
        // Add ambient lighting
        auto ambientLight1 = std::make_shared<LightNode>(LightNode::POINT);
        ambientLight1->setPosition(glm::vec3(-10, 10, -10));
        ambientLight1->setColor(glm::vec3(0.0f, 0.5f, 1.0f));
        ambientLight1->setIntensity(0.5f);
        ambientLight1->setRange(50.0f);
        scene->getRoot()->addChild(ambientLight1);
        
        auto ambientLight2 = std::make_shared<LightNode>(LightNode::POINT);
        ambientLight2->setPosition(glm::vec3(10, 10, 10));
        ambientLight2->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
        ambientLight2->setIntensity(0.5f);
        ambientLight2->setRange(50.0f);
        scene->getRoot()->addChild(ambientLight2);
    }
    
    void setupNetworking() {
        networkClient = std::make_unique<FinalverseNetworkClient>();
        
        // Set up callbacks
        networkClient->onServiceDiscovered = [this](const ServiceInfo& info) {
            std::cout << "Discovered service: " << info.name << std::endl;
            
            // Add to available services
            availableServices.push_back(info);
            
            // Update UI
            if (serviceDiscoveryUI) {
                serviceDiscoveryUI->setAvailableServices(availableServices);
            }
        };
        
        networkClient->onServiceMetricsUpdate = [this](const std::string& serviceId, 
                                                       const ServiceMetrics& metrics) {
            // Update the corresponding service visualization
            auto it = activeServices.find(serviceId);
            if (it != activeServices.end()) {
                it->second->updateMetrics(metrics);
            }
            
            // Update environment based on overall system health
            updateSystemHealth();
        };
        
        // Connect to Finalverse server
        std::string serverUrl = "wss://finalverse.network/api/v1";
        networkClient->connect(serverUrl);
    }
    
    void setupInput() {
        inputManager = std::make_unique<InputManager>();
        
        // Camera controls
        inputManager->onMouseMove = [this](float dx, float dy) {
            if (inputManager->isMouseButtonPressed(0)) { // Left mouse button
                cameraController->rotate(dx * 0.01f, dy * 0.01f);
            }
        };
        
        inputManager->onScroll = [this](float delta) {
            cameraController->zoom(delta * 0.1f);
        };
        
        inputManager->onKeyPress = [this](int key) {
            switch (key) {
                case 'W': cameraController->moveForward(1.0f); break;
                case 'S': cameraController->moveForward(-1.0f); break;
                case 'A': cameraController->moveRight(-1.0f); break;
                case 'D': cameraController->moveRight(1.0f); break;
                case 'Q': cameraController->moveUp(-1.0f); break;
                case 'E': cameraController->moveUp(1.0f); break;
                case 'F': toggleFullscreen(); break;
                case 'TAB': toggleServiceDiscovery(); break;
                case 'ESC': isRunning = false; break;
            }
        };
        
        // Touch controls for iOS
        inputManager->onTouch = [this](const TouchEvent& event) {
            handleTouch(event);
        };
    }
    
    void setupUI() {
        // Service discovery UI
        serviceDiscoveryUI = std::make_shared<ServiceDiscoveryUI>();
        serviceDiscoveryUI->setPosition(glm::vec3(0, 0, 0));
        serviceDiscoveryUI->onServiceSelected = [this](const ServiceInfo& info) {
            deployService(info);
        };
        scene->getRoot()->addChild(serviceDiscoveryUI);
        
        // Stats display
        createStatsDisplay();
        
        // Help panel
        createHelpPanel();
    }
    
    void deployService(const ServiceInfo& info) {
        std::cout << "Deploying service: " << info.name << std::endl;
        
        // Create visualization for the service
        auto serviceViz = ServiceVisualizationFactory::create(info);
        
        // Position it in the world
        glm::vec3 position = findAvailablePosition();
        serviceViz->setPosition(position);
        
        // Add to scene
        scene->getRoot()->addChild(serviceViz);
        activeServices[info.id] = serviceViz;
        
        // Connect to service for real-time metrics
        networkClient->subscribeToService(info.id);
        
        // Animate deployment
        animateServiceDeployment(serviceViz);
    }
    
    glm::vec3 findAvailablePosition() {
        // Simple grid layout for services
        static int gridX = 0;
        static int gridZ = 0;
        
        glm::vec3 position(gridX * 6.0f - 12.0f, 0, gridZ * 6.0f - 12.0f);
        
        gridX++;
        if (gridX > 4) {
            gridX = 0;
            gridZ++;
        }
        
        return position;
    }
    
    void animateServiceDeployment(std::shared_ptr<ServiceVisualization> service) {
        // Start with service scaled down
        service->setScale(glm::vec3(0.01f));
        
        // Create deployment effect
        auto deployEffect = std::make_shared<ParticleSystemNode>();
        deployEffect->setPosition(service->getPosition());
        deployEffect->burst(100);
        scene->getRoot()->addChild(deployEffect);
        
        // Animation will be handled in update loop
        deploymentAnimations[service] = 0.0f;
    }
    
    void updateScene(float deltaTime) {
        // Update scene graph
        scene->update(deltaTime);
        
        // Update camera controller
        cameraController->update(deltaTime);
        
        // Update deployment animations
        auto it = deploymentAnimations.begin();
        while (it != deploymentAnimations.end()) {
            it->second += deltaTime;
            float t = it->second / 1.0f; // 1 second animation
            
            if (t >= 1.0f) {
                it->first->setScale(glm::vec3(1.0f));
                it = deploymentAnimations.erase(it);
            } else {
                // Ease out cubic
                t = 1.0f - pow(1.0f - t, 3.0f);
                it->first->setScale(glm::vec3(t));
                ++it;
            }
        }
        
        // Update network activity visualization
        float networkActivity = networkClient->getNetworkActivity();
        environment->setNetworkActivity(networkActivity);
        
        // Update stats
        updateStats(deltaTime);
    }
    
    void updateSystemHealth() {
        float totalCPU = 0.0f;
        float totalMemory = 0.0f;
        int count = 0;
        
        for (const auto& [id, service] : activeServices) {
            auto metrics = networkClient->getServiceMetrics(id);
            totalCPU += metrics.cpuUsage;
            totalMemory += metrics.memoryUsage;
            count++;
        }
        
        if (count > 0) {
            float avgCPU = totalCPU / count;
            float avgMemory = totalMemory / count;
            float health = 1.0f - (avgCPU + avgMemory) / 200.0f;
            
            environment->updateSystemHealth(health);
        }
    }
    
    void createStatsDisplay() {
        statsPanel = std::make_shared<UI3DPanel>(3.0f, 2.0f);
        statsPanel->setPosition(glm::vec3(-8, 5, -5));
        statsPanel->setBackgroundColor(glm::vec4(0.0f, 0.0f, 0.1f, 0.7f));
        scene->getRoot()->addChild(statsPanel);
    }
    
    void updateStats(float deltaTime) {
        frameTime += deltaTime;
        frameCount++;
        
        if (frameTime >= 1.0f) {
            float fps = frameCount / frameTime;
            
            std::stringstream ss;
            ss << "FinalStorm v1.0\n"
               << "FPS: " << std::fixed << std::setprecision(1) << fps << "\n"
               << "Services: " << activeServices.size() << "\n"
               << "Network: " << networkClient->getConnectionStatus() << "\n";
            
            statsPanel->setText(ss.str());
            
            frameTime = 0.0f;
            frameCount = 0;
        }
    }
    
    void createHelpPanel() {
        helpPanel = std::make_shared<UI3DPanel>(4.0f, 3.0f);
        helpPanel->setPosition(glm::vec3(8, 5, -5));
        helpPanel->setBackgroundColor(glm::vec4(0.0f, 0.1f, 0.0f, 0.7f));
        helpPanel->setText(
            "Controls:\n"
            "WASD - Move\n"
            "QE - Up/Down\n"
            "Mouse - Look\n"
            "Scroll - Zoom\n"
            "TAB - Services\n"
            "F - Fullscreen\n"
            "ESC - Exit"
        );
        helpPanel->setVisible(false);
        scene->getRoot()->addChild(helpPanel);
    }
    
    void toggleServiceDiscovery() {
        static bool visible = true;
        visible = !visible;
        serviceDiscoveryUI->setVisible(visible);
    }
    
    void toggleFullscreen() {
        renderer->toggleFullscreen();
    }
    
    void handleTouch(const TouchEvent& event) {
        if (event.type == TouchEvent::TAP) {
            // Cast ray from touch position
            glm::vec2 screenPos(event.x, event.y);
            Ray ray = camera->screenPointToRay(screenPos);
            
            // Check for UI interaction
            if (auto hit = scene->raycast(ray)) {
                if (auto panel = std::dynamic_pointer_cast<UI3DPanel>(hit)) {
                    if (panel->onPress) {
                        panel->onPress();
                    }
                }
            }
        }
    }
    
    void shutdown() {
        std::cout << "Shutting down FinalStorm..." << std::endl;
        
        // Disconnect from network
        networkClient->disconnect();
        
        // Clean up resources
        scene.reset();
        renderer.reset();
        
        std::cout << "FinalStorm shutdown complete." << std::endl;
    }
    
private:
    // Core systems
    std::unique_ptr<MetalRenderer> renderer;
    std::unique_ptr<Scene> scene;
    std::shared_ptr<CameraNode> camera;
    std::unique_ptr<CameraController> cameraController;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<FinalverseNetworkClient> networkClient;
    
    // Environment
    std::shared_ptr<FinalverseEnvironment> environment;
    
    // UI elements
    std::shared_ptr<ServiceDiscoveryUI> serviceDiscoveryUI;
    std::shared_ptr<UI3DPanel> statsPanel;
    std::shared_ptr<UI3DPanel> helpPanel;
    
    // Services
    std::vector<ServiceInfo> availableServices;
    std::map<std::string, std::shared_ptr<ServiceVisualization>> activeServices;
    std::map<std::shared_ptr<ServiceVisualization>, float> deploymentAnimations;
    
    // State
    bool isRunning;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    float frameTime = 0.0f;
    int frameCount = 0;
};

// Camera controller implementation
class CameraController {
public:
    CameraController(std::shared_ptr<CameraNode> camera) : 
        camera(camera),
        distance(15.0f),
        pitch(0.3f),
        yaw(0.0f),
        targetPosition(0, 0, 0) {
        updateCamera();
    }
    
    void rotate(float deltaYaw, float deltaPitch) {
        yaw += deltaYaw;
        pitch += deltaPitch;
        pitch = glm::clamp(pitch, -1.5f, 1.5f);
        updateCamera();
    }
    
    void zoom(float delta) {
        distance *= (1.0f - delta);
        distance = glm::clamp(distance, 2.0f, 50.0f);
        updateCamera();
    }
    
    void moveForward(float amount) {
        glm::vec3 forward = camera->getRotation() * glm::vec3(0, 0, -1);
        forward.y = 0;
        forward = glm::normalize(forward);
        targetPosition += forward * amount;
        updateCamera();
    }
    
    void moveRight(float amount) {
        glm::vec3 right = camera->getRotation() * glm::vec3(1, 0, 0);
        right.y = 0;
        right = glm::normalize(right);
        targetPosition += right * amount;
        updateCamera();
    }
    
    void moveUp(float amount) {
        targetPosition.y += amount;
        updateCamera();
    }
    
    void update(float deltaTime) {
        // Smooth camera movement
        glm::vec3 currentPos = camera->getPosition();
        glm::vec3 desiredPos = calculateDesiredPosition();
        glm::vec3 newPos = glm::mix(currentPos, desiredPos, deltaTime * 5.0f);
        camera->setPosition(newPos);
        camera->lookAt(targetPosition);
    }
    
private:
    void updateCamera() {
        camera->setPosition(calculateDesiredPosition());
        camera->lookAt(targetPosition);
    }
    
    glm::vec3 calculateDesiredPosition() {
        float x = distance * cos(pitch) * sin(yaw);
        float y = distance * sin(pitch);
        float z = distance * cos(pitch) * cos(yaw);
        return targetPosition + glm::vec3(x, y, z);
    }
    
    std::shared_ptr<CameraNode> camera;
    float distance;
    float pitch;
    float yaw;
    glm::vec3 targetPosition;
};

// Service info structure
struct ServiceInfo {
    std::string id;
    std::string name;
    std::string type;
    std::string endpoint;
    std::string description;
    bool isActive;
};

// Service metrics structure
struct ServiceMetrics {
    float cpuUsage;
    float memoryUsage;
    float networkIn;
    float networkOut;
    int activeConnections;
    float responseTime;
    float uptime;
    std::map<std::string, float> customMetrics;
};

} // namespace FinalStorm

// Main entry point
int main(int argc, char* argv[]) {
    try {
        FinalStorm::FinalStormApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}