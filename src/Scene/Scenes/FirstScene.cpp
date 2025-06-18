// FirstScene.cpp - Implementation of The Nexus first scene

#include "FirstScene.h"
#include "Services/ServiceFactory.h"
#include "UI/ServiceDiscoveryUI.h"
#include "Scene/CameraController.h"
#include "Rendering/Metal/MetalRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace FinalStorm {

// FirstScene Implementation
FirstScene::FirstScene() : Scene("The Nexus") {
    m_isFirstVisit = true; // TODO: Load from user preferences
}

FirstScene::~FirstScene() = default;

void FirstScene::initialize() {
    std::cout << "Initializing The Nexus scene..." << std::endl;
    
    // Create the environment
    createEnvironment();
    
    // Create central nexus
    m_nexus = std::make_unique<CentralNexus>();
    m_nexus->setPosition(glm::vec3(0, 0, 0));
    addNode(m_nexus);
    
    // Create service ring
    m_serviceRing = std::make_unique<ServiceRing>();
    m_serviceRing->setPosition(glm::vec3(0, 0, 0));
    addNode(m_serviceRing);
    
    // Create services
    createServices();
    
    // Setup lighting
    setupLighting();
    
    // Create UI elements
    createUIElements();
    
    // Setup camera path for intro
    setupCameraPath();
    
    // Create welcome sequence if first visit
    if (m_isFirstVisit) {
        m_welcomeSequence = std::make_unique<WelcomeSequence>();
        m_welcomeSequence->onComplete = [this]() {
            skipToMainView();
        };
        addNode(m_welcomeSequence);
    }
}

void FirstScene::onEnter() {
    std::cout << "Entering The Nexus..." << std::endl;
    
    if (m_isFirstVisit && m_welcomeSequence) {
        showWelcomeSequence();
    } else {
        skipToMainView();
    }
    
    // Start ambient animations
    m_nexus->pulse(0.8f);
}

void FirstScene::onExit() {
    std::cout << "Leaving The Nexus..." << std::endl;
    
    // Save user preferences
    m_isFirstVisit = false;
}

void FirstScene::update(float deltaTime) {
    Scene::update(deltaTime);
    
    m_sceneTime += deltaTime;
    
    // Update service connections based on activity
    if (static_cast<int>(m_sceneTime) % 5 == 0) {
        // Pulse random connections to show activity
        if (!m_services.empty()) {
            int idx1 = rand() % m_services.size();
            int idx2 = rand() % m_services.size();
            if (idx1 != idx2) {
                // Simulate data flow between services
                m_nexus->pulse(0.3f);
            }
        }
    }
}

void FirstScene::onServiceSelected(ServiceEntity* service) {
    if (!service) return;
    
    m_selectedService = service;
    
    // Highlight the service
    service->highlight(true);
    
    // Rotate ring to focus on service
    m_serviceRing->rotateToService(service, 1.0f);
    
    // Move camera to focus on service
    auto camera = getActiveCamera();
    if (camera) {
        glm::vec3 servicePos = service->getWorldPosition();
        glm::vec3 cameraPos = servicePos + glm::vec3(0, 2, 5);
        camera->transitionTo(cameraPos, servicePos, 1.5f);
    }
    
    // Show service details
    // TODO: Show detailed service panel
}

void FirstScene::onServiceHovered(ServiceEntity* service) {
    if (!service) return;
    
    // Gentle highlight
    service->setGlowIntensity(0.7f);
    
    // Show quick info
    // TODO: Show tooltip
}

void FirstScene::showWelcomeSequence() {
    if (m_welcomeSequence) {
        m_welcomeSequence->start();
        
        // Start camera animation
        auto camera = getActiveCamera();
        if (camera && !m_introSequence.empty()) {
            // Animate through intro keyframes
            float totalTime = 0;
            for (const auto& keyframe : m_introSequence) {
                camera->addKeyframe(keyframe.position, keyframe.lookAt, 
                                  keyframe.fov, totalTime);
                totalTime += keyframe.duration;
            }
            camera->playAnimation();
        }
    }
}

void FirstScene::skipToMainView() {
    // Set camera to default view
    auto camera = getActiveCamera();
    if (camera) {
        glm::vec3 defaultPos(15, 10, 15);
        glm::vec3 defaultLookAt(0, 0, 0);
        camera->transitionTo(defaultPos, defaultLookAt, 2.0f, 60.0f);
    }
    
    // Expand service ring
    m_serviceRing->expandRing(12.0f, 2.0f);
    
    // Start ambient animations
    m_nexus->setConnectionStrength(1.0f);
}

void FirstScene::toggleServiceDiscovery() {
    // TODO: Implement service discovery UI toggle
}

