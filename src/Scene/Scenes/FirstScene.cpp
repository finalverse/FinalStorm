// ============================================================================
// File: FinalStorm/src/Scene/FirstScene.cpp
// Complete First Scene Implementation with Advanced Component Systems
// Part 6: Component Systems Integration - The Living Canvas of Finalverse
// ============================================================================

#include "FirstScene.h"
#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"
#include "Services/Components/ParticleEmitter.h"
#include "Services/Components/ConnectionBeam.h"
#include "Services/Components/EnergyRing.h"
#include "Services/Visual/ServiceVisualizations.h"
#include "Environment/EnvironmentController.h"
#include "UI/HolographicDisplay.h"
#include "UI/InteractiveOrb.h"
#include "Rendering/RenderContext.h"
#include "Network/FinalverseClient.h"
#include <iostream>
#include <random>

namespace FinalStorm {

// ============================================================================
// FirstScene Implementation - Complete with Component Systems
// ============================================================================

FirstScene::FirstScene() 
    : Scene("First Scene - Finalverse")
    , m_cameraDistance(15.0f)
    , m_cameraAngle(0.0f)
    , m_cameraHeight(5.0f)
    , m_environmentTime(0.0f)
    , m_serviceDiscoveryActive(false)
    , m_isInitialized(false)
    , m_transitionProgress(0.0f)
    , m_introductionPhase(IntroductionPhase::ENVIRONMENT_SETUP)
    , m_connectionManager(nullptr)
    , m_centralNexus(nullptr)
    , m_environmentController(nullptr) {
    
    std::cout << "FirstScene: Initializing the Living Canvas of Finalverse..." << std::endl;
}

FirstScene::~FirstScene() {
    std::cout << "FirstScene: Destructor called." << std::endl;
}

void FirstScene::initialize() {
    if (m_isInitialized) return;
    
    std::cout << "FirstScene: Beginning initialization sequence..." << std::endl;
    
    // Phase 1: Create the foundation environment
    createEnvironment();
    
    // Phase 2: Set up the central nexus with energy rings
    createCentralNexus();
    
    // Phase 3: Initialize connection management system
    createConnectionSystem();
    
    // Phase 4: Create service discovery interface
    createServiceDiscoveryUI();
    
    // Phase 5: Set up ambient particle systems
    createAmbientEffects();
    
    // Phase 6: Initialize camera and lighting
    setupCameraAndLighting();
    
    // Phase 7: Connect to Finalverse network
    initializeNetworking();
    
    m_isInitialized = true;
    std::cout << "FirstScene: Initialization complete!" << std::endl;
}

void FirstScene::update(float deltaTime) {
    Scene::update(deltaTime);
    
    if (!m_isInitialized) {
        initialize();
        return;
    }
    
    m_environmentTime += deltaTime;
    
    updateIntroductionSequence(deltaTime);
    updateCamera(deltaTime);
    updateEnvironment(deltaTime);
    updateServices(deltaTime);
    updateConnections(deltaTime);
    updateParticleEffects(deltaTime);
    updateNetworking(deltaTime);
    updateAudio(deltaTime);
}

void FirstScene::render(RenderContext& context) {
    if (!m_isInitialized) return;
    
    // Set up scene-wide rendering state
    context.pushBlendMode(BlendMode::ALPHA);
    context.enableDepthTest(true);
    context.enableDepthWrite(true);
    
    // Apply camera transform
    applyCameraTransform(context);
    
    // Render environment first
    renderEnvironment(context);
    
    // Render central nexus and energy systems
    renderCentralNexus(context);
    
    // Render services and their visualizations
    renderServices(context);
    
    // Render connection network
    renderConnections(context);
    
    // Render UI elements
    renderUI(context);
    
    // Render particle effects last for proper blending
    renderParticleEffects(context);
    
    context.popBlendMode();
}

// ============================================================================
// Environment Creation - The Living Stage
// ============================================================================

void FirstScene::createEnvironment() {
    std::cout << "FirstScene: Creating immersive environment..." << std::endl;
    
    // Create environment controller
    m_environmentController = std::make_shared<EnvironmentController>();
    m_environmentController->setName("Environment Controller");
    addChild(m_environmentController);
    
    // Configure environment for Finalverse theme
    m_environmentController->setSkyboxTheme("quantum_void");
    m_environmentController->setGroundGridStyle("hexagonal");
    m_environmentController->setAmbientParticles(true);
    m_environmentController->setQuantumFluctuations(0.3f);
    
    // Create ground plane with energy grid
    createEnergyGrid();
    
    // Add atmospheric effects
    createAtmosphericEffects();
    
    // Set initial environment state
    m_environmentController->setEnergyLevel(0.2f);
    m_environmentController->setHarmonyLevel(0.8f); // High harmony for welcome
}

void FirstScene::createEnergyGrid() {
    // Create the iconic ground grid that responds to system activity
    auto gridMesh = std::make_shared<GridMesh>();
    gridMesh->setName("Energy Grid");
    gridMesh->setSize(make_vec3(50.0f, 0.0f, 50.0f));
    gridMesh->setResolution(100);
    gridMesh->setGridColor(make_vec3(0.2f, 0.6f, 1.0f));
    gridMesh->setGlowIntensity(0.5f);
    gridMesh->enableQuantumDistortion(true);
    
    m_energyGrid = gridMesh;
    addChild(m_energyGrid);
    
    // Add grid particle effects
    ParticleEmitter::Config gridParticleConfig;
    gridParticleConfig.emitShape = ParticleEmitter::Shape::BOX;
    gridParticleConfig.emitSize = make_vec3(50.0f, 0.1f, 50.0f);
    gridParticleConfig.emitRate = 5.0f;
    gridParticleConfig.particleLifetime = 8.0f;
    gridParticleConfig.startSize = 0.05f;
    gridParticleConfig.endSize = 0.02f;
    gridParticleConfig.startColor = make_vec4(0.3f, 0.7f, 1.0f, 0.6f);
    gridParticleConfig.endColor = make_vec4(0.1f, 0.3f, 0.8f, 0.0f);
    gridParticleConfig.velocity = 0.5f;
    gridParticleConfig.gravity = make_vec3(0.0f, 0.1f, 0.0f);
    
    auto gridParticles = std::make_shared<ParticleEmitter>(gridParticleConfig);
    gridParticles->setName("Grid Particles");
    gridParticles->setPosition(make_vec3(0.0f, 0.1f, 0.0f));
    addChild(gridParticles);
    
    m_gridParticles = gridParticles;
}

void FirstScene::createAtmosphericEffects() {
    // Create floating data motes for atmosphere
    ParticleEmitter::Config atmosConfig;
    atmosConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    atmosConfig.emitRadius = 30.0f;
    atmosConfig.emitRate = 8.0f;
    atmosConfig.particleLifetime = 15.0f;
    atmosConfig.startSize = 0.08f;
    atmosConfig.endSize = 0.03f;
    atmosConfig.startColor = make_vec4(0.4f, 0.8f, 1.0f, 0.4f);
    atmosConfig.endColor = make_vec4(0.2f, 0.4f, 0.8f, 0.0f);
    atmosConfig.velocity = 1.0f;
    atmosConfig.velocityVariation = 0.5f;
    atmosConfig.gravity = make_vec3(0.0f, 0.05f, 0.0f);
    atmosConfig.turbulence = 0.2f;
    
    auto atmosParticles = std::make_shared<ParticleEmitter>(atmosConfig);
    atmosParticles->setName("Atmospheric Motes");
    atmosParticles->setPosition(make_vec3(0.0f, 10.0f, 0.0f));
    addChild(atmosParticles);
    
    m_atmosphericParticles = atmosParticles;
}

// ============================================================================
// Central Nexus Creation - The Heart of the System
// ============================================================================

void FirstScene::createCentralNexus() {
    std::cout << "FirstScene: Creating central nexus with energy rings..." << std::endl;
    
    // Create the central nexus container
    m_centralNexus = std::make_shared<SceneNode>("Central Nexus");
    m_centralNexus->setPosition(make_vec3(0.0f, 2.0f, 0.0f));
    addChild(m_centralNexus);
    
    // Create multiple energy rings for a complex nexus
    createNexusRings();
    
    // Add central core visualization
    createNexusCore();
    
    // Add energy pillars extending upward
    createEnergyPillars();
    
    // Create orbital service platforms
    createServiceOrbitalPlatforms();
}

void FirstScene::createNexusRings() {
    // Primary ring - largest, slowest
    auto primaryRing = std::make_shared<EnergyRing>();
    primaryRing->setName("Primary Ring");
    primaryRing->setInnerRadius(3.0f);
    primaryRing->setOuterRadius(3.5f);
    primaryRing->setHeight(0.2f);
    primaryRing->setColor(make_vec3(0.3f, 0.7f, 1.0f));
    primaryRing->setRotationSpeed(0.5f);
    primaryRing->setRingType(EnergyRing::RingType::POWER_CORE);
    primaryRing->setState(EnergyRing::RingState::ACTIVE);
    primaryRing->enableHarmonicResonance(true, 0.8f);
    m_centralNexus->addChild(primaryRing);
    m_nexusRings.push_back(primaryRing);
    
    // Secondary ring - medium, counter-rotating
    auto secondaryRing = std::make_shared<EnergyRing>();
    secondaryRing->setName("Secondary Ring");
    secondaryRing->setInnerRadius(2.0f);
    secondaryRing->setOuterRadius(2.3f);
    secondaryRing->setHeight(0.15f);
    secondaryRing->setColor(make_vec3(0.5f, 0.3f, 1.0f));
    secondaryRing->setRotationSpeed(-0.8f); // Counter-rotate
    secondaryRing->setRingType(EnergyRing::RingType::QUANTUM);
    secondaryRing->setState(EnergyRing::RingState::ACTIVE);
    secondaryRing->setPosition(make_vec3(0.0f, 0.5f, 0.0f));
    secondaryRing->setRotationAxis(make_vec3(1.0f, 0.2f, 0.0f)); // Slight tilt
    m_centralNexus->addChild(secondaryRing);
    m_nexusRings.push_back(secondaryRing);
    
    // Tertiary ring - smallest, fastest
    auto tertiaryRing = std::make_shared<EnergyRing>();
    tertiaryRing->setName("Tertiary Ring");
    tertiaryRing->setInnerRadius(1.0f);
    tertiaryRing->setOuterRadius(1.2f);
    tertiaryRing->setHeight(0.1f);
    tertiaryRing->setColor(make_vec3(1.0f, 0.4f, 0.2f));
    tertiaryRing->setRotationSpeed(1.5f);
    tertiaryRing->setRingType(EnergyRing::RingType::DATA_STREAM);
    tertiaryRing->setState(EnergyRing::RingState::ACTIVE);
    tertiaryRing->setPosition(make_vec3(0.0f, 1.0f, 0.0f));
    tertiaryRing->setRotationAxis(make_vec3(0.0f, 1.0f, 0.5f)); // Different tilt
    m_centralNexus->addChild(tertiaryRing);
    m_nexusRings.push_back(tertiaryRing);
    
    std::cout << "FirstScene: Created " << m_nexusRings.size() << " nexus rings." << std::endl;
}

void FirstScene::createNexusCore() {
    // Central energy core with pulsing effects
    auto coreOrb = std::make_shared<InteractiveOrb>();
    coreOrb->setName("Nexus Core");
    coreOrb->setRadius(0.8f);
    coreOrb->setColor(make_vec3(1.0f, 1.0f, 1.0f));
    coreOrb->setGlowIntensity(1.5f);
    coreOrb->setPulseRate(1.2f);
    coreOrb->enableQuantumFlicker(true);
    coreOrb->setPosition(make_vec3(0.0f, 0.5f, 0.0f));
    
    // Set up core interaction
    coreOrb->setOnActivateCallback([this]() {
        onNexusCoreActivated();
    });
    
    m_centralNexus->addChild(coreOrb);
    m_nexusCore = coreOrb;
    
    // Add core particle effects
    ParticleEmitter::Config coreConfig;
    coreConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    coreConfig.emitRadius = 0.9f;
    coreConfig.emitRate = 15.0f;
    coreConfig.particleLifetime = 3.0f;
    coreConfig.startSize = 0.04f;
    coreConfig.endSize = 0.15f;
    coreConfig.startColor = make_vec4(1.0f, 1.0f, 1.0f, 0.8f);
    coreConfig.endColor = make_vec4(0.3f, 0.7f, 1.0f, 0.0f);
    coreConfig.velocity = 2.0f;
    coreConfig.gravity = make_vec3(0.0f, 0.0f, 0.0f); // Neutral gravity
    
    auto coreParticles = std::make_shared<ParticleEmitter>(coreConfig);
    coreParticles->setName("Core Particles");
    coreParticles->setPosition(make_vec3(0.0f, 0.5f, 0.0f));
    m_centralNexus->addChild(coreParticles);
    
    m_coreParticles = coreParticles;
}

void FirstScene::createEnergyPillars() {
    // Create vertical energy beams extending from the nexus
    int pillarCount = 6;
    float pillarRadius = 5.0f;
    
    for (int i = 0; i < pillarCount; ++i) {
        float angle = (i / float(pillarCount)) * 2.0f * M_PI;
        vec3 pillarPos = make_vec3(
            cos(angle) * pillarRadius,
            0.0f,
            sin(angle) * pillarRadius
        );
        
        // Create energy pillar using connection beam
        auto pillar = std::make_shared<ConnectionBeam>();
        pillar->setName("Energy Pillar " + std::to_string(i));
        pillar->setStartPosition(pillarPos);
        pillar->setEndPosition(pillarPos + make_vec3(0.0f, 8.0f, 0.0f));
        pillar->setConnectionType(ConnectionBeam::ConnectionType::STREAM);
        pillar->setConnectionState(ConnectionBeam::ConnectionState::ACTIVE);
        pillar->setColor(make_vec3(0.4f, 0.8f, 1.0f));
        pillar->setIntensity(0.7f);
        pillar->setThickness(0.1f);
        pillar->setFlowSpeed(2.0f);
        pillar->setFlowDirection(1.0f); // Upward flow
        pillar->enableQuantumFlicker(true);
        
        m_centralNexus->addChild(pillar);
        m_energyPillars.push_back(pillar);
    }
    
    std::cout << "FirstScene: Created " << m_energyPillars.size() << " energy pillars." << std::endl;
}

void FirstScene::createServiceOrbitalPlatforms() {
    // Create orbital platforms where services will be placed
    int platformCount = 8;
    float orbitRadius = 8.0f;
    
    for (int i = 0; i < platformCount; ++i) {
        float angle = (i / float(platformCount)) * 2.0f * M_PI;
        vec3 platformPos = make_vec3(
            cos(angle) * orbitRadius,
            1.5f + sin(angle * 2.0f) * 0.5f, // Slight height variation
            sin(angle) * orbitRadius
        );
        
        // Create platform base
        auto platform = std::make_shared<SceneNode>("Service Platform " + std::to_string(i));
        platform->setPosition(platformPos);
        
        // Add platform energy ring
        auto platformRing = std::make_shared<EnergyRing>();
        platformRing->setName("Platform Ring");
        platformRing->setInnerRadius(0.8f);
        platformRing->setOuterRadius(1.0f);
        platformRing->setHeight(0.05f);
        platformRing->setColor(make_vec3(0.6f, 0.6f, 0.6f));
        platformRing->setGlowIntensity(0.3f);
        platformRing->setRotationSpeed(0.2f);
        platformRing->setState(EnergyRing::RingState::IDLE);
        platform->addChild(platformRing);
        
        addChild(platform);
        m_servicePlatforms.push_back(platform);
    }
    
    std::cout << "FirstScene: Created " << m_servicePlatforms.size() << " service platforms." << std::endl;
}

// ============================================================================
// Connection System Creation - The Neural Network
// ============================================================================

void FirstScene::createConnectionSystem() {
    std::cout << "FirstScene: Creating connection management system..." << std::endl;
    
    // Create connection manager
    m_connectionManager = std::make_shared<ConnectionManager>();
    m_connectionManager->setName("Connection Manager");
    addChild(m_connectionManager);
    
    // Set up connection visualization parameters
    m_connectionManager->setGlobalIntensity(0.8f);
    m_connectionManager->setGlobalFlowSpeed(1.5f);
    
    // Create initial connection network visualization
    createInitialConnections();
}

void FirstScene::createInitialConnections() {
    // Create connections between the central nexus and platforms
    vec3 nexusPos = m_centralNexus->getPosition();
    
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        vec3 platformPos = m_servicePlatforms[i]->getPosition();
        
        // Create connection to nexus
        auto connection = m_connectionManager->createConnection(
            nexusPos + make_vec3(0.0f, 0.5f, 0.0f),
            platformPos + make_vec3(0.0f, 0.2f, 0.0f),
            ConnectionBeam::ConnectionType::DATA_FLOW
        );
        
        connection->setConnectionState(ConnectionBeam::ConnectionState::ACTIVE);
        connection->setIntensity(0.4f);
        connection->setThickness(0.03f);
        connection->setFlowSpeed(1.0f);
        connection->setColor(make_vec3(0.5f, 0.7f, 1.0f));
        
        m_platformConnections.push_back(connection);
    }
    
