// ============================================================================
// File: FinalStorm/src/Services/Components/EnergyRing.cpp
// Advanced Circular Energy Effects Implementation for Finalverse Services
// ============================================================================

#include "Services/Components/EnergyRing.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace FinalStorm {

// ============================================================================
// EnergyRing Implementation
// ============================================================================

EnergyRing::EnergyRing() 
    : SceneNode("Energy Ring")
    , m_innerRadius(0.8f)
    , m_outerRadius(1.2f)
    , m_height(0.1f)
    , m_segments(32)
    , m_ringType(RingType::BASIC)
    , m_ringState(RingState::IDLE)
    , m_color(make_vec3(0.2f, 0.8f, 1.0f))
    , m_glowIntensity(1.0f)
    , m_rotationSpeed(1.0f)
    , m_flowDirection(1.0f)
    , m_rotationAxis(make_vec3(0, 1, 0))
    , m_rotationPhase(0.0f)
    , m_pulseIntensity(0.0f)
    , m_pulsePhase(0.0f)
    , m_distortionAmount(0.0f)
    , m_energyLevel(0.5f)
    , m_harmonicResonance(false)
    , m_harmonicFrequency(1.0f)
    , m_harmonyLevel(0.5f)
    , m_resonanceWithSong(0.0f)
    , m_musicalPhase(0.0f)
    , m_dataThroughput(0.0f)
    , m_connectionCount(0)
    , m_processingLoad(0.0f)
    , m_dataBurstIntensity(0.0f)
    , m_dataBurstTime(0.0f)
    , m_electrostaticField(false)
    , m_magneticDistortion(0.0f)
    , m_quantumTunneling(false)
    , m_temporalDistortion(0.0f)
    , m_quantumFluctuation(0.0f)
    , m_quantumPhase(0.0f)
    , m_temporalPhase(0.0f)
    , m_geometryDirty(true) {
    
    createRingGeometry();
    createRingEffects();
    
    std::cout << "EnergyRing created with radius " << m_outerRadius << std::endl;
}

EnergyRing::~EnergyRing() = default;

void EnergyRing::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateRotation(deltaTime);
    updatePulse(deltaTime);
    updateDistortion(deltaTime);
    updateEffects(deltaTime);
    updateMusicalEffects(deltaTime);
    updateDataVisualization(deltaTime);
    updateQuantumEffects(deltaTime);
    updateTemporalEffects(deltaTime);
    
    if (m_geometryDirty) {
        m_ringMesh->rebuild();
        m_geometryDirty = false;
    }
}

void EnergyRing::render(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    renderRing(context);
    renderGlow(context);
    
    if (m_electrostaticField) {
        renderElectricField(context);
    }
    
    if (m_quantumTunneling) {
        renderQuantumField(context);
    }
    
    renderDataFlowIndicators(context);
    
    context.popBlendMode();
    context.popTransform();
}

void EnergyRing::setInnerRadius(float radius) {
    m_innerRadius = std::max(0.1f, radius);
    m_geometryDirty = true;
}

void EnergyRing::setOuterRadius(float radius) {
    m_outerRadius = std::max(m_innerRadius + 0.1f, radius);
    m_geometryDirty = true;
}

void EnergyRing::setHeight(float height) {
    m_height = std::max(0.01f, height);
    m_geometryDirty = true;
}

void EnergyRing::setSegments(int segments) {
    m_segments = std::max(8, segments);
    m_geometryDirty = true;
}

void EnergyRing::setRingType(RingType type) {
    if (m_ringType != type) {
        m_ringType = type;
        createRingEffects(); // Recreate effects for new type
        updateMaterialColor();
    }
}

void EnergyRing::setColor(const vec3& color) {
    m_color = color;
    updateMaterialColor();
}

void EnergyRing::setGlowIntensity(float intensity) {
    m_glowIntensity = clamp(intensity, 0.0f, 3.0f);
}

void EnergyRing::setRotationSpeed(float speed) {
    m_rotationSpeed = speed;
}

void EnergyRing::setFlowDirection(float direction) {
    m_flowDirection = direction;
}

void EnergyRing::setRotationAxis(const vec3& axis) {
    m_rotationAxis = normalize(axis);
}

void EnergyRing::setState(RingState state) {
    if (m_ringState != state) {
        m_ringState = state;
        updateMaterialColor();
        
        // Trigger state-specific effects
        switch (state) {
            case RingState::OVERLOAD:
                triggerRipple(2.0f);
                setPulseIntensity(1.5f);
                break;
            case RingState::CRITICAL:
                triggerRipple(3.0f);
                setPulseIntensity(2.5f);
                enableQuantumFlicker(true);
                break;
            case RingState::HARMONIZING:
                enableHarmonicResonance(true, 2.0f);
                setResonanceWithSong(1.0f);
                break;
            case RingState::CORRUPTED:
                setQuantumFluctuation(0.8f);
                setDistortionAmount(0.5f);
                break;
            default:
                break;
        }
    }
}

void EnergyRing::triggerRipple(float intensity) {
    auto ripple = std::make_shared<RingRipple>();
    ripple->setRadius(m_outerRadius);
    ripple->setIntensity(intensity);
    ripple->setColor(m_color);
    ripple->setDuration(2.0f);
    ripple->setExpansionSpeed(2.0f);
    
    m_ripples.push_back(ripple);
    addChild(ripple);
    
    std::cout << "EnergyRing: Ripple triggered with intensity " << intensity << std::endl;
}

void EnergyRing::setPulseIntensity(float intensity) {
    m_pulseIntensity = clamp(intensity, 0.0f, 3.0f);
    m_pulsePhase = 0.0f;
}

void EnergyRing::setDistortionAmount(float amount) {
    m_distortionAmount = clamp(amount, 0.0f, 1.0f);
    if (m_ringMesh) {
        m_ringMesh->setDistortion(m_distortionAmount);
    }
}

void EnergyRing::enableHarmonicResonance(bool enable, float frequency) {
    m_harmonicResonance = enable;
    m_harmonicFrequency = frequency;
    m_musicalPhase = 0.0f;
}

void EnergyRing::setQuantumFluctuation(float amount) {
    m_quantumFluctuation = clamp(amount, 0.0f, 1.0f);
    if (m_ringMesh) {
        m_ringMesh->setQuantumFluctuation(m_quantumFluctuation);
    }
}

void EnergyRing::setEnergyLevel(float level) {
    m_energyLevel = clamp(level, 0.0f, 1.0f);
    
    // Adjust visual properties based on energy level
    setGlowIntensity(0.5f + m_energyLevel * 1.5f);
    setRotationSpeed(0.5f + m_energyLevel * 2.0f);
    
    // Update particle emission rates
    updateParticleEffects();
}

void EnergyRing::setMusicalNote(float frequency) {
    m_harmonicFrequency = frequency;
    enableHarmonicResonance(true, frequency);
}

void EnergyRing::setHarmonyLevel(float harmony) {
    m_harmonyLevel = clamp(harmony, 0.0f, 1.0f);
    
    // High harmony = stable, beautiful patterns
    // Low harmony = chaotic, discordant effects
    if (harmony > 0.7f) {
        setDistortionAmount(0.0f);
        setQuantumFluctuation(0.1f);
    } else if (harmony < 0.3f) {
        setDistortionAmount(0.6f);
        setQuantumFluctuation(0.8f);
    }
}

void EnergyRing::triggerChord(const std::vector<float>& frequencies) {
    m_activeChordFrequencies = frequencies;
    enableHarmonicResonance(true, frequencies.empty() ? 1.0f : frequencies[0]);
    
    // Create harmonic ripples for each frequency
    for (float freq : frequencies) {
        triggerRipple(0.8f + freq * 0.1f);
    }
}

void EnergyRing::setResonanceWithSong(float resonance) {
    m_resonanceWithSong = clamp(resonance, 0.0f, 1.0f);
    
    // High resonance with the Song increases harmony and reduces chaos
    setHarmonyLevel(m_resonanceWithSong);
    
    if (resonance > 0.8f) {
        setState(RingState::HARMONIZING);
    }
}

void EnergyRing::setDataThroughput(float mbps) {
    m_dataThroughput = std::max(0.0f, mbps);
    
    // Adjust flow speed based on throughput
    float normalizedThroughput = clamp(mbps / 100.0f, 0.1f, 3.0f);
    setRotationSpeed(normalizedThroughput);
    
    updateParticleEffects();
}

void EnergyRing::setConnectionCount(int connections) {
    m_connectionCount = std::max(0, connections);
    
    // More connections = more complex patterns
    int dynamicSegments = 16 + (connections * 2);
    setSegments(std::min(64, dynamicSegments));
}

void EnergyRing::setProcessingLoad(float load) {
    m_processingLoad = clamp(load, 0.0f, 1.0f);
    
    // High processing load increases pulse intensity
    setPulseIntensity(m_processingLoad * 2.0f);
    
    // Update ring state based on load
    if (load > 0.9f) {
        setState(RingState::OVERLOAD);
    } else if (load > 0.7f) {
        setState(RingState::ACTIVE);
    } else {
        setState(RingState::IDLE);
    }
}