void FirstScene::createEnvironment() {
    // Create nebula environment
    auto nebula = std::make_shared<NebulaEnvironment>();
    nebula->setColorScheme(
        glm::vec3(0.1f, 0.2f, 0.4f),  // Deep blue primary
        glm::vec3(0.4f, 0.1f, 0.3f)   // Purple secondary
    );
    nebula->setDensity(0.7f);
    addNode(nebula);
    
    // Create ambient particle effects
    auto ambientParticles = std::make_shared<ParticleEmitter>();
    ambientParticles->setEmitShape(ParticleEmitter::Shape::SPHERE);
    ambientParticles->setEmitRadius(50.0f);
    ambientParticles->setParticleLifetime(20.0f);
    ambientParticles->setEmitRate(2.0f);
    ambientParticles->setParticleColor(glm::vec4(0.3f, 0.5f, 1.0f, 0.3f));
    ambientParticles->setGravity(glm::vec3(0, 0.01f, 0));
    addNode(ambientParticles);
}

void FirstScene::createServices() {
    // Create all Finalverse services
    struct ServiceConfig {
        ServiceEntity::ServiceType type;
        const char* name;
        glm::vec3 customColor;
    };
    
    ServiceConfig services[] = {
        {ServiceEntity::ServiceType::API_GATEWAY, "API Gateway", glm::vec3(0.0f, 0.8f, 1.0f)},
        {ServiceEntity::ServiceType::AI_ORCHESTRA, "AI Orchestra", glm::vec3(0.8f, 0.0f, 0.8f)},
        {ServiceEntity::ServiceType::SONG_ENGINE, "Song Engine", glm::vec3(1.0f, 0.5f, 0.0f)},
        {ServiceEntity::ServiceType::ECHO_ENGINE, "Echo Engine", glm::vec3(0.0f, 1.0f, 0.5f)},
        {ServiceEntity::ServiceType::WORLD_ENGINE, "World Engine", glm::vec3(0.2f, 0.8f, 0.2f)},
        {ServiceEntity::ServiceType::HARMONY_SERVICE, "Harmony", glm::vec3(1.0f, 0.3f, 0.3f)},
        {ServiceEntity::ServiceType::ASSET_SERVICE, "Assets", glm::vec3(0.5f, 0.5f, 1.0f)},
        {ServiceEntity::ServiceType::COMMUNITY, "Community", glm::vec3(1.0f, 0.8f, 0.0f)},
        {ServiceEntity::ServiceType::SILENCE_SERVICE, "Silence", glm::vec3(0.3f, 0.3f, 0.6f)},
        {ServiceEntity::ServiceType::PROCEDURAL_GEN, "Procedural", glm::vec3(0.6f, 0.2f, 0.8f)},
        {ServiceEntity::ServiceType::BEHAVIOR_AI, "Behavior AI", glm::vec3(0.9f, 0.1f, 0.5f)}
    };
    
    for (const auto& config : services) {
        auto service = ServiceFactory::createService(config.type);
        service->setName(config.name);
        service->setBaseColor(config.customColor);
        
        m_services.push_back(service);
        m_serviceRing->addService(service);
    }
}

void FirstScene::createUIElements() {
    // Create interactive guide
    auto guide = std::make_shared<InteractiveGuide>();
    guide->setPosition(glm::vec3(5, 5, 5));
    guide->showTip("Welcome to The Nexus - Your gateway to the Finalverse", 10.0f);
    addNode(guide);
    
    // Create service connection visualizer
    auto connectionViz = std::make_shared<ServiceConnectionVisualizer>();
    addNode(connectionViz);
    
    // Add some default connections
    if (m_services.size() >= 2) {
        // API Gateway connects to all services
        auto apiGateway = m_services[0].get();
        for (size_t i = 1; i < m_services.size(); ++i) {
            connectionViz->addConnection(apiGateway, m_services[i].get(), 0.5f);
        }
    }
}

void FirstScene::setupLighting() {
    // Main directional light (sun)
    auto sunLight = std::make_shared<DirectionalLight>();
    sunLight->setDirection(glm::vec3(-0.5f, -0.7f, -0.5f));
    sunLight->setColor(glm::vec3(0.9f, 0.9f, 0.8f));
    sunLight->setIntensity(0.6f);
    addNode(sunLight);
    
    // Nexus point light
    auto nexusLight = std::make_shared<PointLight>();
    nexusLight->setPosition(glm::vec3(0, 2, 0));
    nexusLight->setColor(glm::vec3(0.0f, 0.8f, 1.0f));
    nexusLight->setIntensity(2.0f);
    nexusLight->setRadius(20.0f);
    addNode(nexusLight);
    
    // Ambient fill lights
    for (int i = 0; i < 4; ++i) {
        auto fillLight = std::make_shared<PointLight>();
        float angle = (i / 4.0f) * 2.0f * M_PI;
        fillLight->setPosition(glm::vec3(
            cos(angle) * 15.0f,
            5.0f,
            sin(angle) * 15.0f
        ));
        fillLight->setColor(glm::vec3(0.3f, 0.4f, 0.5f));
        fillLight->setIntensity(0.5f);
        fillLight->setRadius(30.0f);
        addNode(fillLight);
    }
}