    // Create inter-platform connections for network visualization
    createInterPlatformConnections();
}

void FirstScene::createInterPlatformConnections() {
    // Create connections between adjacent platforms
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        size_t nextIndex = (i + 1) % m_servicePlatforms.size();
        
        vec3 platformA = m_servicePlatforms[i]->getPosition();
        vec3 platformB = m_servicePlatforms[nextIndex]->getPosition();
        
        auto connection = m_connectionManager->createConnection(
            platformA + make_vec3(0.0f, 0.1f, 0.0f),
            platformB + make_vec3(0.0f, 0.1f, 0.0f),
            ConnectionBeam::ConnectionType::STREAM
        );
        
        connection->setConnectionState(ConnectionBeam::ConnectionState::ACTIVE);
        connection->setIntensity(0.2f);
        connection->setThickness(0.02f);
        connection->setFlowSpeed(0.5f);
        connection->setColor(make_vec3(0.3f, 0.5f, 0.8f));
        
        m_networkConnections.push_back(connection);
    }
}

// ============================================================================
// Service Discovery UI - The Interactive Gateway
// ============================================================================

void FirstScene::createServiceDiscoveryUI() {
    std::cout << "FirstScene: Creating service discovery interface..." << std::endl;
    
    // Create main discovery orb
    auto discoveryOrb = std::make_shared<InteractiveOrb>();
    discoveryOrb->setName("Service Discovery");
    discoveryOrb->setPosition(make_vec3(0.0f, 6.0f, 0.0f));
    discoveryOrb->setRadius(1.2f);
    discoveryOrb->setColor(make_vec3(0.2f, 1.0f, 0.6f));
    discoveryOrb->setGlowIntensity(1.0f);
    discoveryOrb->setPulseRate(0.8f);
    discoveryOrb->enableHolographicShader(true);
    
    discoveryOrb->setOnActivateCallback([this]() {
        onServiceDiscoveryActivated();
    });
    
    addChild(discoveryOrb);
    m_serviceDiscoveryOrb = discoveryOrb;
    
    // Create discovery ring around the orb
    auto discoveryRing = std::make_shared<EnergyRing>();
    discoveryRing->setName("Discovery Ring");
    discoveryRing->setInnerRadius(2.0f);
    discoveryRing->setOuterRadius(2.3f);
    discoveryRing->setHeight(0.1f);
    discoveryRing->setColor(make_vec3(0.2f, 1.0f, 0.6f));
    discoveryRing->setRotationSpeed(1.0f);
    discoveryRing->setRingType(EnergyRing::RingType::DATA_STREAM);
    discoveryRing->setState(EnergyRing::RingState::IDLE);
    discoveryOrb->addChild(discoveryRing);
    
    m_discoveryRing = discoveryRing;
    
    // Create holographic display for service information
    createServiceInformationDisplay();
}