void EnergyRing::showDataBurst(float intensity) {
    m_dataBurstIntensity = intensity;
    m_dataBurstTime = 0.0f;
    
    // Create burst particles
    if (m_sparkParticles) {
        m_sparkParticles->burst(static_cast<int>(50 * intensity));
    }
    
    triggerRipple(intensity);
    
    std::cout << "EnergyRing: Data burst with intensity " << intensity << std::endl;
}

void EnergyRing::enableElectrostaticField(bool enable) {
    m_electrostaticField = enable;
    
    if (enable && !m_electricField) {
        m_electricField = std::make_shared<ElectricField>();
        m_electricField->setFieldRadius(m_outerRadius * 1.5f);
        addChild(m_electricField);
    }
}

void EnergyRing::setMagneticDistortion(float strength) {
    m_magneticDistortion = clamp(strength, 0.0f, 1.0f);
    setDistortionAmount(m_magneticDistortion * 0.3f);
}

void EnergyRing::enableQuantumTunneling(bool enable) {
    m_quantumTunneling = enable;
    
    if (enable && !m_quantumField) {
        m_quantumField = std::make_shared<QuantumField>();
        m_quantumField->setFieldDensity(0.5f);
        addChild(m_quantumField);
    }
}

void EnergyRing::setTemporalDistortion(float amount) {
    m_temporalDistortion = clamp(amount, 0.0f, 1.0f);
    
    // Temporal distortion affects rotation speed in interesting ways
    float timeScale = 1.0f + m_temporalDistortion * sin(m_temporalPhase) * 0.5f;
    setRotationSpeed(m_rotationSpeed * timeScale);
}

// ============================================================================
// Private Implementation Methods
// ============================================================================

void EnergyRing::createRingGeometry() {
    m_ringMesh = std::make_shared<RingMesh>();
    m_ringMesh->setInnerRadius(m_innerRadius);
    m_ringMesh->setOuterRadius(m_outerRadius);
    m_ringMesh->setHeight(m_height);
    m_ringMesh->setSegments(m_segments);
    m_ringMesh->build();
    
    m_ringMaterial = std::make_shared<Material>("energy_ring");
    updateMaterialColor();
}

void EnergyRing::createRingEffects() {
    // Clear existing effects
    if (m_flowParticles) removeChild(m_flowParticles);
    if (m_sparkParticles) removeChild(m_sparkParticles);
    if (m_glowParticles) removeChild(m_glowParticles);
    
    createSparkEffects();
    
    switch (m_ringType) {
        case RingType::PARTICLE_FLOW:
            createParticleFlowEffects();
            break;
        case RingType::QUANTUM:
            createQuantumEffects();
            break;
        case RingType::NEURAL:
            createNeuralEffects();
            break;
        case RingType::MUSICAL:
            createHarmonicEffects();
            break;
        case RingType::DATA_STREAM:
            createDataStreamEffects();
            break;
        case RingType::POWER_CORE:
            createPowerCoreEffects();
            break;
        default:
            break;
    }
}

void EnergyRing::createSparkEffects() {
    ParticleEmitter::Config sparkConfig;
    sparkConfig.emitShape = ParticleEmitter::Shape::RING;
    sparkConfig.emitRate = 5.0f;
    sparkConfig.particleLifetime = 1.5f;
    sparkConfig.startSize = 0.02f;
    sparkConfig.endSize = 0.08f;
    sparkConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 1.0f);
    sparkConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    sparkConfig.velocity = 0.5f;
    sparkConfig.spread = 0.3f;
    
    m_sparkParticles = std::make_shared<ParticleEmitter>(sparkConfig);
    m_sparkParticles->setName("Ring Sparks");
    m_sparkParticles->setEmitRadius(m_outerRadius);
    addChild(m_sparkParticles);
}

void EnergyRing::createParticleFlowEffects() {
    ParticleEmitter::Config flowConfig;
    flowConfig.emitShape = ParticleEmitter::Shape::RING;
    flowConfig.emitRate = 30.0f;
    flowConfig.particleLifetime = 2.0f;
    flowConfig.startSize = 0.01f;
    flowConfig.endSize = 0.03f;
    flowConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.8f);
    flowConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    flowConfig.velocity = 1.0f;
    flowConfig.inheritVelocity = 0.9f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(flowConfig);
    m_flowParticles->setName("Ring Flow");
    m_flowParticles->setEmitRadius((m_innerRadius + m_outerRadius) * 0.5f);
    addChild(m_flowParticles);
}

void EnergyRing::createQuantumEffects() {
    enableQuantumTunneling(true);
    setQuantumFluctuation(0.3f);
    
    ParticleEmitter::Config quantumConfig;
    quantumConfig.emitShape = ParticleEmitter::Shape::RING;
    quantumConfig.emitRate = 15.0f;
    quantumConfig.particleLifetime = 3.0f;
    quantumConfig.startSize = 0.005f;
    quantumConfig.endSize = 0.02f;
    quantumConfig.startColor = make_vec4(0.8f, 0.2f, 1.0f, 1.0f); // Purple quantum color
    quantumConfig.endColor = make_vec4(0.8f, 0.2f, 1.0f, 0.0f);
    quantumConfig.velocity = 0.3f;
    quantumConfig.spread = 0.8f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(quantumConfig);
    m_flowParticles->setName("Quantum Flow");
    addChild(m_flowParticles);
}

void EnergyRing::createNeuralEffects() {
    ParticleEmitter::Config neuralConfig;
    neuralConfig.emitShape = ParticleEmitter::Shape::RING;
    neuralConfig.emitRate = 20.0f;
    neuralConfig.particleLifetime = 1.0f;
    neuralConfig.startSize = 0.008f;
    neuralConfig.endSize = 0.02f;
    neuralConfig.startColor = make_vec4(0.2f, 1.0f, 0.6f, 1.0f); // Neural green
    neuralConfig.endColor = make_vec4(0.2f, 1.0f, 0.6f, 0.0f);
    neuralConfig.velocity = 2.0f;
    neuralConfig.burst = true;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(neuralConfig);
    m_flowParticles->setName("Neural Pulses");
    addChild(m_flowParticles);
}

void EnergyRing::createHarmonicEffects() {
    enableHarmonicResonance(true, 1.0f);
    
    ParticleEmitter::Config harmonicConfig;
    harmonicConfig.emitShape = ParticleEmitter::Shape::RING;
    harmonicConfig.emitRate = 25.0f;
    harmonicConfig.particleLifetime = 2.5f;
    harmonicConfig.startSize = 0.015f;
    harmonicConfig.endSize = 0.04f;
    harmonicConfig.startColor = make_vec4(1.0f, 0.8f, 0.2f, 0.9f); // Golden harmonic color
    harmonicConfig.endColor = make_vec4(1.0f, 0.8f, 0.2f, 0.0f);
    harmonicConfig.velocity = 0.8f;
    harmonicConfig.inheritVelocity = 1.0f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(harmonicConfig);
    m_flowParticles->setName("Harmonic Resonance");
    addChild(m_flowParticles);
}

void EnergyRing::createDataStreamEffects() {
    ParticleEmitter::Config dataConfig;
    dataConfig.emitShape = ParticleEmitter::Shape::RING;
    dataConfig.emitRate = 40.0f;
    dataConfig.particleLifetime = 1.2f;
    dataConfig.startSize = 0.005f;
    dataConfig.endSize = 0.015f;
    dataConfig.startColor = make_vec4(0.4f, 0.8f, 1.0f, 1.0f); // Data cyan
    dataConfig.endColor = make_vec4(0.4f, 0.8f, 1.0f, 0.0f);
    dataConfig.velocity = 3.0f;
    dataConfig.directional = true;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(dataConfig);
    m_flowParticles->setName("Data Stream");
    addChild(m_flowParticles);
}

void EnergyRing::createPowerCoreEffects() {
    enableElectrostaticField(true);
    
    ParticleEmitter::Config coreConfig;
    coreConfig.emitShape = ParticleEmitter::Shape::RING;
    coreConfig.emitRate = 50.0f;
    coreConfig.particleLifetime = 0.8f;
    coreConfig.startSize = 0.02f;
    coreConfig.endSize = 0.06f;
    coreConfig.startColor = make_vec4(1.0f, 0.4f, 0.1f, 1.0f); // Energy orange
    coreConfig.endColor = make_vec4(1.0f, 0.4f, 0.1f, 0.0f);
    coreConfig.velocity = 1.5f;
    coreConfig.gravity = -0.5f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(coreConfig);
    m_flowParticles->setName("Power Core");
    addChild(m_flowParticles);
}

void EnergyRing::updateRotation(float deltaTime) {
    m_rotationPhase += deltaTime * m_rotationSpeed * m_flowDirection;
    
    // Apply rotation to the ring
    quat rotation = quat_from_axis_angle(m_rotationAxis, m_rotationPhase);
    setRotation(rotation);
}

void EnergyRing::updatePulse(float deltaTime) {
    if (m_pulseIntensity > 0.0f) {
        m_pulsePhase += deltaTime * 4.0f; // Pulse frequency
        
        // Natural decay of pulse intensity
        float decayRate = 1.5f;
        m_pulseIntensity = std::max(0.0f, m_pulseIntensity - deltaTime * decayRate);
    }
}