void FirstScene::setupCameraPath() {
    // Define intro camera animation keyframes
    m_introSequence = {
        // Start far away
        {glm::vec3(0, 50, 100), glm::vec3(0, 0, 0), 60.0f, 3.0f},
        
        // Sweep around the scene
        {glm::vec3(50, 30, 50), glm::vec3(0, 0, 0), 60.0f, 3.0f},
        
        // Move through the service ring
        {glm::vec3(20, 5, 0), glm::vec3(0, 0, 0), 70.0f, 2.0f},
        
        // Close up on nexus
        {glm::vec3(0, 5, 10), glm::vec3(0, 0, 0), 50.0f, 2.0f},
        
        // Pull back to default view
        {glm::vec3(15, 10, 15), glm::vec3(0, 0, 0), 60.0f, 2.0f}
    };
}

// CentralNexus Implementation
CentralNexus::CentralNexus() : SceneNode("Central Nexus") {
    createCoreStructure();
    createEnergyRings();
}

void CentralNexus::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    // Rotate core structure
    m_rotationPhase += deltaTime * 0.3f;
    if (m_coreStructure) {
        m_coreStructure->setRotation(
            glm::angleAxis(m_rotationPhase, glm::vec3(0, 1, 0)) *
            glm::angleAxis(m_rotationPhase * 0.7f, glm::vec3(1, 0, 0))
        );
    }
    
    // Update rings
    if (m_innerRings) {
        m_innerRings->setRotation(
            glm::angleAxis(-m_rotationPhase * 1.5f, glm::vec3(0, 1, 0))
        );
    }
    
    // Pulse effect
    if (m_pulseIntensity > 0) {
        m_pulsePhase += deltaTime * 5.0f;
        m_pulseIntensity *= 0.95f; // Decay
        
        float scale = 1.0f + m_pulseIntensity * 0.2f * sin(m_pulsePhase);
        if (m_outerShell) {
            m_outerShell->setScale(glm::vec3(scale));
        }
    }
    
    // Update energy flow
    updateEnergyFlow(deltaTime);
}

void CentralNexus::render(MetalRenderer* renderer) {
    // Set emissive properties for glow effect
    renderer->pushMaterial();
    renderer->setEmission(m_coreColor * (0.5f + m_pulseIntensity));
    
    SceneNode::render(renderer);
    
    renderer->popMaterial();
}

void CentralNexus::pulse(float intensity) {
    m_pulseIntensity = glm::clamp(intensity, 0.0f, 1.0f);
    m_pulsePhase = 0.0f;
    
    // Burst particles
    if (m_energyFlow) {
        m_energyFlow->burst(20);
    }
}

void CentralNexus::setConnectionStrength(float strength) {
    m_connectionStrength = glm::clamp(strength, 0.0f, 1.0f);
    
    // Update particle emission rate
    if (m_energyFlow) {
        m_energyFlow->setEmitRate(5.0f + strength * 45.0f);
    }
}

void CentralNexus::createCoreStructure() {
    // Central crystalline core
    m_coreStructure = std::make_shared<MeshNode>();
    m_coreStructure->setMesh(MeshLibrary::OCTAHEDRON);
    m_coreStructure->setScale(glm::vec3(1.5f));
    m_coreStructure->setMaterial(MaterialLibrary::CRYSTAL);
    addChild(m_coreStructure);
    
    // Inner rotating rings
    m_innerRings = std::make_shared<SceneNode>("Inner Rings");
    for (int i = 0; i < 3; ++i) {
        auto ring = std::make_shared<MeshNode>();
        ring->setMesh(MeshLibrary::TORUS);
        ring->setScale(glm::vec3(2.0f + i * 0.5f, 0.1f, 2.0f + i * 0.5f));
        ring->setRotation(glm::angleAxis(
            glm::radians(30.0f * i), 
            glm::vec3(1, 0, 0)
        ));
        ring->setMaterial(MaterialLibrary::EMISSIVE);
        m_innerRings->addChild(ring);
    }
    addChild(m_innerRings);
    
    // Outer protective shell (transparent)
    m_outerShell = std::make_shared<MeshNode>();
    m_outerShell->setMesh(MeshLibrary::ICOSAHEDRON);
    m_outerShell->setScale(glm::vec3(3.0f));
    m_outerShell->setMaterial(MaterialLibrary::GLASS);
    m_outerShell->setAlpha(0.3f);
    addChild(m_outerShell);
}