void FirstScene::createServiceInformationDisplay() {
    // Create holographic display panel
    auto infoDisplay = std::make_shared<HolographicDisplay>();
    infoDisplay->setName("Service Information");
    infoDisplay->setPosition(make_vec3(5.0f, 4.0f, 0.0f));
    infoDisplay->setSize(make_vec2(4.0f, 3.0f));
    infoDisplay->setColor(make_vec4(0.3f, 0.8f, 1.0f, 0.8f));
    infoDisplay->setText("Finalverse Services\n\nConnecting to network...");
    infoDisplay->setVisible(false); // Initially hidden
    
    addChild(infoDisplay);
    m_serviceInfoDisplay = infoDisplay;
}

// ============================================================================
// Ambient Effects - The Living Atmosphere
// ============================================================================

void FirstScene::createAmbientEffects() {
    std::cout << "FirstScene: Creating ambient particle effects..." << std::endl;
    
    // Create floating light orbs
    createFloatingLightOrbs();
    
    // Create energy wisps
    createEnergyWisps();
    
    // Create quantum fluctuation effects
    createQuantumFluctuations();
}

void FirstScene::createFloatingLightOrbs() {
    // Ambient light orbs floating around the scene
    int orbCount = 12;
    
    for (int i = 0; i < orbCount; ++i) {
        float angle = (i / float(orbCount)) * 2.0f * M_PI;
        float radius = 15.0f + (i % 3) * 5.0f;
        float height = 3.0f + (i % 4) * 2.0f;
        
        vec3 orbPos = make_vec3(
            cos(angle) * radius,
            height,
            sin(angle) * radius
        );
        
        auto lightOrb = std::make_shared<InteractiveOrb>();
        lightOrb->setName("Light Orb " + std::to_string(i));
        lightOrb->setPosition(orbPos);
        lightOrb->setRadius(0.3f);
        lightOrb->setColor(make_vec3(0.6f, 0.8f, 1.0f));
        lightOrb->setGlowIntensity(0.5f);
        lightOrb->setPulseRate(0.3f + (i * 0.1f));
        lightOrb->enableFloat(true);
        lightOrb->setFloatSpeed(0.2f);
        lightOrb->setFloatRange(2.0f);
        
        addChild(lightOrb);
        m_ambientOrbs.push_back(lightOrb);
    }
}

void FirstScene::createEnergyWisps() {
    // Energy wisps that flow around the scene
    ParticleEmitter::Config wispConfig;
    wispConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    wispConfig.emitRadius = 25.0f;
    wispConfig.emitRate = 3.0f;
    wispConfig.particleLifetime = 20.0f;
    wispConfig.startSize = 0.2f;
    wispConfig.endSize = 0.05f;
    wispConfig.startColor = make_vec4(0.4f, 0.8f, 1.0f, 0.6f);
    wispConfig.endColor = make_vec4(0.2f, 0.4f, 0.8f, 0.0f);
    wispConfig.velocity = 1.5f;
    wispConfig.velocityVariation = 0.8f;
    wispConfig.gravity = make_vec3(0.0f, 0.02f, 0.0f);
    wispConfig.turbulence = 0.3f;
    
    auto wispEmitter = std::make_shared<ParticleEmitter>(wispConfig);
    wispEmitter->setName("Energy Wisps");
    wispEmitter->setPosition(make_vec3(0.0f, 8.0f, 0.0f));
    addChild(wispEmitter);
    
    m_energyWisps = wispEmitter;
}

void FirstScene::createQuantumFluctuations() {
    // Quantum fluctuation particles
    ParticleEmitter::Config quantumConfig;
    quantumConfig.emitShape = ParticleEmitter::Shape::BOX;
    quantumConfig.emitSize = make_vec3(40.0f, 15.0f, 40.0f);
    quantumConfig.emitRate = 8.0f;
    quantumConfig.particleLifetime = 5.0f;
    quantumConfig.startSize = 0.01f;
    quantumConfig.endSize = 0.08f;
    quantumConfig.startColor = make_vec4(1.0f, 1.0f, 1.0f, 0.3f);
    quantumConfig.endColor = make_vec4(0.8f, 0.8f, 1.0f, 0.0f);
    quantumConfig.velocity = 0.5f;
    quantumConfig.velocityVariation = 1.0f;
    quantumConfig.gravity = make_vec3(0.0f, 0.0f, 0.0f);
    quantumConfig.turbulence = 0.5f;
    
    auto quantumEmitter = std::make_shared<ParticleEmitter>(quantumConfig);
    quantumEmitter->setName("Quantum Fluctuations");
    quantumEmitter->setPosition(make_vec3(0.0f, 5.0f, 0.0f));
    addChild(quantumEmitter);
    
    m_quantumFluctuations = quantumEmitter;
}

// ============================================================================
// Update Methods - The Living Pulse
// ============================================================================

void FirstScene::updateIntroductionSequence(float deltaTime) {
    // Handle the introduction sequence phases
    switch (m_introductionPhase) {
        case IntroductionPhase::ENVIRONMENT_SETUP:
            m_transitionProgress += deltaTime * 0.5f; // 2 second setup
            if (m_transitionProgress >= 1.0f) {
                m_introductionPhase = IntroductionPhase::NEXUS_ACTIVATION;
                m_transitionProgress = 0.0f;
                activateNexus();
            }
            break;
            
        case IntroductionPhase::NEXUS_ACTIVATION:
            m_transitionProgress += deltaTime * 0.3f; // ~3 second activation
            updateNexusActivation(m_transitionProgress);
            if (m_transitionProgress >= 1.0f) {
                m_introductionPhase = IntroductionPhase::SERVICE_DISCOVERY;
                m_transitionProgress = 0.0f;
                enableServiceDiscovery();
            }
            break;
            
        case IntroductionPhase::SERVICE_DISCOVERY:
            m_transitionProgress += deltaTime * 0.2f; // 5 second discovery
            updateServiceDiscovery(m_transitionProgress);
            if (m_transitionProgress >= 1.0f) {
                m_introductionPhase = IntroductionPhase::COMPLETE;
                m_transitionProgress = 1.0f;
                completeIntroduction();
            }
            break;
            
        case IntroductionPhase::COMPLETE:
            // Introduction complete, normal operation
            break;
    }
}

