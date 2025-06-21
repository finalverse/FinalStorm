// ============================================================================
// File: FinalStorm/src/Scene/Scenes/CentralNexus.h
// Central Nexus Scene Component - The Heart of Finalverse
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "Services/Components/EnergyRing.h"
#include "Services/Components/ConnectionBeam.h"
#include "UI/InteractiveOrb.h"
#include <memory>
#include <vector>
#include <functional>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class RenderContext;
class AudioEngine;

// ============================================================================
// Nexus State - Represents the current state of the central nexus
// ============================================================================

enum class NexusState {
    DORMANT,        // Nexus is inactive
    INITIALIZING,   // Nexus is starting up
    ACTIVE,         // Normal operation
    OVERCHARGED,    // High activity/energy
    HARMONIZING,    // Special Finalverse "Song" state
    CORRUPTED,      // Affected by "Silence"
    CRITICAL        // Emergency state
};

// ============================================================================
// Nexus Configuration
// ============================================================================

struct NexusConfig {
    // Physical properties
    float coreRadius = 0.8f;
    float primaryRingRadius = 3.0f;
    float secondaryRingRadius = 2.0f;
    float tertiaryRingRadius = 1.0f;
    float pillarHeight = 8.0f;
    int pillarCount = 6;
    
    // Visual properties
    vec3 coreColor = make_vec3(1.0f, 1.0f, 1.0f);
    vec3 primaryRingColor = make_vec3(0.3f, 0.7f, 1.0f);
    vec3 secondaryRingColor = make_vec3(0.5f, 0.3f, 1.0f);
    vec3 tertiaryRingColor = make_vec3(1.0f, 0.4f, 0.2f);
    vec3 pillarColor = make_vec3(0.4f, 0.8f, 1.0f);
    
    // Animation properties
    float primaryRingSpeed = 0.5f;
    float secondaryRingSpeed = -0.8f;
    float tertiaryRingSpeed = 1.5f;
    float coreGlowIntensity = 1.5f;
    float ringGlowIntensity = 1.0f;
    
    // Interaction properties
    bool enableCoreInteraction = true;
    bool enableRingInteraction = true;
    bool enablePillarInteraction = false;
    float interactionRadius = 2.0f;
    
    // Audio properties
    bool enableAudio = true;
    float ambientVolume = 0.6f;
    float interactionVolume = 0.8f;
    std::string ambientSound = "nexus_hum";
    std::string activationSound = "nexus_activate";
};

// ============================================================================
// Nexus Event Types
// ============================================================================

enum class NexusEventType {
    CORE_ACTIVATED,
    RING_ACTIVATED,
    PILLAR_ACTIVATED,
    STATE_CHANGED,
    HARMONY_PULSE,
    ENERGY_SURGE,
    SYSTEM_ALERT
};

struct NexusEvent {
    NexusEventType type;
    vec3 position;
    float intensity;
    std::string data;
    uint64_t timestamp;
};

// ============================================================================
// CentralNexus - The Heart of the Finalverse System
// ============================================================================

class CentralNexus : public SceneNode {
public:
    using StateChangeCallback = std::function<void(NexusState oldState, NexusState newState)>;
    using InteractionCallback = std::function<void(const NexusEvent& event)>;
    using HarmonyCallback = std::function<void(float harmonyLevel)>;

    CentralNexus(const NexusConfig& config = NexusConfig{});
    virtual ~CentralNexus();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;
    void onInteraction(const vec3& worldPosition, const std::string& interactionType) override;

    // Configuration
    void setConfig(const NexusConfig& config);
    const NexusConfig& getConfig() const { return m_config; }
    void applyConfig();

    // State management
    void setState(NexusState newState);
    NexusState getState() const { return m_currentState; }
    void setTransitionDuration(float duration) { m_transitionDuration = duration; }
    bool isTransitioning() const { return m_isTransitioning; }
    float getStateProgress() const { return m_stateProgress; }

    // Energy and activity
    void setEnergyLevel(float level); // 0.0 - 1.0
    float getEnergyLevel() const { return m_energyLevel; }
    void setActivityLevel(float level); // 0.0 - 1.0
    float getActivityLevel() const { return m_activityLevel; }
    void setHarmonyLevel(float level); // 0.0 - 1.0 (Finalverse-specific)
    float getHarmonyLevel() const { return m_harmonyLevel; }

    // Nexus components access
    std::shared_ptr<InteractiveOrb> getCore() const { return m_nexusCore; }
    std::shared_ptr<EnergyRing> getPrimaryRing() const { return m_primaryRing; }
    std::shared_ptr<EnergyRing> getSecondaryRing() const { return m_secondaryRing; }
    std::shared_ptr<EnergyRing> getTertiaryRing() const { return m_tertiaryRing; }
    const std::vector<std::shared_ptr<ConnectionBeam>>& getPillars() const { return m_energyPillars; }

