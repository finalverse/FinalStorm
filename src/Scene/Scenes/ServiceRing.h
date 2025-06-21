// ============================================================================
// File: FinalStorm/src/Scene/Scenes/ServiceRing.h
// Service Ring Scene Component - Circular Service Organization System
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "Services/Components/EnergyRing.h"
#include "Services/Components/ConnectionBeam.h"
#include "Services/Visual/ServiceVisualization.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class InteractiveOrb;
class HolographicDisplay;
class RenderContext;
class AudioEngine;

// ============================================================================
// Service Ring Configuration
// ============================================================================

struct ServiceRingConfig {
    // Ring geometry
    float innerRadius = 8.0f;
    float outerRadius = 12.0f;
    float ringHeight = 0.5f;
    int maxServices = 12;
    float serviceSpacing = 30.0f; // Degrees between services
    
    // Ring behavior
    float rotationSpeed = 0.2f;
    bool autoRotation = true;
    bool autoArrange = true;
    bool allowManualPositioning = true;
    
    // Visual appearance
    vec3 ringColor = make_vec3(0.4f, 0.7f, 1.0f);
    float ringGlowIntensity = 0.8f;
    float serviceHighlightIntensity = 1.2f;
    bool showConnectionLines = true;
    bool showServiceLabels = true;
    
    // Interaction
    bool enableServiceDragDrop = true;
    bool enableRingInteraction = true;
    bool enableServiceClustering = true;
    float interactionRadius = 2.0f;
    
    // Animation
    float serviceEntryDuration = 2.0f;
    float serviceExitDuration = 1.5f;
    float connectionAnimationSpeed = 1.0f;
    bool enableEntranceEffects = true;
    bool enableExitEffects = true;
    
    // Audio
    bool enableAudio = true;
    float audioVolume = 0.7f;
    std::string serviceAddSound = "service_add";
    std::string serviceRemoveSound = "service_remove";
    std::string ringRotateSound = "ring_rotate";
};

// ============================================================================
// Service Position and Metadata
// ============================================================================

struct ServicePosition {
    std::string serviceId;
    std::string serviceName;
    std::string serviceType;
    float angle; // Position on ring in radians
    vec3 worldPosition;
    float priority; // Display priority
    bool isVisible;
    bool isInteractable;
    float activityLevel;
    float healthLevel;
    
    // Animation state
    bool isAnimating;
    vec3 targetPosition;
    vec3 startPosition;
    float animationProgress;
    float animationDuration;
    
    // Visual properties
    vec3 color;
    float glowIntensity;
    float scale;
    
    // Metadata
    std::map<std::string, std::string> metadata;
    uint64_t addedTime;
    uint64_t lastUpdateTime;
};

// ============================================================================
// Service Ring Events
// ============================================================================

enum class ServiceRingEventType {
    SERVICE_ADDED,
    SERVICE_REMOVED,
    SERVICE_MOVED,
    SERVICE_SELECTED,
    SERVICE_ACTIVATED,
    RING_ROTATED,
    LAYOUT_CHANGED,
    CLUSTER_FORMED,
    CLUSTER_DISSOLVED
};

struct ServiceRingEvent {
    ServiceRingEventType type;
    std::string serviceId;
    vec3 position;
    float angle;
    std::map<std::string, std::string> data;
    uint64_t timestamp;
};

// ============================================================================
// Service Cluster - Groups of related services
// ============================================================================

class ServiceCluster : public SceneNode {
public:
    ServiceCluster(const std::string& clusterId, const std::string& clusterType);
    virtual ~ServiceCluster();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Cluster management
    void addService(const std::string& serviceId);
    void removeService(const std::string& serviceId);
    void clearServices();
    bool hasService(const std::string& serviceId) const;
    int getServiceCount() const { return static_cast<int>(m_serviceIds.size()); }
    const std::vector<std::string>& getServices() const { return m_serviceIds; }

    // Cluster properties
    void setClusterType(const std::string& type);
    std::string getClusterType() const { return m_clusterType; }
    void setClusterColor(const vec3& color);
    vec3 getClusterColor() const { return m_clusterColor; }
    void setClusterRadius(float radius);
    float getClusterRadius() const { return m_clusterRadius; }