void FirstScene::updateCamera(float deltaTime) {
    // Smooth camera rotation around the scene
    m_cameraAngle += deltaTime * 0.1f; // Slow rotation
    
    // Calculate camera position
    vec3 cameraPos = make_vec3(
        cos(m_cameraAngle) * m_cameraDistance,
        m_cameraHeight,
        sin(m_cameraAngle) * m_cameraDistance
    );
    
    // Update camera
    if (m_camera) {
        m_camera->setPosition(cameraPos);
        m_camera->lookAt(make_vec3(0.0f, 2.0f, 0.0f), make_vec3(0.0f, 1.0f, 0.0f));
    }
}

void FirstScene::updateEnvironment(float deltaTime) {
    if (m_environmentController) {
        // Update environment based on system activity
        float systemActivity = calculateSystemActivity();
        m_environmentController->setEnergyLevel(systemActivity);
        
        // Update grid effects
        if (m_energyGrid) {
            m_energyGrid->setActivityLevel(systemActivity);
            m_energyGrid->setPulseRate(0.5f + systemActivity * 1.0f);
        }
        
        // Update atmospheric effects
        if (m_atmosphericParticles) {
            float emissionRate = 5.0f + systemActivity * 10.0f;
            m_atmosphericParticles->setEmitRate(emissionRate);
        }
    }
}

void FirstScene::updateServices(float deltaTime) {
    // Update service visualizations on platforms
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        auto platform = m_servicePlatforms[i];
        
        // Check if this platform has a service
        if (i < m_serviceVisualizations.size() && m_serviceVisualizations[i]) {
            auto service = m_serviceVisualizations[i];
            
            // Update service activity visualization
            float activity = getServiceActivity(i);
            service->setActivityLevel(activity);
            
            // Update platform ring based on service state
            updatePlatformRingForService(platform, service, activity);
        }
    }
}

void FirstScene::updateConnections(float deltaTime) {
    if (m_connectionManager) {
        // Update connection activities based on network traffic
        updateNetworkConnections(deltaTime);
        
        // Update platform connections
        for (size_t i = 0; i < m_platformConnections.size(); ++i) {
            auto connection = m_platformConnections[i];
            if (connection && !connection->isExpired()) {
                float activity = getServiceActivity(i);
                connection->setIntensity(0.3f + activity * 0.5f);
                connection->setFlowSpeed(0.8f + activity * 1.2f);
            }
        }
        
        // Occasionally create data packet visualizations
        if (fmod(m_environmentTime, 2.0f) < deltaTime) {
            createRandomDataPacket();
        }
    }
}

void FirstScene::updateParticleEffects(float deltaTime) {
    // Update nexus core particles based on system state
    if (m_coreParticles) {
        float coreActivity = calculateNexusActivity();
        float emissionRate = 10.0f + coreActivity * 20.0f;
        m_coreParticles->setEmitRate(emissionRate);
        
        // Change color based on harmony level
        float harmonyLevel = calculateHarmonyLevel();
        vec4 coreColor = lerpColor(
            make_vec4(1.0f, 0.3f, 0.3f, 0.8f), // Red for discord
            make_vec4(0.3f, 1.0f, 0.8f, 0.8f), // Cyan for harmony
            harmonyLevel
        );
        m_coreParticles->setParticleColor(coreColor);
    }
    
    // Update ambient orb animations
    updateAmbientOrbs(deltaTime);
}

void FirstScene::updateNetworking(float deltaTime) {
    // Update network client and handle incoming data
    if (m_finalverseClient && m_finalverseClient->isConnected()) {
        // Process incoming service updates
        auto serviceUpdates = m_finalverseClient->getServiceUpdates();
        for (const auto& update : serviceUpdates) {
            handleServiceUpdate(update);
        }
        
        // Process network events
        auto networkEvents = m_finalverseClient->getNetworkEvents();
        for (const auto& event : networkEvents) {
            handleNetworkEvent(event);
        }
        
        // Send periodic status updates
        static float statusTimer = 0.0f;
        statusTimer += deltaTime;
        if (statusTimer >= 5.0f) { // Every 5 seconds
            sendStatusUpdate();
            statusTimer = 0.0f;
        }
    }
}

void FirstScene::updateAudio(float deltaTime) {
    // Update spatial audio based on scene activity
    if (m_audioEngine) {
        // Update ambient audio intensity
        float ambientLevel = calculateSystemActivity();
        m_audioEngine->setAmbientLevel(ambientLevel);
        
        // Update nexus audio
        float nexusActivity = calculateNexusActivity();
        m_audioEngine->updateNexusAudio(nexusActivity);
        
        // Update service audio
        for (size_t i = 0; i < m_serviceVisualizations.size(); ++i) {
            if (m_serviceVisualizations[i]) {
                float serviceActivity = getServiceActivity(i);
                vec3 servicePos = m_servicePlatforms[i]->getPosition();
                m_audioEngine->updateServiceAudio(i, servicePos, serviceActivity);
            }
        }
    }
}

// ============================================================================
// Event Handlers - The Interactive Symphony
// ============================================================================

void FirstScene::onNexusCoreActivated() {
    std::cout << "FirstScene: Nexus core activated!" << std::endl;
    
    // Trigger nexus-wide effects
    for (auto& ring : m_nexusRings) {
        ring->triggerRipple(1.5f);
        ring->setState(EnergyRing::RingState::OVERLOAD);
    }
    
    // Pulse all energy pillars
    for (auto& pillar : m_energyPillars) {
        pillar->pulse(2.0f);
    }
    
    // Create burst effect from core
    if (m_coreParticles) {
        m_coreParticles->burst(50);
    }
    
    // Trigger audio effect
    if (m_audioEngine) {
        m_audioEngine->playNexusActivationSound();
    }
    
    // After a delay, return rings to normal state
    scheduleRingStateChange(3.0f, EnergyRing::RingState::ACTIVE);
}

void FirstScene::onServiceDiscoveryActivated() {
    std::cout << "FirstScene: Service discovery activated!" << std::endl;
    
    m_serviceDiscoveryActive = !m_serviceDiscoveryActive;
    
    if (m_serviceDiscoveryActive) {
        // Activate discovery mode
        if (m_discoveryRing) {
            m_discoveryRing->setState(EnergyRing::RingState::ACTIVE);
            m_discoveryRing->setRotationSpeed(2.0f);
            m_discoveryRing->triggerRipple(1.0f);
        }
        
        // Show service information display
        if (m_serviceInfoDisplay) {
            m_serviceInfoDisplay->setVisible(true);
            m_serviceInfoDisplay->setText("Scanning for services...");
        }
        
        // Start service discovery process
        startServiceDiscovery();
        
    } else {
        // Deactivate discovery mode
        if (m_discoveryRing) {
            m_discoveryRing->setState(EnergyRing::RingState::IDLE);
            m_discoveryRing->setRotationSpeed(0.5f);
        }
        
        // Hide service information display
        if (m_serviceInfoDisplay) {
            m_serviceInfoDisplay->setVisible(false);
        }
    }
}

void FirstScene::onServicePlatformActivated(int platformIndex) {
    std::cout << "FirstScene: Service platform " << platformIndex << " activated!" << std::endl;
    
    if (platformIndex >= 0 && platformIndex < static_cast<int>(m_servicePlatforms.size())) {
        auto platform = m_servicePlatforms[platformIndex];
        
        // Create or update service visualization on this platform
        createServiceVisualization(platformIndex);
        
        // Trigger platform effects
        triggerPlatformActivation(platform);
        
        // Create connection burst
        if (platformIndex < static_cast<int>(m_platformConnections.size())) {
            auto connection = m_platformConnections[platformIndex];
            if (connection) {
                connection->pulse(1.5f);
                connection->showDataPacket(make_vec3(0.2f, 1.0f, 0.6f), 0.15f);
            }
        }
    }
}

// ============================================================================
// Service Management - The Dynamic Ecosystem
// ============================================================================