void EnergyRing::updateDistortion(float deltaTime) {
    if (m_distortionAmount > 0.0f && m_ringMesh) {
        // Apply time-varying distortion
        float distortionPhase = m_rotationPhase * 2.0f;
        m_ringMesh->applyHarmonicDistortion(2.0f, m_distortionAmount * sin(distortionPhase));
    }
}

void EnergyRing::updateEffects(float deltaTime) {
    cleanupExpiredRipples();
    updateParticleEffects();
    updateElectricField();
}

void EnergyRing::updateMusicalEffects(float deltaTime) {
    if (m_harmonicResonance) {
        m_musicalPhase += deltaTime * m_harmonicFrequency * 2.0f * M_PI;
        
        // Apply harmonic modulation to various effects
        float harmonicModulation = calculateHarmonicModulation(m_musicalPhase);
        
        // Modulate glow intensity with harmonic frequency
        float modulatedGlow = m_glowIntensity * (1.0f + harmonicModulation * 0.3f);
        
        // Update particle emission based on harmony
        if (m_flowParticles) {
            float baseRate = 20.0f;
            float modulatedRate = baseRate * (1.0f + harmonicModulation * 0.5f);
            m_flowParticles->setEmitRate(modulatedRate);
        }
        
        // Chord harmonics
        if (!m_activeChordFrequencies.empty()) {
            for (size_t i = 0; i < m_activeChordFrequencies.size(); ++i) {
                float freq = m_activeChordFrequencies[i];
                float chordPhase = m_musicalPhase * freq / m_harmonicFrequency;
                // Additional harmonic effects could be applied here
            }
        }
    }
}

void EnergyRing::updateDataVisualization(float deltaTime) {
    // Update data burst effect
    if (m_dataBurstIntensity > 0.0f) {
        m_dataBurstTime += deltaTime;
        
        // Decay burst intensity over time
        float burstDuration = 1.0f;
        if (m_dataBurstTime >= burstDuration) {
            m_dataBurstIntensity = 0.0f;
            m_dataBurstTime = 0.0f;
        } else {
            float t = m_dataBurstTime / burstDuration;
            m_dataBurstIntensity *= (1.0f - t);
        }
    }
    
    // Adjust effects based on data throughput
    if (m_dataThroughput > 0.0f && m_flowParticles) {
        float throughputMultiplier = clamp(m_dataThroughput / 50.0f, 0.5f, 3.0f);
        m_flowParticles->setEmitRate(20.0f * throughputMultiplier);
        
        // Higher throughput = faster flow
        m_flowParticles->setVelocity(1.0f * throughputMultiplier);
    }
    
    // Connection count affects visual complexity
    if (m_connectionCount > 0) {
        // More connections = more sparks
        if (m_sparkParticles) {
            float sparkRate = 5.0f + (m_connectionCount * 2.0f);
            m_sparkParticles->setEmitRate(std::min(sparkRate, 50.0f));
        }
    }
}

void EnergyRing::updateQuantumEffects(float deltaTime) {
    if (m_quantumFluctuation > 0.0f) {
        m_quantumPhase += deltaTime * 8.0f; // Fast quantum fluctuation
        
        // Apply quantum displacement to mesh
        if (m_ringMesh) {
            m_ringMesh->applyQuantumDisplacement(m_quantumPhase);
        }
    }
    
    if (m_quantumTunneling && m_quantumField) {
        // Update quantum field parameters based on ring state
        float coherence = 1.0f - m_quantumFluctuation;
        m_quantumField->setCoherenceLevel(coherence);
        
        if (m_energyLevel > 0.8f) {
            m_quantumField->setTunnelingProbability(0.8f);
        } else {
            m_quantumField->setTunnelingProbability(0.3f);
        }
    }
}

void EnergyRing::updateTemporalEffects(float deltaTime) {
    if (m_temporalDistortion > 0.0f) {
        m_temporalPhase += deltaTime * 3.0f;
        
        // Temporal distortion affects time flow around the ring
        float timeModulation = 1.0f + m_temporalDistortion * sin(m_temporalPhase) * 0.3f;
        
        // Apply time dilation to particle systems
        if (m_flowParticles) {
            m_flowParticles->setTimeScale(timeModulation);
        }
        
        if (m_sparkParticles) {
            m_sparkParticles->setTimeScale(timeModulation);
        }
    }
}

void EnergyRing::updateMaterialColor() {
    if (!m_ringMaterial) return;
    
    vec4 stateColor = calculateStateColor();
    vec3 finalColor = lerp(m_color, make_vec3(stateColor.x, stateColor.y, stateColor.z), 0.4f);
    
    m_ringMaterial->setAlbedo(finalColor);
    m_ringMaterial->setEmission(finalColor);
    
    float finalIntensity = m_glowIntensity * calculateStateIntensityMultiplier();
    
    // Add pulse modulation
    if (m_pulseIntensity > 0.0f) {
        finalIntensity += m_pulseIntensity * sin(m_pulsePhase);
    }
    
    // Add harmonic modulation
    if (m_harmonicResonance) {
        finalIntensity *= (1.0f + calculateHarmonicModulation(m_musicalPhase) * 0.2f);
    }
    
    m_ringMaterial->setEmissionStrength(finalIntensity);
    m_ringMaterial->setAlpha(stateColor.w);
}

void EnergyRing::updateParticleEffects() {
    float stateMultiplier = calculateStateIntensityMultiplier();
    
    if (m_flowParticles) {
        // Update flow particles based on energy level and state
        float baseRate = 20.0f;
        float finalRate = baseRate * m_energyLevel * stateMultiplier;
        m_flowParticles->setEmitRate(finalRate);
        
        // Update particle color
        vec4 particleColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.8f * stateMultiplier);
        m_flowParticles->setParticleColor(particleColor);
    }
    
    if (m_sparkParticles) {
        float sparkRate = 5.0f * m_energyLevel * stateMultiplier;
        m_sparkParticles->setEmitRate(sparkRate);
    }
    
    if (m_glowParticles) {
        float glowRate = 10.0f * m_energyLevel;
        m_glowParticles->setEmitRate(glowRate);
    }
}

void EnergyRing::updateElectricField() {
    if (m_electrostaticField && m_electricField) {
        float fieldStrength = m_energyLevel * calculateStateIntensityMultiplier();
        m_electricField->setFieldStrength(fieldStrength);
        
        // More discharge activity when energy is high
        if (m_energyLevel > 0.7f) {
            m_electricField->setDischargeRate(5.0f * m_energyLevel);
            m_electricField->enableLightning(true);
        } else {
            m_electricField->setDischargeRate(1.0f);
            m_electricField->enableLightning(false);
        }
    }
}

void EnergyRing::cleanupExpiredRipples() {
    auto it = std::remove_if(m_ripples.begin(), m_ripples.end(),
        [this](const std::shared_ptr<RingRipple>& ripple) {
            if (ripple && ripple->isExpired()) {
                removeChild(ripple);
                return true;
            }
            return false;
        });
    
    if (it != m_ripples.end()) {
        m_ripples.erase(it, m_ripples.end());
    }
}

void EnergyRing::renderRing(RenderContext& context) {
    if (!m_ringMesh) return;
    
    // Apply current material properties
    updateMaterialColor();
    
    // Set up rendering state
    context.setMaterial(m_ringMaterial);
    
    // Apply additional effects based on state
    if (m_dataBurstIntensity > 0.0f) {
        float burstGlow = m_dataBurstIntensity * 2.0f;
        context.setEmissionMultiplier(1.0f + burstGlow);
    }
    
    // Render the ring mesh
    m_ringMesh->render(context);
    
    // Reset emission multiplier
    context.setEmissionMultiplier(1.0f);
}

void EnergyRing::renderGlow(RenderContext& context) {
    // Render outer glow effect
    float glowIntensity = m_glowIntensity * 0.3f * calculateStateIntensityMultiplier();
    
    if (glowIntensity > 0.1f && m_ringMesh) {
        vec4 glowColor = calculateStateColor();
        glowColor.w = glowIntensity;
        
        context.setColor(glowColor);
        context.setBlendMode(BlendMode::ADDITIVE);
        
        // Scale up the ring slightly for glow effect
        context.pushTransform(mat4_identity());
        context.scale(make_vec3(1.2f, 1.2f, 1.0f));
        
        m_ringMesh->render(context);
        
        context.popTransform();
    }
}

void EnergyRing::renderElectricField(RenderContext& context) {
    // Electric field rendering is handled by the ElectricField component
    // Additional electric arcs around the ring could be rendered here
    
    if (m_energyLevel > 0.8f) {
        // Render electric arcs between random points on the ring
        int arcCount = static_cast<int>(m_energyLevel * 5);
        
        for (int i = 0; i < arcCount; ++i) {
            float angle1 = (rand() / float(RAND_MAX)) * 2.0f * M_PI;
            float angle2 = angle1 + (rand() / float(RAND_MAX)) * M_PI * 0.5f;
            
            vec3 pos1 = make_vec3(cos(angle1) * m_outerRadius, 0, sin(angle1) * m_outerRadius);
            vec3 pos2 = make_vec3(cos(angle2) * m_outerRadius, 0, sin(angle2) * m_outerRadius);
            
            // Draw electric arc
            vec4 arcColor = make_vec4(0.8f, 0.9f, 1.0f, 0.6f);
            context.setColor(arcColor);
            context.drawLightningBolt(pos1, pos2, 0.01f);
        }
    }
}