void CentralNexus::createEnergyRings() {
    // Create energy flow particles
    ParticleEmitter::Config config;
    config.emitShape = ParticleEmitter::Shape::SPHERE;
    config.emitRadius = 1.0f;
    config.emitRate = 20.0f;
    config.particleLifetime = 2.0f;
    config.startSize = 0.1f;
    config.endSize = 0.02f;
    config.startColor = glm::vec4(m_energyColor, 1.0f);
    config.endColor = glm::vec4(m_energyColor, 0.0f);
    config.velocity = 2.0f;
    config.gravity = glm::vec3(0, 0.5f, 0);
    
    m_energyFlow = std::make_shared<ParticleEmitter>(config);
    addChild(m_energyFlow);
}

void CentralNexus::updateEnergyFlow(float deltaTime) {
    // Create orbital energy streams
    static float orbitPhase = 0.0f;
    orbitPhase += deltaTime;
    
    // Update particle emission position to create streams
    if (m_energyFlow) {
        float radius = 2.0f;
        glm::vec3 emitPos(
            cos(orbitPhase * 2.0f) * radius,
            sin(orbitPhase * 3.0f) * 0.5f,
            sin(orbitPhase * 2.0f) * radius
        );
        m_energyFlow->setEmitPosition(emitPos);
    }
}

// ServiceRing Implementation
ServiceRing::ServiceRing() : SceneNode("Service Ring") {
    m_radius = 10.0f;
    m_targetRadius = 10.0f;
}

void ServiceRing::addService(std::shared_ptr<ServiceEntity> service) {
    m_services.push_back(service);
    addChild(service);
    updateServicePositions(0.0f);
}

void ServiceRing::removeService(ServiceEntity* service) {
    auto it = std::find_if(m_services.begin(), m_services.end(),
        [service](const std::shared_ptr<ServiceEntity>& s) {
            return s.get() == service;
        });
    
    if (it != m_services.end()) {
        removeChild(*it);
        m_services.erase(it);
        updateServicePositions(0.0f);
    }
}

void ServiceRing::clear() {
    for (auto& service : m_services) {
        removeChild(service);
    }
    m_services.clear();
}

void ServiceRing::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    // Update ring expansion/contraction
    if (m_expansionTime < m_expansionDuration) {
        m_expansionTime += deltaTime;
        float t = m_expansionTime / m_expansionDuration;
        t = t * t * (3.0f - 2.0f * t); // Smooth step
        
        m_radius = glm::mix(m_radius, m_targetRadius, t);
    }
    
    // Rotate the ring slowly
    m_rotationOffset += deltaTime * m_rotationSpeed;
    
    // Update service positions
    updateServicePositions(deltaTime);
}

void ServiceRing::expandRing(float targetRadius, float duration) {
    m_targetRadius = targetRadius;
    m_expansionTime = 0.0f;
    m_expansionDuration = duration;
}

void ServiceRing::contractRing(float targetRadius, float duration) {
    expandRing(targetRadius, duration); // Same mechanism
}

void ServiceRing::highlightService(ServiceEntity* service) {
    m_highlightedService = service;
    
    // Dim other services
    for (auto& s : m_services) {
        if (s.get() != service) {
            s->setAlpha(0.6f);
        } else {
            s->setAlpha(1.0f);
            s->highlight(true);
        }
    }
}

void ServiceRing::rotateToService(ServiceEntity* service, float duration) {
    // Find service index
    auto it = std::find_if(m_services.begin(), m_services.end(),
        [service](const std::shared_ptr<ServiceEntity>& s) {
            return s.get() == service;
        });
    
    if (it != m_services.end()) {
        int index = std::distance(m_services.begin(), it);
        float targetAngle = -(index / float(m_services.size())) * 2.0f * M_PI;
        
        // TODO: Animate rotation to target angle
        m_rotationOffset = targetAngle;
    }
}

void ServiceRing::updateServicePositions(float deltaTime) {
    int count = m_services.size();
    if (count == 0) return;
    
    for (int i = 0; i < count; ++i) {
        glm::vec3 targetPos = getServicePosition(i, m_radius);
        
        // Smooth position update
        auto& service = m_services[i];
        glm::vec3 currentPos = service->getPosition();
        glm::vec3 newPos = glm::mix(currentPos, targetPos, deltaTime * 5.0f);
        service->setPosition(newPos);
        
        // Make services face outward
        glm::vec3 lookDir = glm::normalize(newPos);
        service->setRotation(glm::quatLookAt(lookDir, glm::vec3(0, 1, 0)));
        
        // Add floating animation
        float floatOffset = sin(m_rotationOffset * 2.0f + i * 0.5f) * 0.2f;
        service->setPosition(newPos + glm::vec3(0, floatOffset, 0));
    }
}

