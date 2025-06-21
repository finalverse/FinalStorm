// ============================================================================
// File: FinalStorm/src/Scene/FirstScene.h
// Complete First Scene Header with Advanced Component Systems Integration
// ============================================================================

#pragma once

#include "Scene/Scene.h"
#include "Core/Math/MathTypes.h"
#include "Network/ServiceTypes.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <chrono>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class ConnectionBeam;
class ConnectionManager;
class EnergyRing;
class InteractiveOrb;
class HolographicDisplay;
class ServiceVisualization;
class EnvironmentController;
class GridMesh;
class Camera;
class FinalverseClient;
class AudioEngine;
class RenderContext;

// ============================================================================
// Service Metrics Structure
// ============================================================================

struct ServiceMetrics {
    float health = 1.0f;              // 0.0 - 1.0 (0 = critical, 1 = perfect)
    float load = 0.0f;                // 0.0 - 1.0 (0 = idle, 1 = max load)
    int connections = 0;              // Number of active connections
    float requestsPerSecond = 0.0f;   // Requests per second
    float responseTime = 0.0f;        // Average response time in ms
    float uptime = 0.0f;              // Service uptime in hours
    std::string status = "active";    // Service status string
    std::map<std::string, float> customMetrics; // Service-specific metrics
};

// ============================================================================
// Network Event Types
// ============================================================================

enum class NetworkEventType {
    DATA_TRANSFER,
    SERVICE_ERROR,
    HIGH_LOAD,
    CONNECTION_ESTABLISHED,
    CONNECTION_LOST,
    HEARTBEAT,
    DISCOVERY_RESPONSE
};

struct NetworkEvent {
    NetworkEventType type;
    std::string sourceService;
    std::string targetService;
    float intensity = 1.0f;
    std::map<std::string, std::string> data;
    uint64_t timestamp;
};

// ============================================================================
// Service Update Structure
// ============================================================================

struct ServiceUpdate {
    std::string serviceName;
    std::string serviceType;
    std::string endpoint;
    float health = 1.0f;
    float load = 0.0f;
    int connections = 0;
    float requestsPerSecond = 0.0f;
    float responseTime = 0.0f;
    bool isOnline = true;
    std::map<std::string, float> metrics;
};

// ============================================================================
// Client Status Structure
// ============================================================================

struct ClientStatus {
    std::string sceneType;
    int activeServices = 0;
    float systemActivity = 0.0f;
    float harmonyLevel = 0.8f;
    uint64_t timestamp;
    std::map<std::string, float> clientMetrics;
};

// ============================================================================
// Introduction Phases
// ============================================================================

enum class IntroductionPhase {
    ENVIRONMENT_SETUP,    // Setting up the environment
    NEXUS_ACTIVATION,     // Activating the central nexus
    SERVICE_DISCOVERY,    // Discovering and connecting to services
    COMPLETE              // Introduction complete, normal operation
};

// ============================================================================
// Delayed Action System
// ============================================================================

struct DelayedAction {
    float delay;
    std::function<void()> action;
};

// ============================================================================
// FirstScene - The Immersive Canvas of Finalverse
// ============================================================================

class FirstScene : public Scene {
public:
    FirstScene();
    virtual ~FirstScene();

    // Scene lifecycle
    void initialize() override;
    void update(float deltaTime) override;
    void render(RenderContext& context) override;
    void cleanup() override;

    // Scene state
    bool isInitialized() const { return m_isInitialized; }
    IntroductionPhase getIntroductionPhase() const { return m_introductionPhase; }
    float getTransitionProgress() const { return m_transitionProgress; }

    // Camera control
    void setCameraDistance(float distance) { m_cameraDistance = distance; }
    void setCameraHeight(float height) { m_cameraHeight = height; }
    void setCameraAutoRotate(bool autoRotate) { m_cameraAutoRotate = autoRotate; }

    // Network integration
    void setFinalverseClient(std::shared_ptr<FinalverseClient> client) { m_finalverseClient = client; }
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine) { m_audioEngine = audioEngine; }

    // Service management
    void addService(const std::string& serviceName, const std::string& serviceType);
    void removeService(const std::string& serviceName);
    void updateService(const std::string& serviceName, const ServiceMetrics& metrics);
    
    // Event handlers
    void onServiceDiscovered(const ServiceUpdate& service);
    void onServiceStatusChanged(const std::string& serviceName, const ServiceMetrics& metrics);
    void onNetworkActivity(const NetworkEvent& event);

    // Interactive callbacks
    void onNexusCoreActivated();
    void onServiceDiscoveryActivated();
    void onServicePlatformActivated(int platformIndex);

    // Visual effects
    void triggerSystemPulse(float intensity = 1.0f);
    void showDataTransfer(const std::string& from, const std::string& to, float intensity = 1.0f);
    void showServiceError(const std::string& serviceName, const std::string& errorType);
    void showHighLoadWarning(const std::string& serviceName, float loadLevel);

    // Scene queries
    float calculateSystemActivity() const;
    float calculateNexusActivity() const;
    float calculateHarmonyLevel() const;
    int getActiveServiceCount() const;
    std::vector<std::string> getActiveServiceNames() const;