void EnergyRing::renderQuantumField(RenderContext& context) {
    // Quantum field rendering is handled by the QuantumField component
    // Additional quantum effects could be rendered here
    
    if (m_quantumFluctuation > 0.3f) {
        // Render quantum probability clouds
        int cloudCount = static_cast<int>(m_quantumFluctuation * 10);
        
        for (int i = 0; i < cloudCount; ++i) {
            float angle = (i / float(cloudCount)) * 2.0f * M_PI;
            float radius = m_innerRadius + (rand() / float(RAND_MAX)) * (m_outerRadius - m_innerRadius);
            
            vec3 pos = make_vec3(cos(angle) * radius, 0, sin(angle) * radius);
            
            float alpha = m_quantumFluctuation * 0.3f;
            vec4 cloudColor = make_vec4(0.8f, 0.2f, 1.0f, alpha);
            context.setColor(cloudColor);
            context.drawGlowSphere(0.05f);
        }
    }
}

void EnergyRing::renderDataFlowIndicators(RenderContext& context) {
    if (m_dataThroughput > 1.0f) {
        // Render data flow indicators moving around the ring
        int indicatorCount = static_cast<int>(clamp(m_dataThroughput / 10.0f, 3.0f, 12.0f));
        
        for (int i = 0; i < indicatorCount; ++i) {
            float baseAngle = (i / float(indicatorCount)) * 2.0f * M_PI;
            float flowAngle = baseAngle + m_rotationPhase * m_flowDirection;
            
            float radius = (m_innerRadius + m_outerRadius) * 0.5f;
            vec3 pos = make_vec3(cos(flowAngle) * radius, 0, sin(flowAngle) * radius);
            
            context.pushTransform(mat4_identity());
            context.translate(pos);
            
            vec4 indicatorColor = make_vec4(0.4f, 0.8f, 1.0f, 0.8f);
            context.setColor(indicatorColor);
            context.drawGlowSphere(0.02f);
            
            context.popTransform();
        }
    }
}

vec4 EnergyRing::calculateStateColor() const {
    switch (m_ringState) {
        case RingState::IDLE:
            return make_vec4(0.6f, 0.6f, 0.6f, 0.6f); // Dim gray
        case RingState::ACTIVE:
            return make_vec4(0.2f, 1.0f, 0.4f, 0.8f); // Green
        case RingState::OVERLOAD:
            return make_vec4(1.0f, 0.6f, 0.2f, 1.0f); // Orange
        case RingState::CRITICAL:
            return make_vec4(1.0f, 0.2f, 0.2f, 1.0f); // Red
        case RingState::HARMONIZING:
            return make_vec4(1.0f, 0.8f, 0.2f, 0.9f); // Golden
        case RingState::CORRUPTED:
            return make_vec4(0.6f, 0.2f, 0.8f, 0.7f); // Purple
        default:
            return make_vec4(0.5f, 0.5f, 0.5f, 0.7f); // Neutral
    }
}

float EnergyRing::calculateStateIntensityMultiplier() const {
    switch (m_ringState) {
        case RingState::IDLE:
            return 0.3f;
        case RingState::ACTIVE:
            return 1.0f;
        case RingState::OVERLOAD:
            return 1.5f + 0.5f * sin(m_rotationPhase * 5.0f); // Pulsing
        case RingState::CRITICAL:
            return 2.0f + sin(m_rotationPhase * 8.0f); // Rapid pulsing
        case RingState::HARMONIZING:
            return 1.2f + 0.3f * calculateHarmonicModulation(m_musicalPhase);
        case RingState::CORRUPTED:
            return 0.8f + 0.6f * sin(m_rotationPhase * 3.0f); // Chaotic pulsing
        default:
            return 1.0f;
    }
}

vec3 EnergyRing::calculateDistortedPosition(const vec3& basePos, float time) const {
    vec3 pos = basePos;
    
    // Apply magnetic distortion
    if (m_magneticDistortion > 0.0f) {
        float distortion = sin(time * 2.0f + basePos.x) * m_magneticDistortion * 0.1f;
        pos.y += distortion;
    }
    
    // Apply quantum fluctuation
    if (m_quantumFluctuation > 0.0f) {
        float quantum = sin(time * 15.0f + basePos.x * 10.0f) * cos(time * 12.0f + basePos.z * 8.0f);
        pos = pos + make_vec3(quantum, quantum * 0.5f, quantum) * m_quantumFluctuation * 0.02f;
    }
    
    return pos;
}

float EnergyRing::calculateHarmonicModulation(float time) const {
    if (!m_harmonicResonance) return 0.0f;
    
    float modulation = sin(time);
    
    // Apply harmony level to modulation
    modulation *= m_harmonyLevel;
    
    // Add chord harmonics
    for (float freq : m_activeChordFrequencies) {
        float harmonic = sin(time * freq / m_harmonicFrequency) * 0.3f;
        modulation += harmonic;
    }
    
    // Apply resonance with Song
    modulation *= (0.5f + m_resonanceWithSong * 0.5f);
    
    return clamp(modulation, -1.0f, 1.0f);
}

// ============================================================================
// RingMesh Implementation
// ============================================================================

RingMesh::RingMesh() 
    : m_innerRadius(0.8f)
    , m_outerRadius(1.2f)
    , m_height(0.1f)
    , m_segments(32)
    , m_distortion(0.0f)
    , m_quantumFluctuation(0.0f)
    , m_tessellationEnabled(false)
    , m_tessellationLevel(1)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
    , m_buffersDirty(true) {
}

RingMesh::~RingMesh() = default;

void RingMesh::setInnerRadius(float inner) {
    m_innerRadius = std::max(0.1f, inner);
    m_buffersDirty = true;
}

void RingMesh::setOuterRadius(float outer) {
    m_outerRadius = std::max(m_innerRadius + 0.1f, outer);
    m_buffersDirty = true;
}

void RingMesh::setHeight(float height) {
    m_height = std::max(0.01f, height);
    m_buffersDirty = true;
}

void RingMesh::setSegments(int segments) {
    m_segments = std::max(8, segments);
    m_buffersDirty = true;
}

void RingMesh::setDistortion(float amount) {
    m_distortion = clamp(amount, 0.0f, 1.0f);
    m_buffersDirty = true;
}

void RingMesh::setQuantumFluctuation(float amount) {
    m_quantumFluctuation = clamp(amount, 0.0f, 1.0f);
    m_buffersDirty = true;
}

void RingMesh::build() {
    generateVertices();
    generateIndices();
    calculateNormals();
    updateBuffers();
}

void RingMesh::rebuild() {
    build();
}

void RingMesh::render(RenderContext& context) {
    if (m_buffersDirty) {
        updateBuffers();
        m_buffersDirty = false;
    }
    
    if (!m_vertices.empty() && !m_indices.empty()) {
        context.drawIndexedMesh(m_vertexBuffer, m_indexBuffer, static_cast<int>(m_indices.size()));
    }
}

void RingMesh::enableTessellation(bool enable) {
    m_tessellationEnabled = enable;
    m_buffersDirty = true;
}

void RingMesh::setTessellationLevel(int level) {
    m_tessellationLevel = std::max(1, level);
    m_buffersDirty = true;
}

void RingMesh::applyHarmonicDistortion(float frequency, float amplitude) {
    // Apply harmonic distortion to existing vertices
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        vec3& vertex = m_vertices[i];
        vertex = calculateHarmonicPosition(vertex, frequency * amplitude);
    }
    m_buffersDirty = true;
}

void RingMesh::applyQuantumDisplacement(float time) {
    // Apply quantum displacement to existing vertices
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        vec3& vertex = m_vertices[i];
        vertex = vertex + calculateQuantumDisplacement(vertex, time);
    }
    m_buffersDirty = true;
}

void RingMesh::generateVertices() {
    m_vertices.clear();
    m_uvs.clear();
    
    int totalSegments = m_tessellationEnabled ? m_segments * m_tessellationLevel : m_segments;
    float angleStep = 2.0f * M_PI / totalSegments;
    
    // Generate vertices for top and bottom of the ring
    for (int layer = 0; layer < 2; ++layer) {
        float y = (layer == 0) ? -m_height * 0.5f : m_height * 0.5f;
        
        for (int seg = 0; seg <= totalSegments; ++seg) {
            float angle = seg * angleStep;
            
            // Inner ring vertices
            float innerX = cos(angle) * m_innerRadius;
            float innerZ = sin(angle) * m_innerRadius;
            m_vertices.push_back(make_vec3(innerX, y, innerZ));
            m_uvs.push_back(make_vec2(0.0f, seg / float(totalSegments)));
            
            // Outer ring vertices
            float outerX = cos(angle) * m_outerRadius;
            float outerZ = sin(angle) * m_outerRadius;
            m_vertices.push_back(make_vec3(outerX, y, outerZ));
            m_uvs.push_back(make_vec2(1.0f, seg / float(totalSegments)));
        }
    }
    
    // Apply distortion if enabled
    if (m_distortion > 0.0f) {
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            vec3& vertex = m_vertices[i];
            float angle = atan2(vertex.z, vertex.x);
            float distortionOffset = sin(angle * 3.0f) * m_distortion * 0.1f;
            vertex.y += distortionOffset;
        }
    }
}