void FirstScene::createServiceVisualization(int platformIndex) {
    if (platformIndex < 0 || platformIndex >= static_cast<int>(m_servicePlatforms.size())) {
        return;
    }
    
    auto platform = m_servicePlatforms[platformIndex];
    
    // Remove existing service if any
    if (platformIndex < static_cast<int>(m_serviceVisualizations.size()) && 
        m_serviceVisualizations[platformIndex]) {
        platform->removeChild(m_serviceVisualizations[platformIndex]);
    }
    
    // Ensure vector is large enough
    if (m_serviceVisualizations.size() <= static_cast<size_t>(platformIndex)) {
        m_serviceVisualizations.resize(platformIndex + 1);
    }
    
    // Create appropriate service visualization based on platform index
    std::shared_ptr<ServiceVisualization> serviceViz;
    
    switch (platformIndex % 8) {
        case 0: // API Gateway
            serviceViz = std::make_shared<APIGatewayVisualization>();
            break;
        case 1: // World Engine
            serviceViz = std::make_shared<WorldEngineVisualization>();
            break;
        case 2: // AI Orchestra
            serviceViz = std::make_shared<AIServiceVisualization>();
            break;
        case 3: // Song Engine
            serviceViz = std::make_shared<AudioServiceVisualization>();
            break;
        case 4: // Echo Engine
            serviceViz = std::make_shared<EchoEngineVisualization>();
            break;
        case 5: // Database/Storage
            serviceViz = std::make_shared<DatabaseVisualization>();
            break;
        case 6: // Community Service
            serviceViz = std::make_shared<CommunityVisualization>();
            break;
        case 7: // Harmony Service
            serviceViz = std::make_shared<HarmonyServiceVisualization>();
            break;
    }
    
    if (serviceViz) {
        serviceViz->setName("Service " + std::to_string(platformIndex));
        serviceViz->setPosition(make_vec3(0.0f, 0.5f, 0.0f)); // Above platform
        serviceViz->initialize();
        
        platform->addChild(serviceViz);
        m_serviceVisualizations[platformIndex] = serviceViz;
        
        std::cout << "FirstScene: Created service visualization " << 
                     serviceViz->getServiceType() << " on platform " << platformIndex << std::endl;
    }
}

void FirstScene::updateServiceVisualization(int platformIndex, const ServiceMetrics& metrics) {
    if (platformIndex >= 0 && platformIndex < static_cast<int>(m_serviceVisualizations.size()) &&
        m_serviceVisualizations[platformIndex]) {
        
        auto service = m_serviceVisualizations[platformIndex];
        service->updateMetrics(metrics);
        
        // Update platform ring color based on service health
        updatePlatformRingForMetrics(platformIndex, metrics);
        
        // Update connection intensity
        if (platformIndex < static_cast<int>(m_platformConnections.size()) &&
            m_platformConnections[platformIndex]) {
            
            auto connection = m_platformConnections[platformIndex];
            float healthIntensity = metrics.health * 0.8f + 0.2f;
            connection->setIntensity(healthIntensity);
            
            // Change color based on service state
            vec3 connectionColor = make_vec3(0.5f, 0.7f, 1.0f);
            if (metrics.health < 0.3f) {
                connectionColor = make_vec3(1.0f, 0.3f, 0.3f); // Red for unhealthy
            } else if (metrics.health > 0.8f) {
                connectionColor = make_vec3(0.3f, 1.0f, 0.5f); // Green for healthy
            }
            connection->setColor(connectionColor);
        }
    }
}

// ============================================================================
// Network Integration - The Living Connection
// ============================================================================

void FirstScene::initializeNetworking() {
    std::cout << "FirstScene: Initializing Finalverse network connection..." << std::endl;
    
    // Create Finalverse client
    m_finalverseClient = std::make_shared<FinalverseClient>();
    
    // Set up event handlers
    m_finalverseClient->setOnConnectedCallback([this]() {
        onNetworkConnected();
    });
    
    m_finalverseClient->setOnDisconnectedCallback([this]() {
        onNetworkDisconnected();
    });
    
    m_finalverseClient->setOnServiceUpdateCallback([this](const ServiceUpdate& update) {
        handleServiceUpdate(update);
    });
    
    m_finalverseClient->setOnNetworkEventCallback([this](const NetworkEvent& event) {
        handleNetworkEvent(event);
    });
    
    // Attempt connection
    try {
        m_finalverseClient->connect("ws://localhost:3000/ws");
        std::cout << "FirstScene: Connection initiated to Finalverse server." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "FirstScene: Failed to connect to Finalverse server: " << e.what() << std::endl;
        // Continue in offline mode
    }
}

void FirstScene::onNetworkConnected() {
    std::cout << "FirstScene: Connected to Finalverse network!" << std::endl;
    
    // Update UI to show connected state
    if (m_serviceInfoDisplay) {
        m_serviceInfoDisplay->setText("Connected to Finalverse\n\nDiscovering services...");
    }
    
    // Trigger connection effects
    if (m_serviceDiscoveryOrb) {
        m_serviceDiscoveryOrb->setColor(make_vec3(0.2f, 1.0f, 0.2f)); // Green for connected
        m_serviceDiscoveryOrb->pulse(1.5f);
    }
    
    // Start service discovery
    if (m_finalverseClient) {
        m_finalverseClient->requestServiceList();
    }
}

void FirstScene::onNetworkDisconnected() {
    std::cout << "FirstScene: Disconnected from Finalverse network." << std::endl;
    
    // Update UI to show disconnected state
    if (m_serviceInfoDisplay) {
        m_serviceInfoDisplay->setText("Disconnected from Finalverse\n\nAttempting reconnection...");
    }
    
    // Change discovery orb to indicate disconnection
    if (m_serviceDiscoveryOrb) {
        m_serviceDiscoveryOrb->setColor(make_vec3(1.0f, 0.5f, 0.2f)); // Orange for disconnected
    }
    
    // Fade out service visualizations
    fadeOutServices();
}

void FirstScene::handleServiceUpdate(const ServiceUpdate& update) {
    std::cout << "FirstScene: Received service update for " << update.serviceName << std::endl;
    
    // Find the appropriate platform for this service
    int platformIndex = findPlatformForService(update.serviceName);
    if (platformIndex == -1) {
        // Assign to next available platform
        platformIndex = assignServiceToPlatform(update.serviceName);
    }
    
    if (platformIndex >= 0) {
        // Create or update service visualization
        if (platformIndex >= static_cast<int>(m_serviceVisualizations.size()) ||
            !m_serviceVisualizations[platformIndex]) {
            createServiceVisualization(platformIndex);
        }
        
        // Update with new metrics
        ServiceMetrics metrics;
        metrics.health = update.health;
        metrics.load = update.load;
        metrics.connections = update.connections;
        metrics.requestsPerSecond = update.requestsPerSecond;
        metrics.responseTime = update.responseTime;
        
        updateServiceVisualization(platformIndex, metrics);
        
        // Update service info display if this is the selected service
        updateServiceInfoDisplay(update);
    }
}

void FirstScene::handleNetworkEvent(const NetworkEvent& event) {
    std::cout << "FirstScene: Network event: " << event.type << std::endl;
    
    switch (event.type) {
        case NetworkEventType::DATA_TRANSFER:
            visualizeDataTransfer(event);
            break;
            
        case NetworkEventType::SERVICE_ERROR:
            visualizeServiceError(event);
            break;
            
        case NetworkEventType::HIGH_LOAD:
            visualizeHighLoad(event);
            break;
            
        case NetworkEventType::CONNECTION_ESTABLISHED:
            visualizeNewConnection(event);
            break;
            
        case NetworkEventType::CONNECTION_LOST:
            visualizeConnectionLost(event);
            break;
    }
}

// ============================================================================
// Visual Effects - The Spectacular Display
// ============================================================================

void FirstScene::visualizeDataTransfer(const NetworkEvent& event) {
    int fromPlatform = findPlatformForService(event.sourceService);
    int toPlatform = findPlatformForService(event.targetService);
    
    if (fromPlatform >= 0 && toPlatform >= 0) {
        vec3 fromPos = m_servicePlatforms[fromPlatform]->getPosition();
        vec3 toPos = m_servicePlatforms[toPlatform]->getPosition();
        
        // Create temporary connection beam for data transfer
        auto transferBeam = m_connectionManager->createConnection(
            fromPos + make_vec3(0.0f, 0.5f, 0.0f),
            toPos + make_vec3(0.0f, 0.5f, 0.0f),
            ConnectionBeam::ConnectionType::DATA_FLOW
        );
        
        transferBeam->setDuration(2.0f); // 2 second transfer visualization
        transferBeam->setIntensity(1.2f);
        transferBeam->setThickness(0.05f);
        transferBeam->setColor(make_vec3(0.2f, 1.0f, 0.6f));
        transferBeam->setFlowSpeed(3.0f);
        transferBeam->showDataPacket(make_vec3(1.0f, 1.0f, 0.2f), 0.1f);
        
        // Play transfer sound
        if (m_audioEngine) {
            m_audioEngine->playDataTransferSound(fromPos, toPos);
        }
    }
}