private:
    // Initialization methods
    void createEnvironment();
    void createEnergyGrid();
    void createAtmosphericEffects();
    void createCentralNexus();
    void createNexusRings();
    void createNexusCore();
    void createEnergyPillars();
    void createServiceOrbitalPlatforms();
    void createConnectionSystem();
    void createInitialConnections();
    void createInterPlatformConnections();
    void createServiceDiscoveryUI();
    void createServiceInformationDisplay();
    void createAmbientEffects();
    void createFloatingLightOrbs();
    void createEnergyWisps();
    void createQuantumFluctuations();
    void setupCameraAndLighting();
    void createSceneLighting();

    // Update methods
    void updateIntroductionSequence(float deltaTime);
    void updateCamera(float deltaTime);
    void updateEnvironment(float deltaTime);
    void updateServices(float deltaTime);
    void updateConnections(float deltaTime);
    void updateParticleEffects(float deltaTime);
    void updateAmbientOrbs(float deltaTime);
    void updateNetworking(float deltaTime);
    void updateAudio(float deltaTime);
    void updateScheduledActions(float deltaTime);

    // Rendering methods
    void applyCameraTransform(RenderContext& context);
    void renderEnvironment(RenderContext& context);
    void renderCentralNexus(RenderContext& context);
    void renderServices(RenderContext& context);
    void renderConnections(RenderContext& context);
    void renderUI(RenderContext& context);
    void renderParticleEffects(RenderContext& context);

    // Service management methods
    void createServiceVisualization(int platformIndex);
    void updateServiceVisualization(int platformIndex, const ServiceMetrics& metrics);
    void removeServiceVisualization(int platformIndex);
    void updatePlatformRingForService(std::shared_ptr<SceneNode> platform, 
                                     std::shared_ptr<ServiceVisualization> service, 
                                     float activity);
    void updatePlatformRingForMetrics(int platformIndex, const ServiceMetrics& metrics);

    // Network integration methods
    void initializeNetworking();
    void onNetworkConnected();
    void onNetworkDisconnected();
    void handleServiceUpdate(const ServiceUpdate& update);
    void handleNetworkEvent(const NetworkEvent& event);
    void sendStatusUpdate();

    // Visual effects methods
    void visualizeDataTransfer(const NetworkEvent& event);
    void visualizeServiceError(const NetworkEvent& event);
    void visualizeHighLoad(const NetworkEvent& event);
    void visualizeNewConnection(const NetworkEvent& event);
    void visualizeConnectionLost(const NetworkEvent& event);
    void triggerPlatformActivation(std::shared_ptr<SceneNode> platform);
    void createRandomDataPacket();

    // Introduction sequence methods
    void updateNexusActivation(float progress);
    void updateServiceDiscovery(float progress);
    void activateNexus();
    void enableServiceDiscovery();
    void completeIntroduction();
    void startServiceDiscovery();
    void createDemoServices();

    // Helper methods
    float getServiceActivity(int platformIndex) const;
    int findPlatformForService(const std::string& serviceName) const;
    int assignServiceToPlatform(const std::string& serviceName);
    vec4 lerpColor(const vec4& a, const vec4& b, float t) const;
    void updateNetworkConnections(float deltaTime);
    void updateServiceInfoDisplay(const ServiceUpdate& update);
    void fadeOutServices();
    void enableAllInteractions();

    // Scheduled actions
    void scheduleRingStateChange(float delay, EnergyRing::RingState newState);
    void scheduleRingStateChange(std::shared_ptr<EnergyRing> ring, float delay, EnergyRing::RingState newState);
    void scheduleRingColorChange(std::shared_ptr<EnergyRing> ring, float delay, const vec3& newColor);
    void scheduleNodeRemoval(std::shared_ptr<SceneNode> node, float delay);

    // Audio methods
    void initializeAudio();
    void setupSpatialAudio();