glm::vec3 ServiceRing::getServicePosition(int index, float radius) {
    float angle = (index / float(m_services.size())) * 2.0f * M_PI + m_rotationOffset;
    float height = sin(index * 0.8f) * m_verticalSpread;
    
    return glm::vec3(
        cos(angle) * radius,
        height,
        sin(angle) * radius
    );
}

// WelcomeSequence Implementation
WelcomeSequence::WelcomeSequence() : SceneNode("Welcome Sequence") {
    createUIElements();
}

void WelcomeSequence::start() {
    m_currentPhase = Phase::FADE_IN;
    m_phaseTime = 0.0f;
    m_isComplete = false;
    m_fadeAlpha = 1.0f;
    
    setVisible(true);
}

void WelcomeSequence::skip() {
    m_isComplete = true;
    setVisible(false);
    
    if (onComplete) {
        onComplete();
    }
}

void WelcomeSequence::update(float deltaTime) {
    if (m_isComplete) return;
    
    SceneNode::update(deltaTime);
    updatePhase(deltaTime);
}

void WelcomeSequence::render(MetalRenderer* renderer) {
    if (!isVisible()) return;
    
    // Render fade overlay
    if (m_fadeAlpha > 0.01f) {
        renderer->renderFullscreenQuad(glm::vec4(0, 0, 0, m_fadeAlpha));
    }
    
    SceneNode::render(renderer);
}

void WelcomeSequence::updatePhase(float deltaTime) {
    m_phaseTime += deltaTime;
    
    switch (m_currentPhase) {
        case Phase::FADE_IN:
            m_fadeAlpha = 1.0f - (m_phaseTime / 2.0f);
            if (m_phaseTime > 2.0f) {
                transitionToPhase(Phase::TITLE_REVEAL);
            }
            break;
            
        case Phase::TITLE_REVEAL:
            if (m_titlePanel) {
                float alpha = glm::min(m_phaseTime / 1.0f, 1.0f);
                m_titlePanel->setAlpha(alpha);
            }
            if (m_phaseTime > 3.0f) {
                transitionToPhase(Phase::NEXUS_FORMATION);
            }
            break;
            
        case Phase::NEXUS_FORMATION:
            if (m_subtitlePanel) {
                float alpha = glm::min(m_phaseTime / 1.0f, 1.0f);
                m_subtitlePanel->setAlpha(alpha);
            }
            if (m_phaseTime > 3.0f) {
                transitionToPhase(Phase::SERVICE_INTRODUCTION);
            }
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            if (m_instructionPanel) {
                float alpha = glm::min(m_phaseTime / 1.0f, 1.0f);
                m_instructionPanel->setAlpha(alpha);
            }
            if (m_phaseTime > 4.0f) {
                transitionToPhase(Phase::READY);
            }
            break;
            
        case Phase::READY:
            // Fade out all UI
            float fadeOut = 1.0f - (m_phaseTime / 2.0f);
            if (m_titlePanel) m_titlePanel->setAlpha(fadeOut);
            if (m_subtitlePanel) m_subtitlePanel->setAlpha(fadeOut);
            if (m_instructionPanel) m_instructionPanel->setAlpha(fadeOut);
            
            if (m_phaseTime > 2.0f) {
                skip();
            }
            break;
    }
}

void WelcomeSequence::transitionToPhase(Phase newPhase) {
    m_currentPhase = newPhase;
        m_currentPhase = newPhase;
    m_phaseTime = 0.0f;
    
    // Trigger effects for new phase
    switch (newPhase) {
        case Phase::NEXUS_FORMATION:
            // Burst of particles
            if (m_sparkles) {
                m_sparkles->burst(100);
            }
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            // Start continuous sparkles
            if (m_sparkles) {
                m_sparkles->setEmitRate(20.0f);
            }
            break;
            
        default:
            break;
    }
}