void FirstScene::visualizeServiceError(const NetworkEvent& event) {
    int platformIndex = findPlatformForService(event.sourceService);
    if (platformIndex >= 0) {
        auto platform = m_servicePlatforms[platformIndex];
        
        // Flash platform ring red
        auto platformRing = platform->findChildByName("Platform Ring");
        if (platformRing) {
            auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
            if (energyRing) {
                energyRing->setColor(make_vec3(1.0f, 0.2f, 0.2f));
                energyRing->setState(EnergyRing::RingState::CRITICAL);
                energyRing->triggerRipple(1.0f);
                
                // Schedule return to normal after 3 seconds
                scheduleRingColorChange(energyRing, 3.0f, make_vec3(0.6f, 0.6f, 0.6f));
            }
        }
        
        // Create error particle burst
        ParticleEmitter::Config errorConfig;
        errorConfig.emitShape = ParticleEmitter::Shape::SPHERE;
        errorConfig.emitRadius = 1.0f;
        errorConfig.particleLifetime = 2.0f;
        errorConfig.startSize = 0.1f;
        errorConfig.endSize = 0.02f;
        errorConfig.startColor = make_vec4(1.0f, 0.3f, 0.3f, 1.0f);
        errorConfig.endColor = make_vec4(1.0f, 0.3f, 0.3f, 0.0f);
        errorConfig.velocity = 2.0f;
        
        auto errorEmitter = std::make_shared<ParticleEmitter>(errorConfig);
        errorEmitter->setPosition(platform->getPosition() + make_vec3(0.0f, 1.0f, 0.0f));
        errorEmitter->burst(20);
        
        addChild(errorEmitter);
        
        // Schedule removal after particles die
        scheduleNodeRemoval(errorEmitter, 3.0f);
    }
}

void FirstScene::visualizeHighLoad(const NetworkEvent& event) {
    int platformIndex = findPlatformForService(event.sourceService);
    if (platformIndex >= 0) {
        auto platform = m_servicePlatforms[platformIndex];
        
        // Intensify platform effects
        auto platformRing = platform->findChildByName("Platform Ring");
        if (platformRing) {
            auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
            if (energyRing) {
                energyRing->setState(EnergyRing::RingState::OVERLOAD);
                energyRing->setRotationSpeed(energyRing->getRotationSpeed() * 2.0f);
                energyRing->setGlowIntensity(1.5f);
                
                // Schedule return to normal
                scheduleRingStateChange(energyRing, 5.0f, EnergyRing::RingState::ACTIVE);
            }
        }
        
        // Update connection to nexus
        if (platformIndex < static_cast<int>(m_platformConnections.size()) &&
            m_platformConnections[platformIndex]) {
            
            auto connection = m_platformConnections[platformIndex];
            connection->setIntensity(1.5f);
            connection->setFlowSpeed(2.5f);
            connection->setColor(make_vec3(1.0f, 0.7f, 0.2f)); // Orange for high load
        }
    }
}

// ============================================================================
// Helper Methods - The Supporting Framework
// ============================================================================

float FirstScene::calculateSystemActivity() const {
    float totalActivity = 0.0f;
    int activeServices = 0;
    
    for (const auto& service : m_serviceVisualizations) {
        if (service) {
            totalActivity += service->getActivityLevel();
            activeServices++;
        }
    }
    
    return activeServices > 0 ? totalActivity / activeServices : 0.2f;
}

float FirstScene::calculateNexusActivity() const {
    // Base activity from system + special nexus effects
    float baseActivity = calculateSystemActivity();
    
    // Add pulsing effect
    float pulseEffect = 0.5f + 0.3f * sin(m_environmentTime * 2.0f);
    
    return clamp(baseActivity * pulseEffect, 0.0f, 1.0f);
}

float FirstScene::calculateHarmonyLevel() const {
    // In Finalverse, harmony represents the balance between services
    float harmony = 0.8f; // Default high harmony
    
    // Check for service errors or high loads
    int healthyServices = 0;
    int totalServices = 0;
    
    for (const auto& service : m_serviceVisualizations) {
        if (service) {
            totalServices++;
            if (service->getHealthLevel() > 0.7f) {
                healthyServices++;
            }
        }
    }
    
    if (totalServices > 0) {
        harmony = static_cast<float>(healthyServices) / totalServices;
    }
    
    return harmony;
}

float FirstScene::getServiceActivity(int platformIndex) const {
    if (platformIndex >= 0 && platformIndex < static_cast<int>(m_serviceVisualizations.size()) &&
        m_serviceVisualizations[platformIndex]) {
        return m_serviceVisualizations[platformIndex]->getActivityLevel();
    }
    return 0.1f; // Minimal activity for empty platforms
}

int FirstScene::findPlatformForService(const std::string& serviceName) const {
    for (size_t i = 0; i < m_serviceVisualizations.size(); ++i) {
        if (m_serviceVisualizations[i] && 
            m_serviceVisualizations[i]->getServiceName() == serviceName) {
            return static_cast<int>(i);
        }
    }
    return -1; // Not found
}

int FirstScene::assignServiceToPlatform(const std::string& serviceName) {
    // Find first available platform
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        if (i >= m_serviceVisualizations.size() || !m_serviceVisualizations[i]) {
            return static_cast<int>(i);
        }
    }
    return -1; // No available platforms
}

vec4 FirstScene::lerpColor(const vec4& a, const vec4& b, float t) const {
    return make_vec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
}

// ============================================================================
// Scene Lifecycle - The Grand Symphony
// ============================================================================

void FirstScene::activateNexus() {
    std::cout << "FirstScene: Activating central nexus..." << std::endl;
    
    // Gradually increase nexus ring speeds and intensities
    for (auto& ring : m_nexusRings) {
        ring->setState(EnergyRing::RingState::ACTIVE);
        ring->setGlowIntensity(1.0f);
        ring->enableHarmonicResonance(true, 1.0f);
    }
    
    // Activate energy pillars
    for (auto& pillar : m_energyPillars) {
        pillar->setConnectionState(ConnectionBeam::ConnectionState::ACTIVE);
        pillar->setIntensity(0.8f);
    }
    
    // Increase core particle emission
    if (m_coreParticles) {
        m_coreParticles->setEmitRate(20.0f);
    }
}

void FirstScene::enableServiceDiscovery() {
    std::cout << "FirstScene: Enabling service discovery..." << std::endl;
    
    // Activate discovery orb
    if (m_serviceDiscoveryOrb) {
        m_serviceDiscoveryOrb->setGlowIntensity(1.2f);
        m_serviceDiscoveryOrb->setPulseRate(1.5f);
    }
    
    // Start discovery ring animation
    if (m_discoveryRing) {
        m_discoveryRing->setState(EnergyRing::RingState::ACTIVE);
        m_discoveryRing->setRotationSpeed(1.5f);
    }
    
    // Begin actual service discovery
    startServiceDiscovery();
}

void FirstScene::completeIntroduction() {
    std::cout << "FirstScene: Introduction sequence complete!" << std::endl;
    
    // Enable all interactive elements
    enableAllInteractions();
    
    // Show welcome message
    if (m_serviceInfoDisplay) {
        m_serviceInfoDisplay->setVisible(true);
        m_serviceInfoDisplay->setText("Welcome to Finalverse!\n\nYour immersive journey into\nthe Song of Creation begins.");
    }
    
    // Play completion audio
    if (m_audioEngine) {
        m_audioEngine->playIntroductionCompleteSound();
    }
}

void FirstScene::startServiceDiscovery() {
    if (m_finalverseClient && m_finalverseClient->isConnected()) {
        m_finalverseClient->requestServiceList();
    } else {
        // Demo mode - create mock services
        createDemoServices();
    }
}

void FirstScene::createDemoServices() {
    std::cout << "FirstScene: Creating demo services..." << std::endl;
    
    // Create visualizations for common Finalverse services
    std::vector<std::string> demoServices = {
        "API Gateway",
        "World Engine", 
        "AI Orchestra",
        "Song Engine",
        "Echo Engine",
        "Asset Service",
        "Community Service",
        "Harmony Service"
    };
    
    for (size_t i = 0; i < demoServices.size() && i < m_servicePlatforms.size(); ++i) {
        createServiceVisualization(static_cast<int>(i));
        
        // Simulate service metrics
        ServiceMetrics metrics;
        metrics.health = 0.8f + (rand() % 20) / 100.0f;
        metrics.load = 0.3f + (rand() % 40) / 100.0f;
        metrics.connections = 10 + (rand() % 50);
        metrics.requestsPerSecond = 5.0f + (rand() % 20);
        metrics.responseTime = 50.0f + (rand() % 100);
        
        updateServiceVisualization(static_cast<int>(i), metrics);
        
        // Trigger platform activation effect
        onServicePlatformActivated(static_cast<int>(i));
    }
}

// ============================================================================
// Cleanup and Resource Management
// ============================================================================

void FirstScene::cleanup() {
    std::cout << "FirstScene: Cleaning up resources..." << std::endl;
    
    // Disconnect from network
    if (m_finalverseClient) {
        m_finalverseClient->disconnect();
        m_finalverseClient.reset();
    }
    
    // Clear all containers
    m_serviceVisualizations.clear();
    m_servicePlatforms.clear();
    m_platformConnections.clear();
    m_networkConnections.clear();
    m_nexusRings.clear();
    m_energyPillars.clear();
    m_ambientOrbs.clear();
    
    // Reset shared pointers
    m_connectionManager.reset();
    m_centralNexus.reset();
    m_environmentController.reset();
    m_serviceDiscoveryOrb.reset();
    m_discoveryRing.reset();
    m_serviceInfoDisplay.reset();
    m_nexusCore.reset();
    m_energyGrid.reset();
    
    // Clear particle systems
    m_coreParticles.reset();
    m_gridParticles.reset();
    m_atmosphericParticles.reset();
    m_energyWisps.reset();
    m_quantumFluctuations.reset();
    
    Scene::cleanup();
}