private:
    // Scene state
    bool m_isInitialized;
    IntroductionPhase m_introductionPhase;
    float m_transitionProgress;
    float m_environmentTime;
    bool m_serviceDiscoveryActive;

    // Camera properties
    float m_cameraDistance;
    float m_cameraAngle;
    float m_cameraHeight;
    bool m_cameraAutoRotate = true;
    std::shared_ptr<Camera> m_camera;

    // Core scene components
    std::shared_ptr<EnvironmentController> m_environmentController;
    std::shared_ptr<ConnectionManager> m_connectionManager;
    std::shared_ptr<SceneNode> m_centralNexus;

    // Central nexus components
    std::vector<std::shared_ptr<EnergyRing>> m_nexusRings;
    std::vector<std::shared_ptr<ConnectionBeam>> m_energyPillars;
    std::shared_ptr<InteractiveOrb> m_nexusCore;

    // Service platforms and visualizations
    std::vector<std::shared_ptr<SceneNode>> m_servicePlatforms;
    std::vector<std::shared_ptr<ServiceVisualization>> m_serviceVisualizations;
    std::vector<std::shared_ptr<ConnectionBeam>> m_platformConnections;
    std::vector<std::shared_ptr<ConnectionBeam>> m_networkConnections;

    // Service discovery UI
    std::shared_ptr<InteractiveOrb> m_serviceDiscoveryOrb;
    std::shared_ptr<EnergyRing> m_discoveryRing;
    std::shared_ptr<HolographicDisplay> m_serviceInfoDisplay;

    // Environment components
    std::shared_ptr<GridMesh> m_energyGrid;
    std::vector<std::shared_ptr<InteractiveOrb>> m_ambientOrbs;

    // Particle systems
    std::shared_ptr<ParticleEmitter> m_coreParticles;
    std::shared_ptr<ParticleEmitter> m_gridParticles;
    std::shared_ptr<ParticleEmitter> m_atmosphericParticles;
    std::shared_ptr<ParticleEmitter> m_energyWisps;
    std::shared_ptr<ParticleEmitter> m_quantumFluctuations;

    // Network integration
    std::shared_ptr<FinalverseClient> m_finalverseClient;
    std::shared_ptr<AudioEngine> m_audioEngine;

    // Service mapping
    std::map<std::string, int> m_serviceToplatform;
    std::map<int, std::string> m_platformToService;
    std::map<std::string, ServiceMetrics> m_serviceMetrics;

    // Scheduled actions
    std::vector<std::shared_ptr<DelayedAction>> m_scheduledActions;

    // Animation and timing
    float m_nexusRotationPhase = 0.0f;
    float m_discoveryPulsePhase = 0.0f;
    float m_ambientLightPhase = 0.0f;
    float m_networkActivityTimer = 0.0f;
    float m_statusUpdateTimer = 0.0f;
};

// ============================================================================
// Service Visualization Factory
// ============================================================================

class ServiceVisualizationFactory {
public:
    static std::shared_ptr<ServiceVisualization> createVisualization(const std::string& serviceType);
    static std::shared_ptr<ServiceVisualization> createVisualizationForService(const ServiceUpdate& service);
    
    // Service type detection
    static std::string detectServiceType(const std::string& serviceName);
    static vec3 getServiceTypeColor(const std::string& serviceType);
    static ConnectionBeam::ConnectionType getServiceConnectionType(const std::string& serviceType);
    
    // Service visualization templates
    static std::shared_ptr<ServiceVisualization> createAPIGatewayVisualization();
    static std::shared_ptr<ServiceVisualization> createDatabaseVisualization();
    static std::shared_ptr<ServiceVisualization> createAIServiceVisualization();
    static std::shared_ptr<ServiceVisualization> createAudioServiceVisualization();
    static std::shared_ptr<ServiceVisualization> createWorldEngineVisualization();
    static std::shared_ptr<ServiceVisualization> createCommunityVisualization();
    static std::shared_ptr<ServiceVisualization> createHarmonyServiceVisualization();
    static std::shared_ptr<ServiceVisualization> createEchoEngineVisualization();
};

// ============================================================================
// Service Metrics Analyzer
// ============================================================================

class ServiceMetricsAnalyzer {
public:
    static float calculateOverallHealth(const std::vector<ServiceMetrics>& allMetrics);
    static float calculateSystemLoad(const std::vector<ServiceMetrics>& allMetrics);
    static float calculateNetworkActivity(const std::vector<ServiceMetrics>& allMetrics);
    static float calculateHarmonyLevel(const std::vector<ServiceMetrics>& allMetrics);
    
    // Anomaly detection
    static bool detectHighLoad(const ServiceMetrics& metrics, float threshold = 0.8f);
    static bool detectLowHealth(const ServiceMetrics& metrics, float threshold = 0.3f);
    static bool detectSlowResponse(const ServiceMetrics& metrics, float threshold = 1000.0f);
    static bool detectConnectionSpike(const ServiceMetrics& current, const ServiceMetrics& previous, float threshold = 2.0f);
    