void RingMesh::generateIndices() {
    m_indices.clear();
    
    int totalSegments = m_tessellationEnabled ? m_segments * m_tessellationLevel : m_segments;
    int vertsPerLayer = (totalSegments + 1) * 2; // 2 vertices per segment (inner + outer)
    
    // Generate indices for the ring surface
    for (int seg = 0; seg < totalSegments; ++seg) {
        int bottomInner = seg * 2;
        int bottomOuter = seg * 2 + 1;
        int topInner = vertsPerLayer + seg * 2;
        int topOuter = vertsPerLayer + seg * 2 + 1;
        
        int nextBottomInner = (seg + 1) * 2;
        int nextBottomOuter = (seg + 1) * 2 + 1;
        int nextTopInner = vertsPerLayer + (seg + 1) * 2;
        int nextTopOuter = vertsPerLayer + (seg + 1) * 2 + 1;
        
        // Bottom face (inner to outer)
        m_indices.push_back(bottomInner);
        m_indices.push_back(nextBottomInner);
        m_indices.push_back(bottomOuter);
        
        m_indices.push_back(nextBottomInner);
        m_indices.push_back(nextBottomOuter);
        m_indices.push_back(bottomOuter);
        
        // Top face (outer to inner)
        m_indices.push_back(topOuter);
        m_indices.push_back(nextTopOuter);
        m_indices.push_back(topInner);
        
        m_indices.push_back(nextTopOuter);
        m_indices.push_back(nextTopInner);
        m_indices.push_back(topInner);
        
        // Inner wall
        m_indices.push_back(bottomInner);
        m_indices.push_back(topInner);
        m_indices.push_back(nextBottomInner);
        
        m_indices.push_back(topInner);
        m_indices.push_back(nextTopInner);
        m_indices.push_back(nextBottomInner);
        
        // Outer wall
        m_indices.push_back(nextBottomOuter);
        m_indices.push_back(nextTopOuter);
        m_indices.push_back(bottomOuter);
        
        m_indices.push_back(nextTopOuter);
        m_indices.push_back(topOuter);
        m_indices.push_back(bottomOuter);
    }
}

void RingMesh::calculateNormals() {
    m_normals.clear();
    m_normals.resize(m_vertices.size(), vec3_zero());
    
    // Calculate normals for each triangle
    for (size_t i = 0; i < m_indices.size(); i += 3) {
        uint32_t i0 = m_indices[i];
        uint32_t i1 = m_indices[i + 1];
        uint32_t i2 = m_indices[i + 2];
        
        vec3 v0 = m_vertices[i0];
        vec3 v1 = m_vertices[i1];
        vec3 v2 = m_vertices[i2];
        
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        vec3 normal = normalize(cross(edge1, edge2));
        
        m_normals[i0] = m_normals[i0] + normal;
        m_normals[i1] = m_normals[i1] + normal;
        m_normals[i2] = m_normals[i2] + normal;
    }
    
    // Normalize all normals
    for (vec3& normal : m_normals) {
        normal = normalize(normal);
    }
}

void RingMesh::updateBuffers() {
    // In a real implementation, this would update the Metal/Vulkan buffers
    // For now, we'll just mark that the buffers are updated
    
    // Create or update vertex buffer
    if (m_vertexBuffer) {
        // Update existing buffer
    } else {
        // Create new buffer
        // m_vertexBuffer = createVertexBuffer(m_vertices, m_normals, m_uvs);
    }
    
    // Create or update index buffer
    if (m_indexBuffer) {
        // Update existing buffer
    } else {
        // Create new buffer
        // m_indexBuffer = createIndexBuffer(m_indices);
    }
}

vec3 RingMesh::calculateHarmonicPosition(const vec3& basePos, float time) const {
    vec3 pos = basePos;
    
    // Apply harmonic distortion based on angle and time
    float angle = atan2(basePos.z, basePos.x);
    float radius = sqrt(basePos.x * basePos.x + basePos.z * basePos.z);
    
    // Multiple harmonic frequencies
    float harmonic1 = sin(angle * 3.0f + time) * 0.05f;
    float harmonic2 = sin(angle * 5.0f + time * 1.3f) * 0.03f;
    float harmonic3 = sin(angle * 7.0f + time * 0.7f) * 0.02f;
    
    float totalHarmonic = harmonic1 + harmonic2 + harmonic3;
    
    // Apply to Y position for wave-like effect
    pos.y += totalHarmonic * radius * 0.5f;
    
    return pos;
}

vec3 RingMesh::calculateQuantumDisplacement(const vec3& basePos, float time) const {
    if (m_quantumFluctuation <= 0.0f) return vec3_zero();
    
    // Quantum fluctuation creates random displacement
    float x = sin(basePos.x * 20.0f + time * 10.0f) * cos(basePos.z * 15.0f + time * 8.0f);
    float y = cos(basePos.y * 25.0f + time * 12.0f) * sin(basePos.x * 18.0f + time * 9.0f);
    float z = sin(basePos.z * 22.0f + time * 11.0f) * cos(basePos.y * 16.0f + time * 7.0f);
    
    vec3 displacement = make_vec3(x, y, z) * m_quantumFluctuation * 0.02f;
    
    return displacement;
}

// ============================================================================
// RingRipple Implementation
// ============================================================================

RingRipple::RingRipple() 
    : SceneNode("Ring Ripple")
    , m_radius(1.0f)
    , m_currentRadius(0.0f)
    , m_intensity(1.0f)
    , m_color(make_vec3(0.4f, 0.8f, 1.0f))
    , m_duration(2.0f)
    , m_time(0.0f)
    , m_expansionSpeed(2.0f)
    , m_fadeInTime(0.2f)
    , m_fadeOutTime(0.5f)
    , m_isExpired(false) {
    
    m_rippleMesh = std::make_shared<RingMesh>();
    m_rippleMaterial = std::make_shared<Material>("ring_ripple");
    
    // Configure ripple mesh
    m_rippleMesh->setHeight(0.02f);
    m_rippleMesh->setSegments(48);
    m_rippleMesh->enableTessellation(true);
    m_rippleMesh->setTessellationLevel(2);
}

RingRipple::~RingRipple() = default;

void RingRipple::setRadius(float radius) {
    m_radius = std::max(0.1f, radius);
}

void RingRipple::setIntensity(float intensity) {
    m_intensity = clamp(intensity, 0.0f, 3.0f);
}

void RingRipple::setColor(const vec3& color) {
    m_color = color;
}

void RingRipple::setDuration(float duration) {
    m_duration = std::max(0.1f, duration);
}

void RingRipple::setExpansionSpeed(float speed) {
    m_expansionSpeed = std::max(0.1f, speed);
}

void RingRipple::setFadeInTime(float time) {
    m_fadeInTime = clamp(time, 0.0f, m_duration * 0.5f);
}

void RingRipple::setFadeOutTime(float time) {
    m_fadeOutTime = clamp(time, 0.0f, m_duration * 0.5f);
}

void RingRipple::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateRipple(deltaTime);
    
    if (m_time >= m_duration) {
        m_isExpired = true;
    }
}

void RingRipple::render(RenderContext& context) {
    if (m_isExpired) return;
    
    context.pushTransform(getWorldMatrix());
    context.setBlendMode(BlendMode::ADDITIVE);
    
    float alpha = calculateAlpha();
    vec4 rippleColor = make_vec4(m_color.x, m_color.y, m_color.z, alpha);
    
    context.setColor(rippleColor);
    context.setEmission(m_color * m_intensity * alpha);
    
    if (m_rippleMesh && m_currentRadius > 0.01f) {
        m_rippleMesh->render(context);
    }
    
    context.popTransform();
}

void RingRipple::updateRipple(float deltaTime) {
    m_time += deltaTime;
    
    // Expand the ripple
    m_currentRadius = m_time * m_expansionSpeed;
    
    // Update ripple mesh dimensions
    float innerRadius = std::max(0.0f, m_currentRadius - 0.1f);
    float outerRadius = m_currentRadius;
    
    m_rippleMesh->setInnerRadius(innerRadius);
    m_rippleMesh->setOuterRadius(outerRadius);
    m_rippleMesh->rebuild();
}

float RingRipple::calculateAlpha() const {
    float progress = m_time / m_duration;
    float alpha = m_intensity;
    
    // Fade in
    if (m_time < m_fadeInTime) {
        alpha *= (m_time / m_fadeInTime);
    }
    // Fade out
    else if (m_time > (m_duration - m_fadeOutTime)) {
        float fadeProgress = (m_time - (m_duration - m_fadeOutTime)) / m_fadeOutTime;
        alpha *= (1.0f - fadeProgress);
    }
    
    // Additional fade based on expansion
    alpha *= (1.0f - progress);
    
    return clamp(alpha, 0.0f, 1.0f);
}

// ============================================================================
// ElectricField Implementation
// ============================================================================