void WelcomeSequence::createUIElements() {
    // Title panel
    m_titlePanel = std::make_shared<UI3DPanel>(8.0f, 2.0f);
    m_titlePanel->setText("WELCOME TO THE NEXUS");
    m_titlePanel->setBackgroundColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_titlePanel->setBorderColor(glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));
    m_titlePanel->setPosition(glm::vec3(0, 5, -10));
    m_titlePanel->setAlpha(0.0f);
    addChild(m_titlePanel);
    
    // Subtitle panel
    m_subtitlePanel = std::make_shared<UI3DPanel>(6.0f, 1.0f);
    m_subtitlePanel->setText("Your Gateway to the Finalverse");
    m_subtitlePanel->setBackgroundColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_subtitlePanel->setBorderColor(glm::vec4(0.5f, 0.8f, 1.0f, 0.8f));
    m_subtitlePanel->setPosition(glm::vec3(0, 3, -10));
    m_subtitlePanel->setAlpha(0.0f);
    addChild(m_subtitlePanel);
    
    // Instruction panel
    m_instructionPanel = std::make_shared<UI3DPanel>(4.0f, 0.8f);
    m_instructionPanel->setText("Click any service to explore");
    m_instructionPanel->setBackgroundColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
    m_instructionPanel->setBorderColor(glm::vec4(0.3f, 0.8f, 1.0f, 0.6f));
    m_instructionPanel->setPosition(glm::vec3(0, -3, -10));
    m_instructionPanel->setAlpha(0.0f);
    addChild(m_instructionPanel);
    
    // Sparkle particles
    ParticleEmitter::Config config;
    config.emitShape = ParticleEmitter::Shape::BOX;
    config.emitSize = glm::vec3(10, 5, 1);
    config.emitRate = 0.0f; // Start with burst only
    config.particleLifetime = 3.0f;
    config.startSize = 0.05f;
    config.endSize = 0.01f;
    config.startColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    config.endColor = glm::vec4(0.8f, 0.9f, 1.0f, 0.0f);
    config.velocity = 0.5f;
    config.gravity = glm::vec3(0, -0.1f, 0);
    
    m_sparkles = std::make_shared<ParticleEmitter>(config);
    m_sparkles->setPosition(glm::vec3(0, 0, -10));
    addChild(m_sparkles);
}

// NebulaEnvironment Implementation
NebulaEnvironment::NebulaEnvironment() : EnvironmentController() {
    setName("Nebula Environment");
    createNebulaLayers();
    
    // Initialize layer speeds
    m_layerSpeeds = {0.05f, -0.03f, 0.08f, -0.06f, 0.04f};
}

void NebulaEnvironment::update(float deltaTime) {
    EnvironmentController::update(deltaTime);
    updateFlow(deltaTime);
}

void NebulaEnvironment::render(MetalRenderer* renderer) {
    // Render with additive blending for nebula effect
    renderer->pushBlendMode(BlendMode::ADDITIVE);
    
    EnvironmentController::render(renderer);
    
    renderer->popBlendMode();
}

void NebulaEnvironment::setColorScheme(const glm::vec3& primary, const glm::vec3& secondary) {
    m_primaryColor = primary;
    m_secondaryColor = secondary;
    
    // Update layer colors
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        float t = i / float(m_nebulaLayers.size() - 1);
        glm::vec3 color = glm::mix(m_primaryColor, m_secondaryColor, t);
        // Set material color for each layer
    }
}

void NebulaEnvironment::setDensity(float density) {
    m_density = glm::clamp(density, 0.0f, 1.0f);
    
    // Update particle emission rates
    if (m_stars) {
        m_stars->setEmitRate(50.0f * m_density);
    }
    if (m_dust) {
        m_dust->setEmitRate(20.0f * m_density);
    }
}

void NebulaEnvironment::addStarburst(const glm::vec3& position, float intensity) {
    // Create a temporary particle burst
    auto burst = std::make_shared<ParticleEmitter>();
    burst->setPosition(position);
    burst->burst(int(intensity * 100));
    burst->setParticleLifetime(2.0f);
    burst->setParticleColor(glm::vec4(1.0f, 0.9f, 0.8f, 1.0f));
    addChild(burst);
    
    // Remove after animation
    // TODO: Add timed removal system
}

void NebulaEnvironment::createNebulaLayers() {
    // Create multiple nebula cloud layers
    for (int i = 0; i < 5; ++i) {
        auto layer = std::make_shared<MeshNode>();
        layer->setMesh(MeshLibrary::SPHERE);
        layer->setScale(glm::vec3(100.0f + i * 20.0f));
        layer->setMaterial(MaterialLibrary::NEBULA);
        layer->setAlpha(0.1f * m_density);
        
        // Offset layers for depth
        layer->setPosition(glm::vec3(
            sin(i * 1.2f) * 10.0f,
            cos(i * 0.8f) * 5.0f,
            i * -5.0f
        ));
        
        m_nebulaLayers.push_back(layer);
        addChild(layer);
    }
    
    // Star field
    ParticleEmitter::Config starConfig;
    starConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    starConfig.emitRadius = 200.0f;
    starConfig.emitRate = 50.0f;
    starConfig.particleLifetime = 1000.0f; // Very long life
    starConfig.startSize = 0.02f;
    starConfig.endSize = 0.02f;
    starConfig.startColor = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
    starConfig.endColor = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
    starConfig.velocity = 0.0f; // Static stars
    
    m_stars = std::make_shared<ParticleEmitter>(starConfig);
    addChild(m_stars);
    
    // Dust particles
    ParticleEmitter::Config dustConfig;
    dustConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    dustConfig.emitRadius = 50.0f;
    dustConfig.emitRate = 20.0f;
    dustConfig.particleLifetime = 30.0f;
    dustConfig.startSize = 0.5f;
    dustConfig.endSize = 0.8f;
    dustConfig.startColor = glm::vec4(m_primaryColor, 0.05f);
    dustConfig.endColor = glm::vec4(m_secondaryColor, 0.02f);
    dustConfig.velocity = 0.2f;
    dustConfig.gravity = glm::vec3(0, 0.01f, 0);
    
    m_dust = std::make_shared<ParticleEmitter>(dustConfig);
    addChild(m_dust);
}

