// ============================================================================
// File: FinalStorm/src/Services/Components/EnergyRing.h
// Advanced Circular Energy Effects System for Finalverse Services
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include <memory>
#include <vector>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class RenderContext;

// ============================================================================
// EnergyRing - Rotating energy rings for service visualization
// ============================================================================

class EnergyRing : public SceneNode {
public:
    // Ring types for different visual styles
    enum class RingType {
        BASIC,          // Simple solid ring
        PARTICLE_FLOW,  // Ring with flowing particles
        QUANTUM,        // Quantum field visualization
        NEURAL,         // Neural network style
        MUSICAL,        // Musical/harmonic visualization
        DATA_STREAM,    // Data streaming visualization
        POWER_CORE      // Power/energy core style
    };
    
    // Ring states affecting animation and appearance
    enum class RingState {
        IDLE,           // Minimal activity
        ACTIVE,         // Normal operation
        OVERLOAD,       // High activity/stress
        CRITICAL,       // Critical state
        HARMONIZING,    // Special harmonic state (Finalverse)
        CORRUPTED       // Affected by "Silence"
    };

    EnergyRing();
    virtual ~EnergyRing();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Ring geometry
    void setInnerRadius(float radius);
    void setOuterRadius(float radius);
    void setHeight(float height);
    void setSegments(int segments);
    void setRingType(RingType type);
    
    // Visual properties
    void setColor(const vec3& color);
    void setGlowIntensity(float intensity);
    void setRotationSpeed(float speed);
    void setFlowDirection(float direction); // 1.0 = clockwise, -1.0 = counter-clockwise
    void setRotationAxis(const vec3& axis);
    void setState(RingState state);
    
    // Animation and effects
    void triggerRipple(float intensity = 1.0f);
    void setPulseIntensity(float intensity);
    void setDistortionAmount(float amount);
    void enableHarmonicResonance(bool enable, float frequency = 1.0f);
    void setQuantumFluctuation(float amount);
    void setEnergyLevel(float level); // 0.0 - 1.0
    
    // Musical/Harmonic effects (for Finalverse Song mechanics)
    void setMusicalNote(float frequency);
    void setHarmonyLevel(float harmony); // Affects visual harmony/chaos
    void triggerChord(const std::vector<float>& frequencies);
    void setResonanceWithSong(float resonance);
    
    // Data visualization
    void setDataThroughput(float mbps);
    void setConnectionCount(int connections);
    void setProcessingLoad(float load);
    void showDataBurst(float intensity = 1.5f);
    
    // Advanced effects
    void enableElectrostaticField(bool enable);
    void setMagneticDistortion(float strength);
    void enableQuantumTunneling(bool enable);
    void setTemporalDistortion(float amount); // Time dilation effect
    
    // State queries
    float getInnerRadius() const { return m_innerRadius; }
    float getOuterRadius() const { return m_outerRadius; }
    RingType getType() const { return m_ringType; }
    RingState getState() const { return m_ringState; }
    float getEnergyLevel() const { return m_energyLevel; }

private:
    // Geometry properties
    float m_innerRadius;
    float m_outerRadius;
    float m_height;
    int m_segments;
    RingType m_ringType;
    RingState m_ringState;
    
    // Visual properties
    vec3 m_color;
    float m_glowIntensity;
    float m_rotationSpeed;
    float m_flowDirection;
    vec3 m_rotationAxis;
    
    // Animation state
    float m_rotationPhase;
    float m_pulseIntensity;
    float m_pulsePhase;
    float m_distortionAmount;
    float m_energyLevel;
    
    // Musical/Harmonic properties
    bool m_harmonicResonance;
    float m_harmonicFrequency;
    float m_harmonyLevel;
    float m_resonanceWithSong;
    std::vector<float> m_activeChordFrequencies;
    float m_musicalPhase;
    
    // Data visualization
    float m_dataThroughput;
    int m_connectionCount;
    float m_processingLoad;
    float m_dataBurstIntensity;
    float m_dataBurstTime;
    
    // Advanced effects
    bool m_electrostaticField;
    float m_magneticDistortion;
    bool m_quantumTunneling;
    float m_temporalDistortion;
    float m_quantumFluctuation;
    float m_quantumPhase;
    float m_temporalPhase;
    
    // Rendering components
    std::shared_ptr<class RingMesh> m_ringMesh;
    std::shared_ptr<Material> m_ringMaterial;
    std::shared_ptr<ParticleEmitter> m_flowParticles;
    std::shared_ptr<ParticleEmitter> m_sparkParticles;
    std::shared_ptr<ParticleEmitter> m_glowParticles;
    bool m_geometryDirty;
    