ElectricField::ElectricField() 
    : SceneNode("Electric Field")
    , m_fieldStrength(1.0f)
    , m_fieldRadius(2.0f)
    , m_dischargeRate(2.0f)
    , m_lightningEnabled(false)
    , m_lightningIntensity(1.0f)
    , m_dischargeTime(0.0f) {
    
    // Create field particles
    ParticleEmitter::Config fieldConfig;
    fieldConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    fieldConfig.emitRate = 20.0f;
    fieldConfig.particleLifetime = 2.0f;
    fieldConfig.startSize = 0.01f;
    fieldConfig.endSize = 0.03f;
    fieldConfig.startColor = make_vec4(0.8f, 0.9f, 1.0f, 0.6f);
    fieldConfig.endColor = make_vec4(0.8f, 0.9f, 1.0f, 0.0f);
    fieldConfig.velocity = 0.5f;
    fieldConfig.spread = 1.0f;
    
    m_fieldParticles = std::make_shared<ParticleEmitter>(fieldConfig);
    m_fieldParticles->setName("Electric Field");
    addChild(m_fieldParticles);
}

ElectricField::~ElectricField() = default;

void ElectricField::setFieldStrength(float strength) {
    m_fieldStrength = clamp(strength, 0.0f, 2.0f);
    
    if (m_fieldParticles) {
        float emissionRate = 20.0f * m_fieldStrength;
        m_fieldParticles->setEmitRate(emissionRate);
    }
}

void ElectricField::setFieldRadius(float radius) {
    m_fieldRadius = std::max(0.5f, radius);
    
    if (m_fieldParticles) {
        m_fieldParticles->setEmitRadius(m_fieldRadius);
    }
}

void ElectricField::setDischargeRate(float rate) {
    m_dischargeRate = clamp(rate, 0.1f, 10.0f);
}

void ElectricField::enableLightning(bool enable) {
    m_lightningEnabled = enable;
}

void ElectricField::setLightningIntensity(float intensity) {
    m_lightningIntensity = clamp(intensity, 0.0f, 2.0f);
}

void ElectricField::triggerDischarge(float intensity) {
    if (m_lightningEnabled) {
        // Generate lightning bolt to random point
        vec3 start = vec3_zero();
        float angle = (rand() / float(RAND_MAX)) * 2.0f * M_PI;
        float radius = m_fieldRadius * 0.8f;
        vec3 end = make_vec3(cos(angle) * radius, 0, sin(angle) * radius);
        
        generateLightningBolt(start, end);
    }
    
    // Add charge point
    addChargePoint(vec3_zero(), intensity);
}

void ElectricField::addChargePoint(const vec3& position, float charge) {
    ChargePoint point;
    point.position = position;
    point.charge = charge;
    point.life = 2.0f;
    
    m_chargePoints.push_back(point);
}

void ElectricField::clearChargePoints() {
    m_chargePoints.clear();
}

void ElectricField::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateChargePoints(deltaTime);
    updateLightning(deltaTime);
    
    // Auto-discharge based on field strength
    m_dischargeTime += deltaTime;
    if (m_dischargeTime >= (1.0f / m_dischargeRate)) {
        if (m_fieldStrength > 0.5f) {
            triggerDischarge(m_fieldStrength);
        }
        m_dischargeTime = 0.0f;
    }
}

void ElectricField::render(RenderContext& context) {
    SceneNode::render(context);
    
    renderFieldLines(context);
    renderLightningBolts(context);
}

void ElectricField::updateChargePoints(float deltaTime) {
    for (auto& point : m_chargePoints) {
        point.life -= deltaTime;
    }
    
    // Remove expired charge points
    m_chargePoints.erase(
        std::remove_if(m_chargePoints.begin(), m_chargePoints.end(),
            [](const ChargePoint& point) { return point.life <= 0.0f; }),
        m_chargePoints.end()
    );
}

void ElectricField::updateLightning(float deltaTime) {
    for (auto& bolt : m_lightningBolts) {
        bolt.life -= deltaTime;
        
        // Fade intensity over time
        if (bolt.life > 0.0f) {
            bolt.intensity *= 0.95f; // Quick fade
        }
    }
    
    // Remove expired lightning bolts
    m_lightningBolts.erase(
        std::remove_if(m_lightningBolts.begin(), m_lightningBolts.end(),
            [](const LightningBolt& bolt) { return bolt.life <= 0.0f; }),
        m_lightningBolts.end()
    );
}

void ElectricField::generateLightningBolt(const vec3& start, const vec3& end) {
    LightningBolt bolt;
    bolt.intensity = m_lightningIntensity;
    bolt.life = 0.3f;
    bolt.maxLife = 0.3f;
    
    // Generate jagged path from start to end
    vec3 direction = end - start;
    float distance = magnitude(direction);
    vec3 normalizedDir = normalize(direction);
    
    int segments = static_cast<int>(distance * 10); // More segments for longer bolts
    segments = std::max(3, std::min(segments, 20));
    
    bolt.points.push_back(start);
    
    for (int i = 1; i < segments; ++i) {
        float t = i / float(segments);
        vec3 basePoint = start + direction * t;
        
        // Add random deviation
        vec3 perpendicular = cross(normalizedDir, make_vec3(0, 1, 0));
        if (magnitude(perpendicular) < 0.1f) {
            perpendicular = cross(normalizedDir, make_vec3(1, 0, 0));
        }
        perpendicular = normalize(perpendicular);
        
        float deviation = ((rand() / float(RAND_MAX)) - 0.5f) * distance * 0.2f;
        vec3 point = basePoint + perpendicular * deviation;
        
        bolt.points.push_back(point);
    }
    
    bolt.points.push_back(end);
    m_lightningBolts.push_back(bolt);
}

void ElectricField::renderLightningBolts(RenderContext& context) {
    for (const auto& bolt : m_lightningBolts) {
        if (bolt.life > 0.0f && bolt.points.size() >= 2) {
            float alpha = bolt.intensity * (bolt.life / bolt.maxLife);
            vec4 lightningColor = make_vec4(0.9f, 0.95f, 1.0f, alpha);
            
            context.setColor(lightningColor);
            context.setLineWidth(0.02f * alpha);
            
            // Draw lightning as connected line segments
            for (size_t i = 0; i < bolt.points.size() - 1; ++i) {
                context.drawLine(bolt.points[i], bolt.points[i + 1]);
            }
        }
    }
}

void ElectricField::renderFieldLines(RenderContext& context) {
    if (m_fieldStrength > 0.1f) {
        // Render field visualization as radiating lines
        int lineCount = static_cast<int>(8 * m_fieldStrength);
        
        for (int i = 0; i < lineCount; ++i) {
            float angle = (i / float(lineCount)) * 2.0f * M_PI;
            vec3 direction = make_vec3(cos(angle), 0, sin(angle));
            vec3 end = direction * m_fieldRadius;
            
            float alpha = m_fieldStrength * 0.3f;
            vec4 fieldColor = make_vec4(0.7f, 0.8f, 1.0f, alpha);
            
            context.setColor(fieldColor);
            context.setLineWidth(0.01f);
            context.drawLine(vec3_zero(), end);
        }
    }
}

// ============================================================================
// QuantumField Implementation
// ============================================================================

QuantumField::QuantumField() 
    : SceneNode("Quantum Field")
    , m_fieldDensity(0.5f)
    , m_fluctuationRate(2.0f)
    , m_tunnelingProbability(0.3f)
    , m_coherenceLevel(0.7f)
    , m_waveFunctionEnabled(true)
    , m_quantumState(0)
    , m_isCollapsed(false)
    , m_fluctuationPhase(0.0f)
    , m_collapseTime(0.0f) {
    
    // Create quantum fluctuation particles
    ParticleEmitter::Config quantumConfig;
    quantumConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    quantumConfig.emitRate = 15.0f;
    quantumConfig.particleLifetime = 3.0f;
    quantumConfig.startSize = 0.005f;
    quantumConfig.endSize = 0.02f;
    quantumConfig.startColor = make_vec4(0.8f, 0.2f, 1.0f, 0.8f);
    quantumConfig.endColor = make_vec4(0.8f, 0.2f, 1.0f, 0.0f);
    quantumConfig.velocity = 0.2f;
    quantumConfig.spread = 2.0f;
    
    m_fluctuationParticles = std::make_shared<ParticleEmitter>(quantumConfig);
    m_fluctuationParticles->setName("Quantum Fluctuations");
    addChild(m_fluctuationParticles);
    
    // Initialize quantum particles
    for (int i = 0; i < 20; ++i) {
        QuantumParticle particle;
        float angle = (i / 20.0f) * 2.0f * M_PI;
        particle.position = make_vec3(cos(angle), 0, sin(angle));
        particle.momentum = make_vec3(0, 0, 0);
        particle.probability = 1.0f;
        particle.state = i % 3;
        particle.tunneling = false;
        
        m_quantumParticles.push_back(particle);
    }
}

QuantumField::~QuantumField() = default;

void QuantumField::setFieldDensity(float density) {
    m_fieldDensity = clamp(density, 0.0f, 1.0f);
    
    if (m_fluctuationParticles) {
        float emissionRate = 15.0f * m_fieldDensity;
        m_fluctuationParticles->setEmitRate(emissionRate);
    }
}

void QuantumField::setFluctuationRate(float rate) {
    m_fluctuationRate = clamp(rate, 0.1f, 10.0f);
}

void QuantumField::setTunnelingProbability(float probability) {
    m_tunnelingProbability = clamp(probability, 0.0f, 1.0f);
}