void NebulaEnvironment::updateFlow(float deltaTime) {
    m_flowPhase += deltaTime * 0.1f;
    
    // Animate nebula layers
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        auto& layer = m_nebulaLayers[i];
        
        // Rotate each layer at different speed
        layer->rotate(glm::angleAxis(
            m_layerSpeeds[i] * deltaTime,
            glm::vec3(0, 1, 0)
        ));
        
        // Subtle scale pulsing
        float scale = 100.0f + i * 20.0f;
        scale += sin(m_flowPhase + i) * 2.0f;
        layer->setScale(glm::vec3(scale));
    }
}

// InteractiveGuide Implementation
InteractiveGuide::InteractiveGuide() : SceneNode("Interactive Guide") {
    // Create guide visual
    m_core = std::make_shared<MeshNode>();
    m_core->setMesh(MeshLibrary::ICOSAHEDRON);
    m_core->setScale(glm::vec3(0.3f));
    m_core->setMaterial(MaterialLibrary::EMISSIVE);
    addChild(m_core);
    
    // Text panel
    m_textPanel = std::make_shared<UI3DPanel>(3.0f, 1.0f);
    m_textPanel->setPosition(glm::vec3(0, 0.5f, 0));
    m_textPanel->setBackgroundColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));
    m_textPanel->setBorderColor(glm::vec4(0.5f, 0.8f, 1.0f, 1.0f));
    m_textPanel->setVisible(false);
    addChild(m_textPanel);
    
    // Aura particles
    ParticleEmitter::Config config;
    config.emitShape = ParticleEmitter::Shape::POINT;
    config.emitRate = 10.0f;
    config.particleLifetime = 1.0f;
    config.startSize = 0.05f;
    config.endSize = 0.1f;
    config.startColor = glm::vec4(0.5f, 0.8f, 1.0f, 0.8f);
    config.endColor = glm::vec4(0.5f, 0.8f, 1.0f, 0.0f);
    config.velocity = 0.3f;
    
    m_aura = std::make_shared<ParticleEmitter>(config);
    addChild(m_aura);
}

void InteractiveGuide::showTip(const std::string& text, float duration) {
    if (m_textPanel) {
        m_textPanel->setText(text);
        m_textPanel->setVisible(true);
        m_textDisplayTime = 0.0f;
        m_textDuration = duration;
    }
}

void InteractiveGuide::showServiceInfo(ServiceEntity* service) {
    if (!service) return;
    
    std::string info = service->getName() + "\n";
    info += "Status: " + std::string(service->isConnected() ? "Online" : "Offline");
    
    showTip(info, 5.0f);
}

void InteractiveGuide::hide() {
    m_isVisible = false;
    setVisible(false);
}

void InteractiveGuide::update(float deltaTime) {
    if (!m_isVisible) return;
    
    SceneNode::update(deltaTime);
    
    updateMovement(deltaTime);
    updateBobbing(deltaTime);
    
    // Update text display
    if (m_textPanel && m_textPanel->isVisible()) {
        m_textDisplayTime += deltaTime;
        if (m_textDisplayTime > m_textDuration) {
            m_textPanel->setVisible(false);
        }
    }
    
    // Rotate core
    if (m_core) {
        m_core->rotate(glm::angleAxis(deltaTime, glm::vec3(0, 1, 0)));
    }
}

void InteractiveGuide::render(MetalRenderer* renderer) {
    if (!m_isVisible) return;
    
    SceneNode::render(renderer);
}

void InteractiveGuide::moveToPosition(const glm::vec3& target, float duration) {
    m_targetPosition = target;
    m_moveTime = 0.0f;
    m_moveDuration = duration;
}

void InteractiveGuide::orbitAround(const glm::vec3& center, float radius) {
    // TODO: Implement orbital movement
}