    // Effect systems
    std::vector<std::shared_ptr<class RingRipple>> m_ripples;
    std::shared_ptr<class ElectricField> m_electricField;
    std::shared_ptr<class QuantumField> m_quantumField;
    
    // Private methods
    void createRingGeometry();
    void createRingEffects();
    void createSparkEffects();
    void createQuantumEffects();
    void createHarmonicEffects();
    
    void updateRotation(float deltaTime);
    void updatePulse(float deltaTime);
    void updateDistortion(float deltaTime);
    void updateEffects(float deltaTime);
    void updateMusicalEffects(float deltaTime);
    void updateDataVisualization(float deltaTime);
    void updateQuantumEffects(float deltaTime);
    void updateTemporalEffects(float deltaTime);
    
    void updateMaterialColor();
    void updateParticleEffects();
    void updateElectricField();
    void cleanupExpiredRipples();
    
    void renderRing(RenderContext& context);
    void renderGlow(RenderContext& context);
    void renderElectricField(RenderContext& context);
    void renderQuantumField(RenderContext& context);
    void renderDataFlowIndicators(RenderContext& context);
    
    vec4 calculateStateColor() const;
    float calculateStateIntensityMultiplier() const;
    vec3 calculateDistortedPosition(const vec3& basePos, float time) const;
    float calculateHarmonicModulation(float time) const;
};

// ============================================================================
// RingMesh - Specialized mesh for rendering energy rings
// ============================================================================

class RingMesh {
public:
    RingMesh();
    ~RingMesh();
    
    void setInnerRadius(float inner);
    void setOuterRadius(float outer);
    void setHeight(float height);
    void setSegments(int segments);
    void setDistortion(float amount);
    void setQuantumFluctuation(float amount);
    
    void build();
    void rebuild();
    void render(RenderContext& context);
    
    // Advanced geometry features
    void enableTessellation(bool enable);
    void setTessellationLevel(int level);
    void applyHarmonicDistortion(float frequency, float amplitude);
    void applyQuantumDisplacement(float time);
    
private:
    float m_innerRadius;
    float m_outerRadius;
    float m_height;
    int m_segments;
    float m_distortion;
    float m_quantumFluctuation;
    bool m_tessellationEnabled;
    int m_tessellationLevel;
    
    std::vector<vec3> m_vertices;
    std::vector<vec3> m_normals;
    std::vector<vec2> m_uvs;
    std::vector<uint32_t> m_indices;
    
    // Metal buffer objects
    id<MTLBuffer> m_vertexBuffer;
    id<MTLBuffer> m_indexBuffer;
    bool m_buffersDirty;
    
    void generateVertices();
    void generateIndices();
    void calculateNormals();
    void updateBuffers();
    
    vec3 calculateHarmonicPosition(const vec3& basePos, float time) const;
    vec3 calculateQuantumDisplacement(const vec3& basePos, float time) const;
};

// ============================================================================
// RingRipple - Expanding energy ripple effect
// ============================================================================

class RingRipple : public SceneNode {
public:
    RingRipple();
    virtual ~RingRipple();
    
    void setRadius(float radius);
    void setIntensity(float intensity);
    void setColor(const vec3& color);
    void setDuration(float duration);
    void setExpansionSpeed(float speed);
    void setFadeInTime(float time);
    void setFadeOutTime(float time);
    
    bool isExpired() const { return m_isExpired; }
    float getProgress() const { return m_time / m_duration; }
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    float m_radius;
    float m_currentRadius;
    float m_intensity;
    vec3 m_color;
    float m_duration;
    float m_time;
    float m_expansionSpeed;
    float m_fadeInTime;
    float m_fadeOutTime;
    bool m_isExpired;
    
    std::shared_ptr<RingMesh> m_rippleMesh;
    std::shared_ptr<Material> m_rippleMaterial;
    
    void updateRipple(float deltaTime);
    float calculateAlpha() const;
};

// ============================================================================
// ElectricField - Electrostatic field visualization
// ============================================================================

class ElectricField : public SceneNode {
public:
    ElectricField();
    virtual ~ElectricField();
    
    void setFieldStrength(float strength);
    void setFieldRadius(float radius);
    void setDischargeRate(float rate);
    void enableLightning(bool enable);
    void setLightningIntensity(float intensity);
    