void QuantumField::setCoherenceLevel(float coherence) {
    m_coherenceLevel = clamp(coherence, 0.0f, 1.0f);
    m_isCollapsed = coherence < 0.3f;
}

void QuantumField::enableWaveFunction(bool enable) {
    m_waveFunctionEnabled = enable;
}

void QuantumField::setQuantumState(int state) {
    m_quantumState = state;
}

void QuantumField::collapseWaveFunction() {
    m_isCollapsed = true;
    m_collapseTime = 0.0f;
    m_coherenceLevel = 0.1f;
    
    // All quantum particles collapse to the same state
    for (auto& particle : m_quantumParticles) {
        particle.state = m_quantumState;
        particle.probability = 1.0f;
    }
}

void QuantumField::entangleWithField(std::shared_ptr<QuantumField> other) {
    m_entangledField = other;
}

void QuantumField::measureQuantumState() {
    // Measurement causes partial collapse
    m_coherenceLevel *= 0.7f;
    if (m_coherenceLevel < 0.3f) {
        collapseWaveFunction();
    }
}

void QuantumField::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateQuantumParticles(deltaTime);
    updateWaveFunction(deltaTime);
    updateEntanglement(deltaTime);
    
    m_fluctuationPhase += deltaTime * m_fluctuationRate;
    
    if (m_isCollapsed) {
        m_collapseTime += deltaTime;
        
        // Gradual recovery from collapse
        if (m_collapseTime > 5.0f) {
            m_coherenceLevel = std::min(1.0f, m_coherenceLevel + deltaTime * 0.1f);
            if (m_coherenceLevel > 0.5f) {
                m_isCollapsed = false;
            }
        }
    }
}

void QuantumField::render(RenderContext& context) {
    SceneNode::render(context);
    
    if (m_waveFunctionEnabled) {
        renderWaveFunction(context);
    }
    
    renderQuantumParticles(context);
}

void QuantumField::updateQuantumParticles(float deltaTime) {
    for (auto& particle : m_quantumParticles) {
        // Update quantum state probability
        if (!m_isCollapsed) {
            particle.probability *= (0.99f + 0.02f * sin(m_fluctuationPhase * 3.0f + particle.state));
            particle.probability = clamp(particle.probability, 0.1f, 1.0f);
        }
        
        // Check for quantum tunneling
        float tunnelingChance = calculateTunnelingProbability(particle);
        if ((rand() / float(RAND_MAX)) < tunnelingChance * deltaTime) {
            particle.tunneling = true;
            particle.position = particle.position + make_vec3(
                ((rand() / float(RAND_MAX)) - 0.5f) * 0.5f,
                ((rand() / float(RAND_MAX)) - 0.5f) * 0.5f,
                ((rand() / float(RAND_MAX)) - 0.5f) * 0.5f
            );
        }
        
        // Update position based on momentum
        particle.position = particle.position + particle.momentum * deltaTime;
        
        // Apply quantum fluctuations
        vec3 fluctuation = make_vec3(
            sin(m_fluctuationPhase * 2.0f + particle.state) * 0.01f,
            cos(m_fluctuationPhase * 1.8f + particle.state) * 0.01f,
            sin(m_fluctuationPhase * 2.2f + particle.state) * 0.01f
        );
        particle.position = particle.position + fluctuation * m_fieldDensity;
    }
}

void QuantumField::updateWaveFunction(float deltaTime) {
    if (m_waveFunctionEnabled && !m_isCollapsed) {
        // Wave function evolution - simplified Schrödinger equation
        for (auto& particle : m_quantumParticles) {
            float waveAmplitude = calculateWaveFunctionAmplitude(particle.position, m_fluctuationPhase).x;
            particle.probability = waveAmplitude * waveAmplitude * m_coherenceLevel;
        }
    }
}

void QuantumField::updateEntanglement(float deltaTime) {
    auto entangled = m_entangledField.lock();
    if (entangled) {
        // Simple entanglement - mirror states with some delay
        if (m_quantumState != entangled->m_quantumState) {
            m_quantumState = entangled->m_quantumState;
            
            // Update particle states
            for (auto& particle : m_quantumParticles) {
                particle.state = m_quantumState;
            }
        }
    }
}

void QuantumField::renderWaveFunction(RenderContext& context) {
    if (m_coherenceLevel < 0.1f) return;
    
    // Render wave function as probability cloud
    int gridSize = 16;
    float step = 2.0f / gridSize;
    
    for (int x = 0; x < gridSize; ++x) {
        for (int z = 0; z < gridSize; ++z) {
            vec3 pos = make_vec3((x - gridSize/2) * step, 0, (z - gridSize/2) * step);
            vec3 amplitude = calculateWaveFunctionAmplitude(pos, m_fluctuationPhase);
            
            float probability = dot(amplitude, amplitude) * m_coherenceLevel;
            if (probability > 0.1f) {
                context.pushTransform(mat4_identity());
                context.translate(pos);
                
                vec4 waveColor = make_vec4(0.6f, 0.2f, 1.0f, probability * 0.3f);
                context.setColor(waveColor);
                context.drawGlowSphere(0.05f * probability);
                
                context.popTransform();
            }
        }
    }
}

void QuantumField::renderQuantumParticles(RenderContext& context) {
    for (const auto& particle : m_quantumParticles) {
        if (particle.probability > 0.1f) {
            context.pushTransform(mat4_identity());
            context.translate(particle.position);
            
            // Color based on quantum state
            vec3 stateColors[] = {
                make_vec3(1.0f, 0.2f, 0.8f), // Magenta
                make_vec3(0.2f, 0.8f, 1.0f), // Cyan
                make_vec3(0.8f, 1.0f, 0.2f)  // Yellow-green
            };
            
            vec3 particleColor = stateColors[particle.state % 3];
            vec4 color = make_vec4(particleColor.x, particleColor.y, particleColor.z, particle.probability);
            
            context.setColor(color);
            
            float size = 0.02f;
            if (particle.tunneling) {
                size *= 1.5f; // Larger when tunneling
                context.setEmission(particleColor * 2.0f);
            }
            
            context.drawGlowSphere(size);
            
            context.popTransform();
        }
    }
}

vec3 QuantumField::calculateWaveFunctionAmplitude(const vec3& position, float time) const {
    // Simplified wave function calculation
    float x = position.x;
    float z = position.z;
    
    // Multiple wave components
    float wave1 = sin(x * 3.0f + time) * cos(z * 2.0f + time * 0.7f);
    float wave2 = cos(x * 2.0f + time * 1.3f) * sin(z * 3.0f + time * 0.9f);
    float wave3 = sin((x + z) * 1.5f + time * 1.1f);
    
    vec3 amplitude = make_vec3(wave1, wave2, wave3) * m_coherenceLevel;
    
    return amplitude;
}

float QuantumField::calculateTunnelingProbability(const QuantumParticle& particle) const {
    // Tunneling probability based on position and field density
    float distance = magnitude(particle.position);
    float baseProbability = m_tunnelingProbability * m_fieldDensity;
    
    // Higher probability at field boundaries
    float boundaryFactor = 1.0f + (distance / 2.0f);
    
    return baseProbability * boundaryFactor * (1.0f - m_coherenceLevel);
}

// ============================================================================
// RingCluster Implementation
// ============================================================================

RingCluster::RingCluster() 
    : SceneNode("Ring Cluster")
    , m_clusterIntensity(1.0f)
    , m_clusterColor(make_vec3(0.4f, 0.8f, 1.0f))
    , m_globalRotationSpeed(1.0f)
    , m_synchronizationEnabled(false)
    , m_synchronizationPhase(0.0f)
    , m_resonanceEnabled(false)
    , m_resonanceFrequency(1.0f)
    , m_interferenceStrength(0.0f)
    , m_serviceLoad(0.0f)
    , m_serviceActivity(0.0f)
    , m_clusterPhase(0.0f)
    , m_resonancePhase(0.0f) {
    
    std::cout << "RingCluster created." << std::endl;
}

RingCluster::~RingCluster() = default;

void RingCluster::addRing(const RingConfig& config) {
    auto ring = std::make_shared<EnergyRing>();
    
    // Configure the ring
    ring->setInnerRadius(config.radius * 0.8f);
    ring->setOuterRadius(config.radius);
    ring->setHeight(config.height);
    ring->setColor(config.color);
    ring->setRotationSpeed(config.rotationSpeed);
    ring->setRotationAxis(config.rotationAxis);
    ring->setRingType(config.type);
    
    m_rings.push_back(ring);
    m_ringConfigs.push_back(config);
    addChild(ring);
    
    std::cout << "RingCluster: Added ring with radius " << config.radius << std::endl;
}

void RingCluster::removeRing(int index) {
    if (index >= 0 && index < static_cast<int>(m_rings.size())) {
        removeChild(m_rings[index]);
        m_rings.erase(m_rings.begin() + index);
        m_ringConfigs.erase(m_ringConfigs.begin() + index);
    }
}

void RingCluster::clearRings() {
    for (auto& ring : m_rings) {
        removeChild(ring);
    }
    m_rings.clear();
    m_ringConfigs.clear();
}

void RingCluster::setClusterIntensity(float intensity) {
    m_clusterIntensity = clamp(intensity, 0.0f, 2.0f);
    applyClusterIntensity();
}