    // Visual effects
    void showClusterBoundary(bool show);
    void setClusterGlow(float intensity);
    void pulseCluster(float intensity = 1.5f);
    void highlightCluster(bool highlight);

    // Cluster behavior
    void setAutoCollapse(bool autoCollapse);
    void setMinServicesForCluster(int minServices);
    void setMaxDistanceForClustering(float maxDistance);

    // State queries
    bool isCollapsed() const { return m_isCollapsed; }
    bool isHighlighted() const { return m_isHighlighted; }
    vec3 getClusterCenter() const;
    float getClusterSpread() const;

private:
    std::string m_clusterId;
    std::string m_clusterType;
    std::vector<std::string> m_serviceIds;
    
    // Visual properties
    vec3 m_clusterColor;
    float m_clusterRadius;
    float m_glowIntensity;
    bool m_showBoundary;
    bool m_isHighlighted;
    
    // Behavior
    bool m_autoCollapse;
    bool m_isCollapsed;
    int m_minServicesForCluster;
    float m_maxDistanceForClustering;
    
    // Visual components
    std::shared_ptr<EnergyRing> m_boundaryRing;
    std::shared_ptr<ParticleEmitter> m_clusterParticles;
    
    // Animation state
    float m_pulsePhase;
    float m_glowPhase;
    
    void updateClusterVisuals();
    void createBoundaryRing();
    vec3 calculateClusterCenter() const;
};

// ============================================================================
// ServiceRing - Main service organization ring
// ============================================================================

class ServiceRing : public SceneNode {
public:
    using ServiceEventCallback = std::function<void(const ServiceRingEvent& event)>;
    using ServiceInteractionCallback = std::function<void(const std::string& serviceId, const std::string& action)>;
    using LayoutChangeCallback = std::function<void()>;

    ServiceRing(const ServiceRingConfig& config = ServiceRingConfig{});
    virtual ~ServiceRing();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;
    void onInteraction(const vec3& worldPosition, const std::string& interactionType) override;

    // Configuration
    void setConfig(const ServiceRingConfig& config);
    const ServiceRingConfig& getConfig() const { return m_config; }
    void applyConfig();

    // Service management
    void addService(const std::string& serviceId, const std::string& serviceName, const std::string& serviceType);
    void removeService(const std::string& serviceId);
    void clearServices();
    bool hasService(const std::string& serviceId) const;
    int getServiceCount() const { return static_cast<int>(m_services.size()); }
    std::vector<std::string> getServiceIds() const;

    // Service positioning
    void setServicePosition(const std::string& serviceId, float angle);
    void setServicePosition(const std::string& serviceId, const vec3& worldPosition);
    float getServiceAngle(const std::string& serviceId) const;
    vec3 getServicePosition(const std::string& serviceId) const;
    void moveServiceToPosition(const std::string& serviceId, float targetAngle, float duration = 2.0f);

    // Service properties
    void setServiceActivity(const std::string& serviceId, float activity);
    void setServiceHealth(const std::string& serviceId, float health);
    void setServiceColor(const std::string& serviceId, const vec3& color);
    void setServiceScale(const std::string& serviceId, float scale);
    void setServiceVisibility(const std::string& serviceId, bool visible);
    void setServiceInteractable(const std::string& serviceId, bool interactable);

    // Service queries
    float getServiceActivity(const std::string& serviceId) const;
    float getServiceHealth(const std::string& serviceId) const;
    vec3 getServiceColor(const std::string& serviceId) const;
    float getServiceScale(const std::string& serviceId) const;
    bool isServiceVisible(const std::string& serviceId) const;
    bool isServiceInteractable(const std::string& serviceId) const;
    const ServicePosition* getServicePosition(const std::string& serviceId) const;

    // Ring control
    void setRotationSpeed(float speed);
    void startRotation();
    void stopRotation();
    void rotateToService(const std::string& serviceId, float duration = 3.0f);
    void rotateByAngle(float angle, float duration = 2.0f);
    void setRingRotation(float rotation);
    float getRingRotation() const { return m_currentRotation; }