void InteractiveGuide::updateMovement(float deltaTime) {
    if (m_moveTime < m_moveDuration) {
        m_moveTime += deltaTime;
        float t = m_moveTime / m_moveDuration;
        t = t * t * (3.0f - 2.0f * t); // Smooth step
        
        glm::vec3 currentPos = getPosition();
        glm::vec3 newPos = glm::mix(currentPos, m_targetPosition, t);
        setPosition(newPos);
    }
}

void InteractiveGuide::updateBobbing(float deltaTime) {
    m_bobPhase += deltaTime * 2.0f;
    
    glm::vec3 pos = getPosition();
    pos.y += sin(m_bobPhase) * 0.1f * deltaTime;
    setPosition(pos);
}

// ServiceConnectionVisualizer Implementation
ServiceConnectionVisualizer::ServiceConnectionVisualizer() 
    : SceneNode("Service Connections") {
    
    // Create data flow particles
    ParticleEmitter::Config config;
    config.emitShape = ParticleEmitter::Shape::LINE;
    config.emitRate = 0.0f; // Manual control
    config.particleLifetime = 2.0f;
    config.startSize = 0.03f;
    config.endSize = 0.01f;
    config.startColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    config.endColor = glm::vec4(0.0f, 0.5f, 1.0f, 0.0f);
    config.velocity = 5.0f;
    
    m_dataFlow = std::make_shared<ParticleEmitter>(config);
    addChild(m_dataFlow);
}

void ServiceConnectionVisualizer::addConnection(ServiceEntity* from, ServiceEntity* to, float strength) {
    Connection conn;
    conn.from = from;
    conn.to = to;
    conn.strength = strength;
    conn.pulsePhase = 0.0f;
    
    createConnectionBeam(conn);
    m_connections.push_back(conn);
}

void ServiceConnectionVisualizer::removeConnection(ServiceEntity* from, ServiceEntity* to) {
    auto it = std::remove_if(m_connections.begin(), m_connections.end(),
        [from, to](const Connection& conn) {
            return conn.from == from && conn.to == to;
        });
    
    if (it != m_connections.end()) {
        if (it->visual) {
            removeChild(it->visual);
        }
        m_connections.erase(it);
    }
}

void ServiceConnectionVisualizer::clearConnections() {
    for (auto& conn : m_connections) {
        if (conn.visual) {
            removeChild(conn.visual);
        }
    }
    m_connections.clear();
}

void ServiceConnectionVisualizer::update(float deltaTime) {
    SceneNode::update(deltaTime);
    updateConnectionVisuals(deltaTime);
}

void ServiceConnectionVisualizer::render(MetalRenderer* renderer) {
    // Render with additive blending for energy effect
    renderer->pushBlendMode(BlendMode::ADDITIVE);
    
    SceneNode::render(renderer);
    
    renderer->popBlendMode();
}

void ServiceConnectionVisualizer::pulseConnection(ServiceEntity* from, ServiceEntity* to) {
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [from, to](const Connection& conn) {
            return conn.from == from && conn.to == to;
        });
    
    if (it != m_connections.end()) {
        it->pulsePhase = 0.0f;
        
        // Emit particles along the connection
        if (m_dataFlow) {
            glm::vec3 start = from->getWorldPosition();
            glm::vec3 end = to->getWorldPosition();
            m_dataFlow->setEmitLine(start, end);
            m_dataFlow->burst(10);
        }
    }
}

void ServiceConnectionVisualizer::updateConnectionVisuals(float deltaTime) {
    for (auto& conn : m_connections) {
        // Update pulse animation
        conn.pulsePhase += deltaTime * 3.0f;
        
        if (conn.visual) {
            // Update connection beam position
            glm::vec3 start = conn.from->getWorldPosition();
            glm::vec3 end = conn.to->getWorldPosition();
            glm::vec3 mid = (start + end) * 0.5f;
            
            conn.visual->setPosition(mid);
            
            // Orient beam
            glm::vec3 dir = glm::normalize(end - start);
            conn.visual->setRotation(glm::quatLookAt(dir, glm::vec3(0, 1, 0)));
            
            // Scale to match distance
            float length = glm::distance(start, end);
            conn.visual->setScale(glm::vec3(0.02f, length, 0.02f));
            
            // Pulse effect
            float pulse = 0.5f + 0.5f * sin(conn.pulsePhase);
            conn.visual->setEmission(pulse * conn.strength);
        }
    }
}

void ServiceConnectionVisualizer::createConnectionBeam(Connection& conn) {
    auto beam = std::make_shared<MeshNode>();
    beam->setMesh(MeshLibrary::CYLINDER);
    beam->setMaterial(MaterialLibrary::ENERGY_BEAM);
    beam->setAlpha(0.5f * conn.strength);
    
    conn.visual = beam;
    addChild(beam);
}

} // namespace FinalStorm