// ============================================================================
// Private Helper Methods
// ============================================================================

void FirstScene::updateNexusActivation(float progress) {
    // Smooth activation of nexus components
    float intensity = smoothstep(0.0f, 1.0f, progress);
    
    // Update ring intensities
    for (size_t i = 0; i < m_nexusRings.size(); ++i) {
        auto& ring = m_nexusRings[i];
        float ringIntensity = intensity * (0.5f + i * 0.25f);
        ring->setGlowIntensity(ringIntensity);
        
        if (progress > 0.3f) {
            ring->setState(EnergyRing::RingState::ACTIVE);
        }
    }
    
    // Update pillar intensities
    for (auto& pillar : m_energyPillars) {
        pillar->setIntensity(intensity * 0.8f);
    }
    
    // Update core effects
    if (m_nexusCore) {
        m_nexusCore->setGlowIntensity(intensity * 1.5f);
        m_nexusCore->setPulseRate(0.5f + intensity * 1.0f);
    }
}

void FirstScene::updateServiceDiscovery(float progress) {
    // Animate service discovery process
    if (m_discoveryRing) {
        float rotationSpeed = 0.5f + progress * 2.0f;
        m_discoveryRing->setRotationSpeed(rotationSpeed);
        
        if (progress > 0.5f) {
            m_discoveryRing->triggerRipple(progress);
        }
    }
    
    // Update discovery orb
    if (m_serviceDiscoveryOrb) {
        float pulseRate = 0.8f + progress * 1.5f;
        m_serviceDiscoveryOrb->setPulseRate(pulseRate);
    }
}

void FirstScene::setupCameraAndLighting() {
    // Set up main camera
    m_camera = std::make_shared<Camera>();
    m_camera->setName("Main Camera");
    m_camera->setPerspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    
    // Initial camera position
    vec3 initialPos = make_vec3(m_cameraDistance, m_cameraHeight, 0.0f);
    m_camera->setPosition(initialPos);
    m_camera->lookAt(make_vec3(0.0f, 2.0f, 0.0f), make_vec3(0.0f, 1.0f, 0.0f));
    
    setActiveCamera(m_camera);
    
    // Set up lighting
    createSceneLighting();
}

void FirstScene::createSceneLighting() {
    // Main directional light (simulating ambient cosmic energy)
    auto mainLight = std::make_shared<DirectionalLight>();
    mainLight->setName("Main Light");
    mainLight->setDirection(normalize(make_vec3(-0.3f, -0.8f, -0.4f)));
    mainLight->setColor(make_vec3(0.8f, 0.9f, 1.0f));
    mainLight->setIntensity(0.6f);
    addChild(mainLight);
    
    // Ambient light for general illumination
    auto ambientLight = std::make_shared<AmbientLight>();
    ambientLight->setName("Ambient Light");
    ambientLight->setColor(make_vec3(0.3f, 0.4f, 0.6f));
    ambientLight->setIntensity(0.4f);
    addChild(ambientLight);
    
    // Point light at nexus core
    auto nexusLight = std::make_shared<PointLight>();
    nexusLight->setName("Nexus Light");
    nexusLight->setPosition(make_vec3(0.0f, 2.5f, 0.0f));
    nexusLight->setColor(make_vec3(0.4f, 0.8f, 1.0f));
    nexusLight->setIntensity(1.2f);
    nexusLight->setRadius(15.0f);
    addChild(nexusLight);
    
    // Discovery light
    auto discoveryLight = std::make_shared<PointLight>();
    discoveryLight->setName("Discovery Light");
    discoveryLight->setPosition(make_vec3(0.0f, 6.0f, 0.0f));
    discoveryLight->setColor(make_vec3(0.2f, 1.0f, 0.6f));
    discoveryLight->setIntensity(0.8f);
    discoveryLight->setRadius(8.0f);
    addChild(discoveryLight);
}

void FirstScene::applyCameraTransform(RenderContext& context) {
    if (m_camera) {
        context.setViewMatrix(m_camera->getViewMatrix());
        context.setProjectionMatrix(m_camera->getProjectionMatrix());
        context.setViewPosition(m_camera->getPosition());
    }
}

void FirstScene::renderEnvironment(RenderContext& context) {
    // Environment is rendered by child nodes automatically
    // Additional environment rendering can be added here
}

void FirstScene::renderCentralNexus(RenderContext& context) {
    // Central nexus and its components render themselves
    // Additional nexus-specific rendering can be added here
}

void FirstScene::renderServices(RenderContext& context) {
    // Services render themselves as child nodes
    // Additional service-specific rendering can be added here
}

void FirstScene::renderConnections(RenderContext& context) {
    // Connections are managed by ConnectionManager
    // Additional connection rendering can be added here
}

void FirstScene::renderUI(RenderContext& context) {
    // UI elements render themselves
    // Additional UI rendering can be added here
}

void FirstScene::renderParticleEffects(RenderContext& context) {
    // Particle effects render themselves
    // Additional particle rendering can be added here
}

void FirstScene::updatePlatformRingForService(std::shared_ptr<SceneNode> platform, 
                                             std::shared_ptr<ServiceVisualization> service, 
                                             float activity) {
    auto platformRing = platform->findChildByName("Platform Ring");
    if (platformRing) {
        auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
        if (energyRing) {
            // Update ring based on service activity
            energyRing->setGlowIntensity(0.3f + activity * 0.7f);
            energyRing->setRotationSpeed(0.2f + activity * 0.8f);
            
            // Update ring color based on service type
            vec3 serviceColor = service->getServiceColor();
            energyRing->setColor(serviceColor);
            
            // Update ring state based on service health
            float health = service->getHealthLevel();
            if (health > 0.8f) {
                energyRing->setState(EnergyRing::RingState::ACTIVE);
            } else if (health > 0.5f) {
                energyRing->setState(EnergyRing::RingState::IDLE);
            } else {
                energyRing->setState(EnergyRing::RingState::CRITICAL);
            }
        }
    }
}

void FirstScene::updatePlatformRingForMetrics(int platformIndex, const ServiceMetrics& metrics) {
    if (platformIndex >= 0 && platformIndex < static_cast<int>(m_servicePlatforms.size())) {
        auto platform = m_servicePlatforms[platformIndex];
        auto platformRing = platform->findChildByName("Platform Ring");
        
        if (platformRing) {
            auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
            if (energyRing) {
                // Update based on metrics
                energyRing->setGlowIntensity(0.3f + metrics.load * 0.7f);
                energyRing->setDataThroughput(metrics.requestsPerSecond);
                energyRing->setConnectionCount(metrics.connections);
                energyRing->setProcessingLoad(metrics.load);
                
                // Color based on health
                vec3 healthColor;
                if (metrics.health > 0.8f) {
                    healthColor = make_vec3(0.3f, 1.0f, 0.5f); // Green
                } else if (metrics.health > 0.5f) {
                    healthColor = make_vec3(0.8f, 0.8f, 0.3f); // Yellow
                } else {
                    healthColor = make_vec3(1.0f, 0.3f, 0.3f); // Red
                }
                energyRing->setColor(healthColor);
            }
        }
    }
}

void FirstScene::updateNetworkConnections(float deltaTime) {
    // Simulate network activity on connections
    static float networkTimer = 0.0f;
    networkTimer += deltaTime;
    
    if (networkTimer >= 1.0f) { // Update every second
        networkTimer = 0.0f;
        
        // Randomly pulse some network connections
        for (auto& connection : m_networkConnections) {
            if (connection && !connection->isExpired()) {
                if ((rand() % 100) < 20) { // 20% chance per second
                    connection->pulse(1.0f + (rand() % 50) / 100.0f);
                    
                    // Occasionally show data packets
                    if ((rand() % 100) < 10) { // 10% chance
                        vec3 packetColor = make_vec3(
                            0.5f + (rand() % 50) / 100.0f,
                            0.5f + (rand() % 50) / 100.0f,
                            0.8f + (rand() % 20) / 100.0f
                        );
                        connection->showDataPacket(packetColor, 0.08f);
                    }
                }
            }
        }
    }
}