    // Layout management
    void autoArrangeServices();
    void arrangeServicesByType();
    void arrangeServicesByActivity();
    void arrangeServicesByHealth();
    void arrangeServicesByName();
    void compactServices();
    void distributeServicesEvenly();

    // Service selection and interaction
    void selectService(const std::string& serviceId);
    void deselectService(const std::string& serviceId);
    void clearSelection();
    std::string getSelectedService() const { return m_selectedServiceId; }
    bool isServiceSelected(const std::string& serviceId) const;
    std::string getServiceAtPosition(const vec3& worldPosition, float tolerance = 1.0f) const;

    // Clustering
    void enableClustering(bool enable);
    void addServiceToCluster(const std::string& serviceId, const std::string& clusterId);
    void removeServiceFromCluster(const std::string& serviceId);
    void createCluster(const std::string& clusterId, const std::string& clusterType, const std::vector<std::string>& serviceIds);
    void removeCluster(const std::string& clusterId);
    std::shared_ptr<ServiceCluster> getCluster(const std::string& clusterId);
    std::vector<std::string> getClusterIds() const;

    // Visual effects
    void highlightService(const std::string& serviceId, bool highlight);
    void pulseService(const std::string& serviceId, float intensity = 1.5f);
    void showServiceConnections(bool show);
    void addServiceConnection(const std::string& serviceA, const std::string& serviceB, float intensity = 1.0f);
    void removeServiceConnection(const std::string& serviceA, const std::string& serviceB);
    void clearServiceConnections();

    // Ring effects
    void pulseRing(float intensity = 1.2f);
    void setRingGlow(float intensity);
    void showRingSegmentation(bool show);
    void setRingColor(const vec3& color);
    void triggerRingEffect(const std::string& effectType, float intensity = 1.0f);

    // Audio integration
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine);
    void playServiceAddSound(const std::string& serviceId);
    void playServiceRemoveSound(const std::string& serviceId);
    void playSelectionSound(const std::string& serviceId);
    void playRotationSound(float speed);

    // Event system
    void setServiceEventCallback(ServiceEventCallback callback) { m_serviceEventCallback = callback; }
    void setServiceInteractionCallback(ServiceInteractionCallback callback) { m_serviceInteractionCallback = callback; }
    void setLayoutChangeCallback(LayoutChangeCallback callback) { m_layoutChangeCallback = callback; }

    // Performance and optimization
    void setLevelOfDetail(float distance);
    void enableOcclusion(bool enable);
    void setMaxVisibleServices(int maxVisible);
    void optimizeForPerformance(bool optimize);

    // State queries
    bool isRotating() const { return m_isRotating; }
    bool isArranging() const { return m_isArranging; }
    bool hasSelection() const { return !m_selectedServiceId.empty(); }
    int getVisibleServiceCount() const;
    float getAverageServiceActivity() const;
    float getAverageServiceHealth() const;

protected:
    // Initialization
    void createRingGeometry();
    void createRingEffects();
    void setupInteractionZones();

    // Service management internals
    void addServiceInternal(const ServicePosition& servicePos);
    void removeServiceInternal(const std::string& serviceId);
    void updateServicePosition(ServicePosition& servicePos, float deltaTime);
    void updateServiceVisuals(ServicePosition& servicePos, float deltaTime);

    // Layout algorithms
    void arrangeServicesEvenly();
    void arrangeServicesByProperty(std::function<float(const ServicePosition&)> propertyGetter);
    void optimizeServiceSpacing();
    float calculateOptimalAngle(int serviceIndex, int totalServices) const;

    // Ring animation
    void updateRingRotation(float deltaTime);
    void updateRingEffects(float deltaTime);
    void updateServiceAnimations(float deltaTime);

    // Interaction handling
    void handleServiceInteraction(const std::string& serviceId, const vec3& position);
    void handleRingInteraction(const vec3& position);
    std::string findNearestService(const vec3& position) const;

    // Visual updates
    void updateConnectionLines(float deltaTime);
    void updateClusterVisuals(float deltaTime);
    void updateLevelOfDetail();

    // Event dispatching
    void dispatchServiceEvent(const ServiceRingEvent& event);