    void triggerDischarge(float intensity = 1.0f);
    void addChargePoint(const vec3& position, float charge);
    void clearChargePoints();
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    float m_fieldStrength;
    float m_fieldRadius;
    float m_dischargeRate;
    bool m_lightningEnabled;
    float m_lightningIntensity;
    
    struct ChargePoint {
        vec3 position;
        float charge;
        float life;
    };
    std::vector<ChargePoint> m_chargePoints;
    
    struct LightningBolt {
        std::vector<vec3> points;
        float intensity;
        float life;
        float maxLife;
    };
    std::vector<LightningBolt> m_lightningBolts;
    
    std::shared_ptr<ParticleEmitter> m_fieldParticles;
    float m_dischargeTime;
    
    void updateChargePoints(float deltaTime);
    void updateLightning(float deltaTime);
    void generateLightningBolt(const vec3& start, const vec3& end);
    void renderLightningBolts(RenderContext& context);
    void renderFieldLines(RenderContext& context);
};

// ============================================================================
// QuantumField - Quantum field visualization effects
// ============================================================================

class QuantumField : public SceneNode {
public:
    QuantumField();
    virtual ~QuantumField();
    
    void setFieldDensity(float density);
    void setFluctuationRate(float rate);
    void setTunnelingProbability(float probability);
    void setCoherenceLevel(float coherence);
    void enableWaveFunction(bool enable);
    void setQuantumState(int state);
    
    void collapseWaveFunction();
    void entangleWithField(std::shared_ptr<QuantumField> other);
    void measureQuantumState();
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    float m_fieldDensity;
    float m_fluctuationRate;
    float m_tunnelingProbability;
    float m_coherenceLevel;
    bool m_waveFunctionEnabled;
    int m_quantumState;
    bool m_isCollapsed;
    
    struct QuantumParticle {
        vec3 position;
        vec3 momentum;
        float probability;
        int state;
        bool tunneling;
    };
    std::vector<QuantumParticle> m_quantumParticles;
    
    std::shared_ptr<ParticleEmitter> m_fluctuationParticles;
    std::weak_ptr<QuantumField> m_entangledField;
    
    float m_fluctuationPhase;
    float m_collapseTime;
    
    void updateQuantumParticles(float deltaTime);
    void updateWaveFunction(float deltaTime);
    void updateEntanglement(float deltaTime);
    void renderWaveFunction(RenderContext& context);
    void renderQuantumParticles(RenderContext& context);
    
    vec3 calculateWaveFunctionAmplitude(const vec3& position, float time) const;
    float calculateTunnelingProbability(const QuantumParticle& particle) const;
};

// ============================================================================
// RingCluster - Multiple coordinated energy rings
// ============================================================================

class RingCluster : public SceneNode {
public:
    struct RingConfig {
        float radius;
        float height;
        vec3 color;
        float rotationSpeed;
        EnergyRing::RingType type;
        vec3 rotationAxis;
    };
    
    RingCluster();
    virtual ~RingCluster();
    
    void addRing(const RingConfig& config);
    void removeRing(int index);
    void clearRings();
    
    void setClusterIntensity(float intensity);
    void setClusterColor(const vec3& color);
    void setGlobalRotationSpeed(float speed);
    void enableSynchronization(bool enable);
    void setSynchronizationPhase(float phase);
    
    // Cluster effects
    void triggerClusterPulse(float intensity = 1.5f);
    void enableResonance(bool enable);
    void setResonanceFrequency(float frequency);
    void createInterference(float strength);
    
    // Service visualization
    void setServiceLoad(float load);
    void setServiceState(const std::string& state);
    void showServiceActivity(float activity);
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    std::vector<std::shared_ptr<EnergyRing>> m_rings;
    std::vector<RingConfig> m_ringConfigs;
    
    float m_clusterIntensity;
    vec3 m_clusterColor;
    float m_globalRotationSpeed;
    bool m_synchronizationEnabled;
    float m_synchronizationPhase;
    
    bool m_resonanceEnabled;
    float m_resonanceFrequency;
    float m_interferenceStrength;
    float m_serviceLoad;
    std::string m_serviceState;
    float m_serviceActivity;
    
    float m_clusterPhase;
    float m_resonancePhase;
    
    void updateSynchronization(float deltaTime);
    void updateResonance(float deltaTime);
    void updateServiceVisualization(float deltaTime);
    void updateInterference(float deltaTime);
    
    void applyClusterIntensity();
    void applyClusterColor();
    void applySynchronization();
    void applyResonance();
    
    float calculateRingPhaseOffset(int ringIndex) const;
    vec3 calculateInterferenceColor(int ringIndex, float time) const;
};

} // namespace FinalStorm