    // Visual effects
    void triggerPulse(float intensity = 1.5f);
    void triggerRipple(float intensity = 1.0f);
    void triggerHarmonyBurst(float intensity = 2.0f);
    void triggerEnergyWave(const vec3& direction, float intensity = 1.0f);
    void showSystemAlert(const vec3& color, float duration = 3.0f);

    // Ring-specific operations
    void activateRing(int ringIndex, float intensity = 1.0f);
    void deactivateRing(int ringIndex);
    void setRingColor(int ringIndex, const vec3& color);
    void setRingSpeed(int ringIndex, float speed);
    void synchronizeRings(bool enable);
    void setRingSynchronizationPhase(float phase);

    // Pillar operations
    void activatePillar(int pillarIndex, float intensity = 1.0f);
    void deactivatePillar(int pillarIndex);
    void activateAllPillars(float intensity = 1.0f);
    void deactivateAllPillars();
    void setPillarFlowDirection(int pillarIndex, float direction); // 1.0 = up, -1.0 = down
    void createPillarConnectionTo(int pillarIndex, const vec3& targetPosition);

    // Audio integration
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine);
    void playAmbientAudio();
    void stopAmbientAudio();
    void playActivationSound();
    void playHarmonySound(float harmonyLevel);

    // Service integration
    void setServiceCount(int count);
    void setServiceActivity(const std::vector<float>& activities);
    void showServiceConnection(int serviceIndex, const vec3& servicePosition, float intensity = 1.0f);
    void hideServiceConnection(int serviceIndex);
    void updateServiceConnections(const std::vector<vec3>& servicePositions, const std::vector<float>& activities);

    // Event system
    void setStateChangeCallback(StateChangeCallback callback) { m_stateChangeCallback = callback; }
    void setInteractionCallback(InteractionCallback callback) { m_interactionCallback = callback; }
    void setHarmonyCallback(HarmonyCallback callback) { m_harmonyCallback = callback; }
    void clearCallbacks();

    // Metrics and monitoring
    float getCoreTemperature() const; // Visual effect intensity
    float getRingStability() const; // How stable the ring animations are
    float getPillarEfficiency() const; // How efficiently pillars are operating
    vec3 getDominantEnergyColor() const; // The current dominant color of the nexus
    float getResonanceFrequency() const; // For audio/visual synchronization

    // Advanced effects
    void enableQuantumEffects(bool enable);
    void setQuantumFluctuation(float amount);
    void enableTemporalDistortion(bool enable);
    void setTemporalDistortionAmount(float amount);
    void enableHolographicNoise(bool enable);
    void setHolographicNoiseAmount(float amount);

    // Finalverse-specific features
    void setSongResonance(float resonance); // Connection to the "Song of Creation"
    void setSilenceCorruption(float corruption); // Effect of the "Great Silence"
    void playEchoHarmony(const std::vector<float>& frequencies); // Play harmonic frequencies
    void triggerSongWeaving(const vec3& direction, float power); // Songweaver ability effect
    void showSilenceRift(const vec3& position, float size = 1.0f); // Visualize Silence corruption

protected:
    // Initialization methods
    void createNexusCore();
    void createEnergyRings();
    void createEnergyPillars();
    void createParticleSystems();
    void createConnectionSystems();
    void setupInteractionZones();

    // Update methods
    void updateState(float deltaTime);
    void updateEnergyFlow(float deltaTime);
    void updateRingAnimations(float deltaTime);
    void updatePillarEffects(float deltaTime);
    void updateParticleEffects(float deltaTime);
    void updateAudioEffects(float deltaTime);
    void updateServiceConnections(float deltaTime);

    // State transition methods
    void beginStateTransition(NexusState newState);
    void updateStateTransition(float deltaTime);
    void completeStateTransition();
    void applyStateEffects(NexusState state, float intensity);

    // Visual effect methods
    void updateCoreEffects(float deltaTime);
    void updateRingEffects(float deltaTime);
    void updatePillarEffects(float deltaTime);
    void updateQuantumEffects(float deltaTime);
    void updateHarmonyEffects(float deltaTime);

    // Interaction handling
    void handleCoreInteraction(const vec3& position);
    void handleRingInteraction(int ringIndex, const vec3& position);
    void handlePillarInteraction(int pillarIndex, const vec3& position);

    // Event dispatching
    void dispatchEvent(const NexusEvent& event);
    void dispatchStateChange(NexusState oldState, NexusState newState);
    void dispatchHarmonyChange(float newHarmony);