private:
    // Configuration
    ServiceRingConfig m_config;

    // Ring state
    float m_currentRotation;
    float m_targetRotation;
    float m_rotationSpeed;
    bool m_isRotating;
    bool m_isArranging;
    float m_rotationAnimationTime;
    float m_rotationAnimationDuration;

    // Services
    std::map<std::string, ServicePosition> m_services;
    std::map<std::string, std::shared_ptr<ServiceVisualization>> m_serviceVisualizations;
    std::string m_selectedServiceId;

    // Clustering
    bool m_clusteringEnabled;
    std::map<std::string, std::shared_ptr<ServiceCluster>> m_clusters;
    std::map<std::string, std::string> m_serviceToCluster;

    // Visual components
    std::shared_ptr<EnergyRing> m_ringBase;
    std::shared_ptr<ParticleEmitter> m_ringParticles;
    std::vector<std::shared_ptr<ConnectionBeam>> m_connectionLines;
    std::map<std::pair<std::string, std::string>, std::shared_ptr<ConnectionBeam>> m_serviceConnections;

    // Ring effects
    float m_ringGlowIntensity;
    float m_ringPulsePhase;
    vec3 m_ringColor;
    bool m_showSegmentation;

    // Audio
    std::shared_ptr<AudioEngine> m_audioEngine;
    std::string m_ambientAudioId;

    // Performance
    float m_lodDistance;
    bool m_occlusionEnabled;
    int m_maxVisibleServices;
    bool m_performanceOptimized;

    // Callbacks
    ServiceEventCallback m_serviceEventCallback;
    ServiceInteractionCallback m_serviceInteractionCallback;
    LayoutChangeCallback m_layoutChangeCallback;

    // Helper methods
    float normalizeAngle(float angle) const;
    vec3 angleToPosition(float angle) const;
    float positionToAngle(const vec3& position) const;
    bool isPositionOnRing(const vec3& position, float tolerance = 0.5f) const;
    ServicePosition* findServiceById(const std::string& serviceId);
    const ServicePosition* findServiceById(const std::string& serviceId) const;
    void updateServiceVisualization(const std::string& serviceId);
    void createServiceVisualization(const std::string& serviceId);
    void destroyServiceVisualization(const std::string& serviceId);
};

// ============================================================================
// ServiceRingController - High-level ring management
// ============================================================================

class ServiceRingController {
public:
    ServiceRingController(std::shared_ptr<ServiceRing> serviceRing);
    virtual ~ServiceRingController();

    // Automatic management
    void enableAutoManagement(bool enable);
    void setServiceUpdateInterval(float interval);
    void setAutoArrangementTrigger(int serviceCountThreshold);
    void setActivityThresholds(float low, float medium, float high);

    // Service lifecycle
    void onServiceDiscovered(const std::string& serviceId, const std::string& serviceName, const std::string& serviceType);
    void onServiceStatusUpdate(const std::string& serviceId, float health, float activity);
    void onServiceRemoved(const std::string& serviceId);
    void onServiceError(const std::string& serviceId, const std::string& errorType);

    // Layout automation
    void enableSmartLayout(bool enable);
    void setLayoutAlgorithm(const std::string& algorithm); // "circular", "priority", "type", "activity"
    void setLayoutUpdateFrequency(float frequency);
    void triggerLayoutUpdate();

    // Clustering automation
    void enableAutoClustering(bool enable);
    void setClusteringCriteria(const std::vector<std::string>& criteria); // "type", "activity", "location"
    void setMaxClusterSize(int maxSize);
    void setMinClusterSize(int minSize);

    // Performance management
    void setPerformanceMode(const std::string& mode); // "quality", "balanced", "performance"
    void enableAdaptiveLOD(bool enable);
    void setTargetFrameRate(float fps);

private:
    std::shared_ptr<ServiceRing> m_serviceRing;
    
    // Auto-management state
    bool m_autoManagement;
    float m_serviceUpdateInterval;
    float m_serviceUpdateTimer;
    int m_autoArrangementThreshold;
    float m_lowActivityThreshold;
    float m_mediumActivityThreshold;
    float m_highActivityThreshold;
    
    // Layout automation
    bool m_smartLayoutEnabled;
    std::string m_layoutAlgorithm;
    float m_layoutUpdateFrequency;
    float m_layoutUpdateTimer;
    int m_lastServiceCount;
    
