// FirstScene.h - The Nexus: FinalStorm's default immersive scene
#pragma once

#include "Scene/Scene.h"
#include "Scene/SceneNode.h"
#include "Services/ServiceEntity.h"
#include <memory>
#include <vector>

namespace FinalStorm {

// Forward declarations
class ServiceDiscoveryUI;
class EnvironmentController;
class WelcomeSequence;
class ServiceRing;
class CentralNexus;

/**
 * The Nexus - FinalStorm's first scene
 * 
 * An immersive environment that introduces users to the Finalverse ecosystem.
 * Features a central hub with service visualizations arranged in an 
 * interactive ring, floating in a dynamic nebula environment.
 */
class FirstScene : public Scene {
public:
    FirstScene();
    ~FirstScene();
    
    // Scene lifecycle
    void initialize() override;
    void onEnter() override;
    void onExit() override;
    
    // Update
    void update(float deltaTime) override;
    
    // User interaction
    void onServiceSelected(ServiceEntity* service);
    void onServiceHovered(ServiceEntity* service);
    
    // Scene control
    void showWelcomeSequence();
    void skipToMainView();
    void toggleServiceDiscovery();
    
private:
    // Core components
    std::unique_ptr<CentralNexus> m_nexus;
    std::unique_ptr<ServiceRing> m_serviceRing;
    std::unique_ptr<EnvironmentController> m_environment;
    std::unique_ptr<WelcomeSequence> m_welcomeSequence;
    
    // Services
    std::vector<std::shared_ptr<ServiceEntity>> m_services;
    ServiceEntity* m_selectedService = nullptr;
    
    // State
    bool m_isFirstVisit = true;
    float m_sceneTime = 0.0f;
    
    // Camera positions
    struct CameraKeyframe {
        glm::vec3 position;
        glm::vec3 lookAt;
        float fov;
        float duration;
    };
    
    std::vector<CameraKeyframe> m_introSequence;
    
    void createEnvironment();
    void createServices();
    void createUIElements();
    void setupLighting();
    void setupCameraPath();
};

/**
 * Central Nexus - The heart of the first scene
 * A crystalline structure that pulses with the heartbeat of the Finalverse
 */
class CentralNexus : public SceneNode {
public:
    CentralNexus();
    
    void update(float deltaTime) override;
    void render(MetalRenderer* renderer) override;
    
    void pulse(float intensity = 1.0f);
    void setConnectionStrength(float strength);
    
private:
    // Visual components
    std::shared_ptr<SceneNode> m_coreStructure;
    std::shared_ptr<SceneNode> m_innerRings;
    std::shared_ptr<SceneNode> m_outerShell;
    std::shared_ptr<ParticleEmitter> m_energyFlow;
    
    // Animation state
    float m_rotationPhase = 0.0f;
    float m_pulsePhase = 0.0f;
    float m_pulseIntensity = 0.0f;
    float m_connectionStrength = 1.0f;
    
    // Visual parameters
    glm::vec3 m_coreColor = glm::vec3(0.0f, 0.8f, 1.0f);
    glm::vec3 m_energyColor = glm::vec3(0.2f, 0.9f, 1.0f);
    
    void createCoreStructure();
    void createEnergyRings();
    void updateEnergyFlow(float deltaTime);
};

/**
 * Service Ring - Orbital arrangement of services
 * Services orbit around the central nexus in an interactive ring
 */
class ServiceRing : public SceneNode {
public:
    ServiceRing();
    
    void addService(std::shared_ptr<ServiceEntity> service);
    void removeService(ServiceEntity* service);
    void clear();
    
    void update(float deltaTime) override;
    
    // Animation control
    void expandRing(float targetRadius, float duration = 2.0f);
    void contractRing(float targetRadius, float duration = 2.0f);
    void highlightService(ServiceEntity* service);
    void rotateToService(ServiceEntity* service, float duration = 1.0f);
    
private:
    std::vector<std::shared_ptr<ServiceEntity>> m_services;
    
    // Layout parameters
    float m_radius = 10.0f;
    float m_targetRadius = 10.0f;
    float m_verticalSpread = 2.0f;
    float m_rotationSpeed = 0.1f;
    
    // Animation state
    float m_expansionTime = 0.0f;
    float m_expansionDuration = 0.0f;
    float m_rotationOffset = 0.0f;
    ServiceEntity* m_highlightedService = nullptr;
    
    void updateServicePositions(float deltaTime);
    glm::vec3 getServicePosition(int index, float radius);
};

/**
 * Welcome Sequence - First-time user experience
 * An animated introduction to the Finalverse ecosystem
 */
class WelcomeSequence : public SceneNode {
public:
    WelcomeSequence();
    