private:
    // Configuration
    NexusConfig m_config;

    // State management
    NexusState m_currentState;
    NexusState m_targetState;
    bool m_isTransitioning;
    float m_stateProgress;
    float m_transitionDuration;
    float m_stateTime;

    // Energy and activity
    float m_energyLevel;
    float m_activityLevel;
    float m_harmonyLevel;
    float m_songResonance;
    float m_silenceCorruption;

    // Core components
    std::shared_ptr<InteractiveOrb> m_nexusCore;
    std::shared_ptr<EnergyRing> m_primaryRing;
    std::shared_ptr<EnergyRing> m_secondaryRing;
    std::shared_ptr<EnergyRing> m_tertiaryRing;
    std::vector<std::shared_ptr<ConnectionBeam>> m_energyPillars;

    // Particle systems
    std::shared_ptr<ParticleEmitter> m_coreParticles;
    std::shared_ptr<ParticleEmitter> m_energyParticles;
    std::shared_ptr<ParticleEmitter> m_harmonyParticles;
    std::shared_ptr<ParticleEmitter> m_quantumParticles;

    // Service connections
    std::vector<std::shared_ptr<ConnectionBeam>> m_serviceConnections;
    std::vector<vec3> m_servicePositions;
    std::vector<float> m_serviceActivities;

    // Animation state
    float m_corePhase;
    float m_ringPhase;
    float m_pillarPhase;
    float m_harmonyPhase;
    float m_quantumPhase;
    float m_temporalPhase;
    bool m_ringSynchronization;
    float m_synchronizationPhase;

    // Effects state
    bool m_quantumEffectsEnabled;
    float m_quantumFluctuation;
    bool m_temporalDistortionEnabled;
    float m_temporalDistortionAmount;
    bool m_holographicNoiseEnabled;
    float m_holographicNoiseAmount;

    // Audio integration
    std::shared_ptr<AudioEngine> m_audioEngine;
    std::string m_ambientAudioId;
    bool m_audioPlaying;

    // Callbacks
    StateChangeCallback m_stateChangeCallback;
    InteractionCallback m_interactionCallback;
    HarmonyCallback m_harmonyCallback;

    // Interaction zones
    struct InteractionZone {
        vec3 center;
        float radius;
        std::string type;
        int componentIndex;
    };
    std::vector<InteractionZone> m_interactionZones;

    // Metrics cache
    mutable float m_cachedTemperature;
    mutable float m_cachedStability;
    mutable float m_cachedEfficiency;
    mutable vec3 m_cachedDominantColor;
    mutable float m_cachedResonanceFrequency;
    mutable float m_metricsUpdateTime;
    mutable bool m_metricsNeedUpdate;

    // Helper methods
    vec3 calculateRingPosition(int ringIndex) const;
    vec3 calculatePillarPosition(int pillarIndex) const;
    float calculateEnergyIntensity() const;
    vec3 calculateDominantColor() const;
    void updateMetricsCache() const;
    float smoothStep(float edge0, float edge1, float x) const;
    vec3 lerpColor(const vec3& a, const vec3& b, float t) const;
};

// ============================================================================
// NexusController - High-level nexus management
// ============================================================================

class NexusController {
public:
    NexusController(std::shared_ptr<CentralNexus> nexus);
    virtual ~NexusController();

    // High-level control
    void initializeNexus();
    void shutdownNexus();
    void restartNexus();
    void emergencyShutdown();

    // Automatic state management
    void enableAutoStateManagement(bool enable);
    void setSystemLoadThresholds(float low, float medium, float high);
    void setHarmonyThresholds(float critical, float low, float high);
    void updateSystemMetrics(float load, float harmony, int serviceCount);

    // Event-driven updates
    void onServiceAdded(const vec3& position);
    void onServiceRemoved(int serviceIndex);
    void onServiceActivityChanged(int serviceIndex, float activity);
    void onSystemError(const std::string& errorType, float severity);
    void onHarmonyLevelChanged(float newHarmony);

    // Automation
    void enableRingOrchestration(bool enable);
    void setPillarAutoFlow(bool enable);
    void setEnergyAutoBalance(bool enable);
    void setHarmonyAutoResponse(bool enable);

    // Performance optimization
    void setPerformanceMode(const std::string& mode); // "quality", "balanced", "performance"
    void setMaxParticles(int maxParticles);
    void setLODDistance(float distance);
    void enableCulling(bool enable);

private:
    std::shared_ptr<CentralNexus> m_nexus;
    
    // Auto-management state
    bool m_autoStateManagement;
    bool m_ringOrchestration;
    bool m_pillarAutoFlow;
    bool m_energyAutoBalance;
    bool m_harmonyAutoResponse;
    
    // Thresholds
    float m_lowLoadThreshold;
    float m_mediumLoadThreshold;
    float m_highLoadThreshold;
    float m_criticalHarmonyThreshold;
    float m_lowHarmonyThreshold;
    float m_highHarmonyThreshold;
    
    // Current metrics
    float m_currentLoad;
    float m_currentHarmony;
    int m_currentServiceCount;
    
    // Performance settings
    std::string m_performanceMode;
    int m_maxParticles;
    float m_lodDistance;
    bool m_cullingEnabled;
    
    // Internal methods
    void updateAutoState();
    void orchestrateRings();
    void balanceEnergy();
    void respondToHarmony();
    NexusState calculateOptimalState() const;
};

} // namespace FinalStorm