    // Clustering automation
    bool m_autoClusteringEnabled;
    std::vector<std::string> m_clusteringCriteria;
    int m_maxClusterSize;
    int m_minClusterSize;
    float m_clusteringTimer;
    
    // Performance management
    std::string m_performanceMode;
    bool m_adaptiveLODEnabled;
    float m_targetFrameRate;
    float m_currentFrameRate;
    
    // Service tracking
    std::map<std::string, float> m_serviceHealthHistory;
    std::map<std::string, float> m_serviceActivityHistory;
    std::map<std::string, uint64_t> m_serviceLastSeen;
    
    // Internal methods
    void updateServiceMetrics(float deltaTime);
    void updateLayoutIfNeeded();
    void updateClustersIfNeeded();
    void updatePerformanceSettings();
    void performAutoClustering();
    void calculateOptimalLayout();
    bool shouldTriggerLayoutUpdate() const;
    bool shouldUpdateClusters() const;
};

// ============================================================================
// ServiceRingFactory - Factory for creating different types of service rings
// ============================================================================

class ServiceRingFactory {
public:
    // Standard ring configurations
    static std::shared_ptr<ServiceRing> createStandardRing();
    static std::shared_ptr<ServiceRing> createCompactRing();
    static std::shared_ptr<ServiceRing> createExpandedRing();
    static std::shared_ptr<ServiceRing> createHighPerformanceRing();
    
    // Specialized rings
    static std::shared_ptr<ServiceRing> createDatabaseServicesRing();
    static std::shared_ptr<ServiceRing> createAPIServicesRing();
    static std::shared_ptr<ServiceRing> createAIServicesRing();
    static std::shared_ptr<ServiceRing> createMonitoringServicesRing();
    
    // Finalverse-specific rings
    static std::shared_ptr<ServiceRing> createHarmonyServicesRing();
    static std::shared_ptr<ServiceRing> createSongWeaverRing();
    static std::shared_ptr<ServiceRing> createEchoServicesRing();
    
    // Custom configuration
    static std::shared_ptr<ServiceRing> createCustomRing(const ServiceRingConfig& config);
    static ServiceRingConfig createConfigForServiceType(const std::string& serviceType);
    static ServiceRingConfig createConfigForEnvironment(const std::string& environment);

private:
    static ServiceRingConfig getBaseConfig();
    static void applyServiceTypeTheme(ServiceRingConfig& config, const std::string& serviceType);
    static void applyPerformanceOptimizations(ServiceRingConfig& config, const std::string& mode);
};

// ============================================================================
// MultiRingManager - Manages multiple service rings
// ============================================================================

class MultiRingManager : public SceneNode {
public:
    MultiRingManager();
    virtual ~MultiRingManager();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Ring management
    void addRing(const std::string& ringId, std::shared_ptr<ServiceRing> ring);
    void removeRing(const std::string& ringId);
    void clearRings();
    std::shared_ptr<ServiceRing> getRing(const std::string& ringId);
    std::vector<std::string> getRingIds() const;
    int getRingCount() const { return static_cast<int>(m_rings.size()); }

    // Ring positioning
    void setRingPosition(const std::string& ringId, const vec3& position);
    void setRingRotation(const std::string& ringId, const quat& rotation);
    void setRingScale(const std::string& ringId, float scale);
    void arrangeRingsInCircle(float radius, float height = 0.0f);
    void arrangeRingsInGrid(int columns, float spacing);
    void arrangeRingsVertically(float spacing);

    // Inter-ring connections
    void createRingConnection(const std::string& ringA, const std::string& ringB, float intensity = 1.0f);
    void removeRingConnection(const std::string& ringA, const std::string& ringB);
    void clearRingConnections();
    void showAllRingConnections(bool show);

    // Service management across rings
    void moveServiceBetweenRings(const std::string& serviceId, const std::string& fromRing, const std::string& toRing);
    void distributeServicesAcrossRings(const std::vector<std::string>& serviceIds);
    std::string findServiceRing(const std::string& serviceId) const;
    std::vector<std::string> getAllServices() const;