    void start();
    void skip();
    bool isComplete() const { return m_isComplete; }
    
    void update(float deltaTime) override;
    void render(MetalRenderer* renderer) override;
    
    // Callbacks
    std::function<void()> onComplete;
    
private:
    enum class Phase {
        FADE_IN,
        TITLE_REVEAL,
        NEXUS_FORMATION,
        SERVICE_INTRODUCTION,
        READY
    };
    
    Phase m_currentPhase = Phase::FADE_IN;
    float m_phaseTime = 0.0f;
    bool m_isComplete = false;
    
    // UI elements
    std::shared_ptr<UI3DPanel> m_titlePanel;
    std::shared_ptr<UI3DPanel> m_subtitlePanel;
    std::shared_ptr<UI3DPanel> m_instructionPanel;
    
    // Visual effects
    std::shared_ptr<ParticleEmitter> m_sparkles;
    float m_fadeAlpha = 1.0f;
    
    void updatePhase(float deltaTime);
    void transitionToPhase(Phase newPhase);
    void createUIElements();
};

/**
 * Nebula Environment - Dynamic background
 * A living, breathing nebula that responds to system activity
 */
class NebulaEnvironment : public EnvironmentController {
public:
    NebulaEnvironment();
    
    void update(float deltaTime) override;
    void render(MetalRenderer* renderer) override;
    
    void setColorScheme(const glm::vec3& primary, const glm::vec3& secondary);
    void setDensity(float density);
    void addStarburst(const glm::vec3& position, float intensity);
    
private:
    // Nebula layers
    std::vector<std::shared_ptr<SceneNode>> m_nebulaLayers;
    std::shared_ptr<ParticleEmitter> m_stars;
    std::shared_ptr<ParticleEmitter> m_dust;
    
    // Animation
    float m_flowPhase = 0.0f;
    std::vector<float> m_layerSpeeds;
    
    // Visual parameters
    glm::vec3 m_primaryColor = glm::vec3(0.1f, 0.2f, 0.4f);
    glm::vec3 m_secondaryColor = glm::vec3(0.4f, 0.1f, 0.3f);
    float m_density = 0.7f;
    
    void createNebulaLayers();
    void updateFlow(float deltaTime);
};

/**
 * Interactive Guide - Helpful companion
 * A floating assistant that provides context and guidance
 */
class InteractiveGuide : public SceneNode {
public:
    InteractiveGuide();
    
    void showTip(const std::string& text, float duration = 5.0f);
    void showServiceInfo(ServiceEntity* service);
    void hide();
    
    void update(float deltaTime) override;
    void render(MetalRenderer* renderer) override;
    
    // Movement
    void moveToPosition(const glm::vec3& target, float duration = 1.0f);
    void orbitAround(const glm::vec3& center, float radius);
    
private:
    // Visual components
    std::shared_ptr<SceneNode> m_core;
    std::shared_ptr<UI3DPanel> m_textPanel;
    std::shared_ptr<ParticleEmitter> m_aura;
    
    // State
    bool m_isVisible = true;
    float m_bobPhase = 0.0f;
    float m_textDisplayTime = 0.0f;
    float m_textDuration = 0.0f;
    
    // Movement
    glm::vec3 m_targetPosition;
    float m_moveTime = 0.0f;
    float m_moveDuration = 0.0f;
    
    void updateMovement(float deltaTime);
    void updateBobbing(float deltaTime);
};

/**
 * Service Connection Visualizer
 * Shows the connections and data flow between services
 */
class ServiceConnectionVisualizer : public SceneNode {
public:
    ServiceConnectionVisualizer();
    
    void addConnection(ServiceEntity* from, ServiceEntity* to, float strength = 1.0f);
    void removeConnection(ServiceEntity* from, ServiceEntity* to);
    void clearConnections();
    
    void update(float deltaTime) override;
    void render(MetalRenderer* renderer) override;
    
    void pulseConnection(ServiceEntity* from, ServiceEntity* to);
    
private:
    struct Connection {
        ServiceEntity* from;
        ServiceEntity* to;
        float strength;
        float pulsePhase;
        std::shared_ptr<SceneNode> visual;
    };
    
    std::vector<Connection> m_connections;
    std::shared_ptr<ParticleEmitter> m_dataFlow;
    
    void updateConnectionVisuals(float deltaTime);
    void createConnectionBeam(Connection& conn);
};

} // namespace FinalStorm