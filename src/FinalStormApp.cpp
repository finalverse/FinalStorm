// FinalStormApp.cpp
#include "FinalStormApp.h"
#include "Core/Networking/NetworkClient.h"
#include "Core/Input/InputManager.h"            // The new low-level input manager
#include "Core/Input/InteractionManager.h"      // Existing high-level interaction
#include "Renderer/MetalRenderer.h"             // Use existing renderer
#include "Services/ServiceVisualizations.h"     // Use existing visualizations
#include "UI/UI3DPanel.h"
#include <iostream>

namespace FinalStorm {

FinalStormApp::FinalStormApp() {
    // Initialize components
    m_renderer = std::make_unique<MetalRenderer>();
    m_networkClient = std::make_unique<NetworkClient>();
    m_inputManager = &InputManager::getInstance();
    m_interactionManager = std::make_unique<InteractionManager>();
    
    // Setup callbacks
    setupCallbacks();
}

FinalStormApp::~FinalStormApp() {
    shutdown();
}

bool FinalStormApp::initialize(void* nativeHandle, int width, int height) {
    std::cout << "Initializing FinalStorm..." << std::endl;
    
    // Initialize renderer
    if (!m_renderer->initialize(nativeHandle, width, height)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    // Create camera
    m_camera = std::make_shared<Camera>();
    m_camera->setPerspective(60.0f, float(width)/float(height), 0.1f, 1000.0f);
    m_camera->setPosition(vec3(0, 5, 10));
    m_camera->lookAt(vec3(0, 0, 0));
    
    // Set up interaction manager
    m_interactionManager->setCamera(m_camera);
    
    // Create scene
    createScene();
    
    // Connect to Finalverse server
    m_networkClient->connect("ws://localhost:3000/ws");
    
    m_initialized = true;
    return true;
}

void FinalStormApp::createScene() {
    // Create root scene node
    m_sceneRoot = std::make_shared<SceneNode>();
    
    // Create service discovery UI
    m_serviceDiscovery = std::make_shared<ServiceDiscoveryUI>();
    m_sceneRoot->addChild(m_serviceDiscovery);
    
    // Create environment
    m_environment = std::make_shared<FinalverseEnvironment>();
    m_sceneRoot->addChild(m_environment);
    
    // Set up interaction manager with scene
    m_interactionManager->setSceneRoot(m_sceneRoot);
    
    // Create UI panels
    auto infoPanel = std::make_shared<UI3DPanel>(4.0f, 2.0f);
    infoPanel->setPosition(vec3(0, 2, -5));
    infoPanel->setBackgroundColor(vec4(0.1f, 0.1f, 0.2f, 0.9f));
    m_uiPanels.push_back(infoPanel);
}

void FinalStormApp::setupCallbacks() {
    // Low-level input to high-level interaction
    m_inputManager->setMouseCallback([this](const vec2& pos, const vec2& delta) {
        vec2 viewport(m_renderer->getWidth(), m_renderer->getHeight());
        m_interactionManager->handleTouchMoved(pos, viewport);
    });
    
    m_inputManager->setKeyCallback([this](KeyCode key, bool pressed) {
        if (pressed) {
            handleKeyPress(key);
        }
    });
    
    // Network callbacks
    m_networkClient->setConnectionCallback([this](ConnectionState state) {
        std::cout << "Connection state changed: " << static_cast<int>(state) << std::endl;
        if (state == ConnectionState::Connected) {
            m_networkClient->requestServiceList();
        }
    });
    
    m_networkClient->setServiceCallback([this](const ServiceInfo& info) {
        std::cout << "Service update: " << info.name << std::endl;
        
        // Create or update service visualization
        auto serviceNode = ServiceVisualizationFactory::create(info);
        if (serviceNode) {
            m_sceneRoot->addChild(serviceNode);
            m_serviceNodes[info.id] = serviceNode;
        }
    });
    
    // Service discovery callbacks
    m_serviceDiscovery->onServiceSelected = [this](const ServiceNode::ServiceInfo& info) {
        std::cout << "Service selected: " << info.name << std::endl;
        // Focus camera on selected service
        if (auto it = m_serviceNodes.find(info.id); it != m_serviceNodes.end()) {
            focusOnService(it->second);
        }
    };
}

void FinalStormApp::handleKeyPress(KeyCode key) {
    switch (key) {
        case KeyCode::Escape:
            m_running = false;
            break;
        case KeyCode::Space:
            // Toggle service discovery
            if (m_serviceDiscovery) {
                m_serviceDiscovery->toggleExpanded();
            }
            break;
        case KeyCode::W:
            m_camera->move(vec3(0, 0, -1));
            break;
        case KeyCode::S:
            m_camera->move(vec3(0, 0, 1));
            break;
        case KeyCode::A:
            m_camera->move(vec3(-1, 0, 0));
            break;
        case KeyCode::D:
            m_camera->move(vec3(1, 0, 0));
            break;
        default:
            break;
    }
}

void FinalStormApp::update(float deltaTime) {
    if (!m_initialized) return;
    
    // Update network
    m_networkClient->update();
    
    // Update scene
    if (m_sceneRoot) {
        m_sceneRoot->update(deltaTime);
    }
    
    // Update UI
    for (auto& panel : m_uiPanels) {
        panel->update(deltaTime);
    }
    
    // Update environment based on system health
    updateSystemHealth();
}

void FinalStormApp::render() {
    if (!m_initialized) return;
    
    m_renderer->beginFrame();
    m_renderer->clearRenderTarget(vec4(0.05f, 0.05f, 0.1f, 1.0f));
    
    // Set camera
    m_renderer->setViewMatrix(m_camera->getViewMatrix());
    m_renderer->setProjectionMatrix(m_camera->getProjectionMatrix());
    
    // Render scene
    if (m_sceneRoot) {
        renderNode(m_sceneRoot.get());
    }
    
    // Render UI panels
    for (auto& panel : m_uiPanels) {
        panel->render(m_renderer.get());
    }
    
    m_renderer->endFrame();
    m_renderer->present();
}

void FinalStormApp::renderNode(SceneNode* node) {
    if (!node || !node->isVisible()) return;
    
    // Set transform
    m_renderer->setModelMatrix(node->getWorldTransform());
    
    // Render based on node type
    if (auto meshNode = dynamic_cast<MeshNode*>(node)) {
        m_renderer->drawMesh(meshNode->getMesh());
    } else if (auto serviceViz = dynamic_cast<ServiceVisualization*>(node)) {
        // Service visualizations handle their own rendering
        serviceViz->render(m_renderer.get());
    }
    
    // Render children
    for (const auto& child : node->getChildren()) {
        renderNode(child.get());
    }
}

void FinalStormApp::focusOnService(std::shared_ptr<ServiceVisualization> service) {
    if (!service) return;
    
    // Animate camera to focus on service
    vec3 servicePos = service->getTransform().position;
    vec3 cameraTarget = servicePos + vec3(0, 2, 5);
    
    // Simple immediate positioning for now
    // TODO: Add smooth camera animation
    m_camera->setPosition(cameraTarget);
    m_camera->lookAt(servicePos);
}

void FinalStormApp::updateSystemHealth() {
    // Calculate overall system health from all services
    float totalHealth = 0.0f;
    int serviceCount = 0;
    
    for (const auto& [id, service] : m_serviceNodes) {
        if (auto serviceViz = std::dynamic_pointer_cast<ServiceVisualization>(service)) {
            // Get health from service metrics
            totalHealth += serviceViz->getActivityLevel();
            serviceCount++;
        }
    }
    
    float avgHealth = serviceCount > 0 ? totalHealth / serviceCount : 1.0f;
    
    // Update environment
    if (m_environment) {
        m_environment->updateSystemHealth(avgHealth);
        m_environment->setNetworkActivity(avgHealth);
    }
}

void FinalStormApp::shutdown() {
    if (!m_initialized) return;
    
    std::cout << "Shutting down FinalStorm..." << std::endl;
    
    // Disconnect from server
    m_networkClient->disconnect();
    
    // Shutdown renderer
    m_renderer->shutdown();
    
    m_initialized = false;
}

} // namespace FinalStorm