    // Global operations
    void setGlobalRotationSpeed(float speed);
    void synchronizeRingRotations(bool synchronize);
    void pulseAllRings(float intensity = 1.2f);
    void setGlobalRingColor(const vec3& color);

    // Layout algorithms
    void optimizeRingLayout();
    void balanceRingLoads();
    void groupRingsByServiceType();
    void minimizeInterRingConnections();

    // Event handling
    void onServiceAdded(const std::string& serviceId, const std::string& serviceName, const std::string& serviceType);
    void onServiceRemoved(const std::string& serviceId);
    void onServiceUpdated(const std::string& serviceId, float health, float activity);

private:
    struct RingData {
        std::string ringId;
        std::shared_ptr<ServiceRing> ring;
        vec3 position;
        quat rotation;
        float scale;
        bool visible;
        std::vector<std::string> serviceIds;
    };

    std::map<std::string, RingData> m_rings;
    std::map<std::pair<std::string, std::string>, std::shared_ptr<ConnectionBeam>> m_ringConnections;
    
    // Global state
    bool m_synchronizedRotation;
    float m_globalRotationSpeed;
    vec3 m_globalRingColor;
    
    // Layout state
    bool m_isArranging;
    float m_arrangementProgress;
    std::map<std::string, vec3> m_targetPositions;
    
    // Service distribution
    std::map<std::string, std::string> m_serviceToRing;
    std::map<std::string, std::string> m_serviceTypes;
    
    // Helper methods
    void updateRingPositions(float deltaTime);
    void updateRingConnections(float deltaTime);
    void updateServiceDistribution();
    std::string selectOptimalRingForService(const std::string& serviceId, const std::string& serviceType);
    bool isRingOverloaded(const std::string& ringId) const;
    float calculateRingLoad(const std::string& ringId) const;
    void redistributeServices();
};

// ============================================================================
// ServiceRingMetrics - Analytics and monitoring for service rings
// ============================================================================

class ServiceRingMetrics {
public:
    struct RingMetrics {
        std::string ringId;
        int serviceCount;
        float averageActivity;
        float averageHealth;
        float rotationSpeed;
        int interactionCount;
        float lastInteractionTime;
        std::map<std::string, int> serviceTypeCount;
        std::vector<float> activityHistory;
        std::vector<float> healthHistory;
    };

    ServiceRingMetrics();
    virtual ~ServiceRingMetrics();

    // Metrics collection
    void recordRingState(const std::string& ringId, const ServiceRing& ring);
    void recordServiceInteraction(const std::string& ringId, const std::string& serviceId);
    void recordPerformanceMetrics(const std::string& ringId, float frameTime, int renderCalls);

    // Analytics
    RingMetrics getRingMetrics(const std::string& ringId) const;
    std::vector<RingMetrics> getAllRingMetrics() const;
    float getAverageServiceActivity(const std::string& ringId) const;
    float getAverageServiceHealth(const std::string& ringId) const;
    int getMostActiveRing() const;
    std::vector<std::string> getUnderutilizedRings(float threshold = 0.3f) const;

    // Trends and patterns
    std::vector<float> getActivityTrend(const std::string& ringId, int dataPoints = 100) const;
    std::vector<float> getHealthTrend(const std::string& ringId, int dataPoints = 100) const;
    bool detectActivitySpike(const std::string& ringId, float threshold = 2.0f) const;
    bool detectHealthDegradation(const std::string& ringId, float threshold = 0.2f) const;

    // Recommendations
    std::vector<std::string> getOptimizationRecommendations(const std::string& ringId) const;
    std::string recommendRingForNewService(const std::string& serviceType) const;
    bool shouldCreateNewRing(const std::vector<std::string>& existingRings) const;

    // Reporting
    void generateReport(const std::string& ringId) const;
    void exportMetrics(const std::string& filename) const;
    void clearMetrics(const std::string& ringId = "");

private:
    std::map<std::string, RingMetrics> m_ringMetrics;
    std::map<std::string, std::vector<float>> m_performanceHistory;
    uint64_t m_startTime;
    
    void updateHistory(std::vector<float>& history, float value, size_t maxSize = 1000);
    float calculateTrend(const std::vector<float>& data) const;
    float calculateVariance(const std::vector<float>& data) const;
};

} // namespace FinalStorm