void FirstScene::createRandomDataPacket() {
    // Create random data packet between services
    if (m_serviceVisualizations.size() < 2) return;
    
    int fromIndex = rand() % m_serviceVisualizations.size();
    int toIndex = rand() % m_serviceVisualizations.size();
    
    if (fromIndex != toIndex && 
        m_serviceVisualizations[fromIndex] && 
        m_serviceVisualizations[toIndex]) {
        
        vec3 fromPos = m_servicePlatforms[fromIndex]->getPosition();
        vec3 toPos = m_servicePlatforms[toIndex]->getPosition();
        
        auto packetBeam = m_connectionManager->createConnection(
            fromPos + make_vec3(0.0f, 0.5f, 0.0f),
            toPos + make_vec3(0.0f, 0.5f, 0.0f),
            ConnectionBeam::ConnectionType::DATA_FLOW
        );
        
        packetBeam->setDuration(1.5f);
        packetBeam->setIntensity(0.8f);
        packetBeam->setThickness(0.04f);
        packetBeam->setFlowSpeed(4.0f);
        packetBeam->setColor(make_vec3(0.3f, 0.9f, 0.7f));
        packetBeam->showDataPacket(make_vec3(1.0f, 1.0f, 0.8f), 0.12f);
    }
}

void FirstScene::triggerPlatformActivation(std::shared_ptr<SceneNode> platform) {
    // Create activation effect on platform
    vec3 platformPos = platform->getPosition();
    
    // Burst effect
    ParticleEmitter::Config burstConfig;
    burstConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    burstConfig.emitRadius = 0.5f;
    burstConfig.particleLifetime = 3.0f;
    burstConfig.startSize = 0.08f;
    burstConfig.endSize = 0.02f;
    burstConfig.startColor = make_vec4(0.2f, 1.0f, 0.6f, 1.0f);
    burstConfig.endColor = make_vec4(0.2f, 1.0f, 0.6f, 0.0f);
    burstConfig.velocity = 3.0f;
    burstConfig.gravity = make_vec3(0.0f, -0.2f, 0.0f);
    
    auto burstEmitter = std::make_shared<ParticleEmitter>(burstConfig);
    burstEmitter->setPosition(platformPos + make_vec3(0.0f, 1.0f, 0.0f));
    burstEmitter->burst(30);
    
    addChild(burstEmitter);
    scheduleNodeRemoval(burstEmitter, 4.0f);
    
    // Ring activation effect
    auto platformRing = platform->findChildByName("Platform Ring");
    if (platformRing) {
        auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
        if (energyRing) {
            energyRing->triggerRipple(1.5f);
            energyRing->setState(EnergyRing::RingState::ACTIVE);
            energyRing->setGlowIntensity(1.0f);
        }
    }
}

void FirstScene::updateServiceInfoDisplay(const ServiceUpdate& update) {
    if (m_serviceInfoDisplay && m_serviceInfoDisplay->isVisible()) {
        std::string infoText = "Service: " + update.serviceName + "\n";
        infoText += "Health: " + std::to_string(static_cast<int>(update.health * 100)) + "%\n";
        infoText += "Load: " + std::to_string(static_cast<int>(update.load * 100)) + "%\n";
        infoText += "Connections: " + std::to_string(update.connections) + "\n";
        infoText += "RPS: " + std::to_string(static_cast<int>(update.requestsPerSecond)) + "\n";
        infoText += "Response: " + std::to_string(static_cast<int>(update.responseTime)) + "ms";
        
        m_serviceInfoDisplay->setText(infoText);
    }
}

void FirstScene::fadeOutServices() {
    // Gradually fade out all service visualizations
    for (auto& service : m_serviceVisualizations) {
        if (service) {
            service->setFadeOut(true);
            service->setFadeOutDuration(2.0f);
        }
    }
    
    // Fade platform rings to idle state
    for (auto& platform : m_servicePlatforms) {
        auto platformRing = platform->findChildByName("Platform Ring");
        if (platformRing) {
            auto energyRing = std::dynamic_pointer_cast<EnergyRing>(platformRing);
            if (energyRing) {
                energyRing->setState(EnergyRing::RingState::IDLE);
                energyRing->setGlowIntensity(0.2f);
            }
        }
    }
}

void FirstScene::enableAllInteractions() {
    // Enable nexus core interaction
    if (m_nexusCore) {
        m_nexusCore->setInteractable(true);
    }
    
    // Enable service discovery interaction
    if (m_serviceDiscoveryOrb) {
        m_serviceDiscoveryOrb->setInteractable(true);
    }
    
    // Enable platform interactions
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        auto platform = m_servicePlatforms[i];
        platform->setInteractable(true);
        
        // Set up platform click handler
        platform->setOnActivateCallback([this, i]() {
            onServicePlatformActivated(static_cast<int>(i));
        });
    }
}

void FirstScene::sendStatusUpdate() {
    if (m_finalverseClient && m_finalverseClient->isConnected()) {
        // Send client status to server
        ClientStatus status;
        status.sceneType = "FirstScene";
        status.activeServices = static_cast<int>(m_serviceVisualizations.size());
        status.systemActivity = calculateSystemActivity();
        status.harmonyLevel = calculateHarmonyLevel();
        status.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        
        m_finalverseClient->sendStatusUpdate(status);
    }
}

// ============================================================================
// Scheduled Actions - The Temporal Orchestra
// ============================================================================

void FirstScene::scheduleRingStateChange(float delay, EnergyRing::RingState newState) {
    for (auto& ring : m_nexusRings) {
        scheduleRingStateChange(ring, delay, newState);
    }
}

void FirstScene::scheduleRingStateChange(std::shared_ptr<EnergyRing> ring, float delay, EnergyRing::RingState newState) {
    // In a real implementation, this would use a timer/scheduler system
    // For now, we'll use a simple delayed action
    auto delayedAction = std::make_shared<DelayedAction>();
    delayedAction->delay = delay;
    delayedAction->action = [ring, newState]() {
        if (ring) {
            ring->setState(newState);
        }
    };
    
    m_scheduledActions.push_back(delayedAction);
}

void FirstScene::scheduleRingColorChange(std::shared_ptr<EnergyRing> ring, float delay, const vec3& newColor) {
    auto delayedAction = std::make_shared<DelayedAction>();
    delayedAction->delay = delay;
    delayedAction->action = [ring, newColor]() {
        if (ring) {
            ring->setColor(newColor);
            ring->setState(EnergyRing::RingState::IDLE);
        }
    };
    
    m_scheduledActions.push_back(delayedAction);
}

void FirstScene::scheduleNodeRemoval(std::shared_ptr<SceneNode> node, float delay) {
    auto delayedAction = std::make_shared<DelayedAction>();
    delayedAction->delay = delay;
    delayedAction->action = [this, node]() {
        if (node) {
            removeChild(node);
        }
    };
    
    m_scheduledActions.push_back(delayedAction);
}

void FirstScene::updateScheduledActions(float deltaTime) {
    // Update all scheduled actions
    for (auto it = m_scheduledActions.begin(); it != m_scheduledActions.end();) {
        auto& action = *it;
        action->delay -= deltaTime;
        
        if (action->delay <= 0.0f) {
            // Execute action
            if (action->action) {
                action->action();
            }
            
            // Remove from list
            it = m_scheduledActions.erase(it);
        } else {
            ++it;
        }
    }
}

// ============================================================================
// Audio Integration - The Sonic Landscape
// ============================================================================

void FirstScene::initializeAudio() {
    if (m_audioEngine) {
        // Set up scene-specific audio
        m_audioEngine->loadAmbientTrack("finalverse_ambient");
        m_audioEngine->loadSoundEffect("nexus_activation", "nexus_activate.wav");
        m_audioEngine->loadSoundEffect("service_discovery", "discovery_chime.wav");
        m_audioEngine->loadSoundEffect("data_transfer", "data_flow.wav");
        m_audioEngine->loadSoundEffect("platform_activation", "platform_glow.wav");
        m_audioEngine->loadSoundEffect("introduction_complete", "harmony_chord.wav");
        
        // Start ambient audio
        m_audioEngine->playAmbientTrack("finalverse_ambient", 0.6f, true);
        
        // Set up spatial audio sources
        setupSpatialAudio();
    }
}

void FirstScene::setupSpatialAudio() {
    if (!m_audioEngine) return;
    
    // Nexus core audio source
    vec3 nexusPos = m_centralNexus ? m_centralNexus->getPosition() : vec3_zero();
    m_audioEngine->createSpatialSource("nexus_core", nexusPos);
    m_audioEngine->setSpatialSourceLoop("nexus_core", "energy_hum.wav", 0.4f);
    
    // Service platform audio sources
    for (size_t i = 0; i < m_servicePlatforms.size(); ++i) {
        std::string sourceName = "platform_" + std::to_string(i);
        vec3 platformPos = m_servicePlatforms[i]->getPosition();
        m_audioEngine->createSpatialSource(sourceName, platformPos);
    }
    
    // Discovery orb audio source
    if (m_serviceDiscoveryOrb) {
        m_audioEngine->createSpatialSource("discovery_orb", m_serviceDiscoveryOrb->getPosition());
    }
}

} // namespace FinalStorm