    // Trend analysis
    static float calculateHealthTrend(const std::vector<ServiceMetrics>& history);
    static float calculateLoadTrend(const std::vector<ServiceMetrics>& history);
    static std::string getHealthStatus(float health);
    static std::string getLoadStatus(float load);
};

// ============================================================================
// Scene Event System
// ============================================================================

class SceneEventManager {
public:
    // Event types
    enum class EventType {
        SERVICE_DISCOVERED,
        SERVICE_ACTIVATED,
        SERVICE_DEACTIVATED,
        CONNECTION_ESTABLISHED,
        CONNECTION_LOST,
        SYSTEM_ALERT,
        HARMONY_CHANGED,
        USER_INTERACTION
    };
    
    struct SceneEvent {
        EventType type;
        std::string source;
        std::string target;
        float intensity = 1.0f;
        std::map<std::string, std::string> data;
        uint64_t timestamp;
    };
    
    using EventCallback = std::function<void(const SceneEvent&)>;
    
    // Event management
    void registerEventHandler(EventType type, EventCallback callback);
    void unregisterEventHandler(EventType type);
    void triggerEvent(const SceneEvent& event);
    void triggerEvent(EventType type, const std::string& source, const std::string& target = "", float intensity = 1.0f);
    
    // Event history
    std::vector<SceneEvent> getEventHistory(EventType type = EventType::USER_INTERACTION) const;
    void clearEventHistory();
    void setMaxHistorySize(size_t maxSize) { m_maxHistorySize = maxSize; }

private:
    std::map<EventType, std::vector<EventCallback>> m_eventHandlers;
    std::vector<SceneEvent> m_eventHistory;
    size_t m_maxHistorySize = 1000;
    
    void addToHistory(const SceneEvent& event);
};

// ============================================================================
// Performance Monitor
// ============================================================================

class ScenePerformanceMonitor {
public:
    struct PerformanceMetrics {
        float frameTime = 0.0f;           // Time per frame in ms
        float updateTime = 0.0f;          // Update time in ms
        float renderTime = 0.0f;          // Render time in ms
        int particleCount = 0;            // Total active particles
        int nodeCount = 0;                // Total scene nodes
        int connectionCount = 0;          // Total active connections
        float memoryUsage = 0.0f;         // Memory usage in MB
        float cpuUsage = 0.0f;            // CPU usage percentage
    };
    
    void beginFrame();
    void endFrame();
    void beginUpdate();
    void endUpdate();
    void beginRender();
    void endRender();
    
    void updateCounts(int particles, int nodes, int connections);
    void updateSystemMetrics(float memory, float cpu);
    
    const PerformanceMetrics& getMetrics() const { return m_metrics; }
    float getAverageFrameTime() const;
    float getFPS() const;
    
    // Performance warnings
    bool isPerformanceCritical() const;
    std::vector<std::string> getPerformanceWarnings() const;
    
private:
    PerformanceMetrics m_metrics;
    std::vector<float> m_frameTimeHistory;
    size_t m_maxHistorySize = 60; // 1 second at 60 FPS
    
    std::chrono::high_resolution_clock::time_point m_frameStart;
    std::chrono::high_resolution_clock::time_point m_updateStart;
    std::chrono::high_resolution_clock::time_point m_renderStart;
    
    void addFrameTime(float frameTime);
};

// ============================================================================
// Scene Configuration
// ============================================================================

struct FirstSceneConfig {
    // Visual settings
    bool enableParticleEffects = true;
    bool enableConnectionAnimations = true;
    bool enableSpatialAudio = true;
    bool enableQuantumEffects = true;
    int maxParticles = 10000;
    int maxConnections = 100;
    
    // Performance settings
    float particleLODDistance = 50.0f;
    float connectionLODDistance = 100.0f;
    bool enableOcclusion = true;
    bool enableInstancing = true;
    
    // Interaction settings
    bool enableCameraAutoRotate = true;
    float cameraRotationSpeed = 0.1f;
    float cameraZoomSpeed = 2.0f;
    bool enableGestureControls = true;
    
    // Network settings
    std::string serverURL = "ws://localhost:3000/ws";
    float connectionTimeout = 10.0f;
    float heartbeatInterval = 5.0f;
    bool enableOfflineMode = true;
    
    // Audio settings
    float ambientVolume = 0.6f;
    float effectsVolume = 0.8f;
    bool enableSpatialAudio3D = true;
    float audioFalloffDistance = 20.0f;
    
    // Debug settings
    bool showPerformanceMetrics = false;
    bool showConnectionDebugInfo = false;
    bool showParticleDebugInfo = false;
    bool enableEventLogging = false;
};

} // namespace FinalStorm