void RingCluster::setClusterColor(const vec3& color) {
    m_clusterColor = color;
    applyClusterColor();
}

void RingCluster::setGlobalRotationSpeed(float speed) {
    m_globalRotationSpeed = speed;
    
    for (size_t i = 0; i < m_rings.size(); ++i) {
        if (m_rings[i]) {
            float ringSpeed = m_ringConfigs[i].rotationSpeed * speed;
            m_rings[i]->setRotationSpeed(ringSpeed);
        }
    }
}

void RingCluster::enableSynchronization(bool enable) {
    m_synchronizationEnabled = enable;
    if (enable) {
        m_synchronizationPhase = 0.0f;
    }
}

void RingCluster::setSynchronizationPhase(float phase) {
    m_synchronizationPhase = phase;
}

void RingCluster::triggerClusterPulse(float intensity) {
    for (auto& ring : m_rings) {
        if (ring) {
            ring->triggerRipple(intensity);
            ring->setPulseIntensity(intensity);
        }
    }
    
    std::cout << "RingCluster: Triggered cluster pulse with intensity " << intensity << std::endl;
}

void RingCluster::enableResonance(bool enable) {
    m_resonanceEnabled = enable;
    
    if (enable) {
        for (auto& ring : m_rings) {
            if (ring) {
                ring->enableHarmonicResonance(true, m_resonanceFrequency);
            }
        }
    }
}

void RingCluster::setResonanceFrequency(float frequency) {
    m_resonanceFrequency = frequency;
    
    if (m_resonanceEnabled) {
        for (auto& ring : m_rings) {
            if (ring) {
                ring->setMusicalNote(frequency);
            }
        }
    }
}

void RingCluster::createInterference(float strength) {
    m_interferenceStrength = clamp(strength, 0.0f, 1.0f);
}

void RingCluster::setServiceLoad(float load) {
    m_serviceLoad = clamp(load, 0.0f, 1.0f);
    
    for (auto& ring : m_rings) {
        if (ring) {
            ring->setProcessingLoad(load);
        }
    }
}

void RingCluster::setServiceState(const std::string& state) {
    m_serviceState = state;
    
    EnergyRing::RingState ringState = EnergyRing::RingState::ACTIVE;
    
    if (state == "idle") {
        ringState = EnergyRing::RingState::IDLE;
    } else if (state == "overload") {
        ringState = EnergyRing::RingState::OVERLOAD;
    } else if (state == "critical") {
        ringState = EnergyRing::RingState::CRITICAL;
    } else if (state == "harmonizing") {
        ringState = EnergyRing::RingState::HARMONIZING;
    } else if (state == "corrupted") {
        ringState = EnergyRing::RingState::CORRUPTED;
    }
    
    for (auto& ring : m_rings) {
        if (ring) {
            ring->setState(ringState);
        }
    }
}

void RingCluster::showServiceActivity(float activity) {
    m_serviceActivity = clamp(activity, 0.0f, 2.0f);
    
    for (auto& ring : m_rings) {
        if (ring) {
            ring->showDataBurst(activity);
        }
    }
}

void RingCluster::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    m_clusterPhase += deltaTime * m_globalRotationSpeed;
    
    updateSynchronization(deltaTime);
    updateResonance(deltaTime);
    updateServiceVisualization(deltaTime);
    updateInterference(deltaTime);
}

void RingCluster::render(RenderContext& context) {
    // Rings render themselves as child nodes
    SceneNode::render(context);
    
    // Additional cluster-wide effects could be rendered here
    if (m_interferenceStrength > 0.3f) {
        renderInterferencePattern(context);
    }
}

void RingCluster::updateSynchronization(float deltaTime) {
    if (m_synchronizationEnabled) {
        applySynchronization();
    }
}

void RingCluster::updateResonance(float deltaTime) {
    if (m_resonanceEnabled) {
        m_resonancePhase += deltaTime * m_resonanceFrequency * 2.0f * M_PI;
        applyResonance();
    }
}

void RingCluster::updateServiceVisualization(float deltaTime) {
    // Update ring properties based on service metrics
    float energyLevel = 0.3f + m_serviceLoad * 0.7f + m_serviceActivity * 0.3f;
    
    for (auto& ring : m_rings) {
        if (ring) {
            ring->setEnergyLevel(energyLevel);
        }
    }
}

void RingCluster::updateInterference(float deltaTime) {
    if (m_interferenceStrength > 0.0f) {
        // Apply interference effects between rings
        for (size_t i = 0; i < m_rings.size(); ++i) {
            if (m_rings[i]) {
                vec3 interferenceColor = calculateInterferenceColor(static_cast<int>(i), m_clusterPhase);
                m_rings[i]->setColor(interferenceColor);
            }
        }
    }
}

void RingCluster::applyClusterIntensity() {
    for (auto& ring : m_rings) {
        if (ring) {
            ring->setGlowIntensity(ring->getEnergyLevel() * m_clusterIntensity);
        }
    }
}

void RingCluster::applyClusterColor() {
    for (size_t i = 0; i < m_rings.size(); ++i) {
        if (m_rings[i]) {
            vec3 blendedColor = lerp(m_ringConfigs[i].color, m_clusterColor, 0.5f);
            m_rings[i]->setColor(blendedColor);
        }
    }
}

void RingCluster::applySynchronization() {
    if (m_rings.empty()) return;
    
    for (size_t i = 0; i < m_rings.size(); ++i) {
        if (m_rings[i]) {
            float phaseOffset = calculateRingPhaseOffset(static_cast<int>(i));
            float synchronizedSpeed = m_globalRotationSpeed;
            
            // Apply phase offset to create synchronized motion patterns
            m_rings[i]->setRotationSpeed(synchronizedSpeed);
            
            // Trigger synchronized pulses
            if (fmod(m_clusterPhase + phaseOffset, 2.0f * M_PI) < 0.1f) {
                m_rings[i]->triggerRipple(0.8f);
            }
        }
    }
}

void RingCluster::applyResonance() {
    for (size_t i = 0; i < m_rings.size(); ++i) {
        if (m_rings[i]) {
            // Each ring resonates at a harmonic frequency
            float harmonic = m_resonanceFrequency * (1.0f + i * 0.5f);
            m_rings[i]->setMusicalNote(harmonic);
            
            // Apply resonance-based intensity modulation
            float resonanceModulation = 0.8f + 0.2f * sin(m_resonancePhase * (1.0f + i * 0.3f));
            m_rings[i]->setGlowIntensity(m_clusterIntensity * resonanceModulation);
        }
    }
}

float RingCluster::calculateRingPhaseOffset(int ringIndex) const {
    if (m_rings.empty()) return 0.0f;
    
    // Create spiral or circular phase pattern
    float baseOffset = (ringIndex / float(m_rings.size())) * 2.0f * M_PI;
    
    // Add dynamic phase modulation
    float dynamicOffset = sin(m_clusterPhase * 0.3f) * 0.5f;
    
    return baseOffset + dynamicOffset + m_synchronizationPhase;
}

vec3 RingCluster::calculateInterferenceColor(int ringIndex, float time) const {
    if (m_interferenceStrength <= 0.0f) {
        return m_ringConfigs[ringIndex].color;
    }
    
    // Calculate interference pattern based on ring positions and time
    float interference = sin(time * 2.0f + ringIndex * 1.2f) * cos(time * 1.7f + ringIndex * 0.8f);
    interference *= m_interferenceStrength;
    
    // Shift hue based on interference
    vec3 baseColor = m_ringConfigs[ringIndex].color;
    vec3 interferenceColor = make_vec3(
        baseColor.x + interference * 0.3f,
        baseColor.y + interference * 0.2f,
        baseColor.z + interference * 0.4f
    );
    
    // Clamp to valid color range
    interferenceColor.x = clamp(interferenceColor.x, 0.0f, 1.0f);
    interferenceColor.y = clamp(interferenceColor.y, 0.0f, 1.0f);
    interferenceColor.z = clamp(interferenceColor.z, 0.0f, 1.0f);
    
    return interferenceColor;
}

void RingCluster::renderInterferencePattern(RenderContext& context) {
    // Render interference visualization between rings
    if (m_rings.size() < 2) return;
    
    for (size_t i = 0; i < m_rings.size(); ++i) {
        for (size_t j = i + 1; j < m_rings.size(); ++j) {
            if (m_rings[i] && m_rings[j]) {
                // Calculate interference between rings i and j
                vec3 pos1 = m_rings[i]->getPosition();
                vec3 pos2 = m_rings[j]->getPosition();
                vec3 midpoint = (pos1 + pos2) * 0.5f;
                
                float distance = magnitude(pos2 - pos1);
                float interferenceIntensity = m_interferenceStrength / (1.0f + distance);
                
                if (interferenceIntensity > 0.1f) {
                    context.pushTransform(mat4_identity());
                    context.translate(midpoint);
                    
                    vec4 interferenceColor = make_vec4(
                        0.8f, 0.6f, 1.0f, 
                        interferenceIntensity * 0.5f
                    );
                    context.setColor(interferenceColor);
                    context.drawGlowSphere(0.1f * interferenceIntensity);
                    
                    context.popTransform();
                }
            }
        }
    }
}

} // namespace FinalStorm