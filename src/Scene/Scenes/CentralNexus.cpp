// ============================================================================
// File: FinalStorm/src/Scene/Scenes/CentralNexus.cpp
// CentralNexus Implementation - The Heart of The Nexus
// ============================================================================

#include "Scene/Scenes/FirstScene.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Math.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include <iostream>
#include <cmath>

namespace FinalStorm {

// ============================================================================
// CentralNexus Implementation - The crystalline heart of Finalverse
// ============================================================================

CentralNexus::CentralNexus() : SceneNode("Central Nexus") {
    // Initialize visual parameters
    m_coreColor = make_vec3(0.0f, 0.8f, 1.0f);
    m_energyColor = make_vec3(0.2f, 0.9f, 1.0f);
    m_rotationPhase = 0.0f;
    m_pulsePhase = 0.0f;
    m_pulseIntensity = 0.0f;
    m_connectionStrength = 1.0f;
    
    // Create the nexus structure
    createCoreStructure();
    createEnergyRings();
    createEnergyFlow();
    createProtectiveShell();
    
    std::cout << "Central Nexus initialized with crystalline core structure." << std::endl;
}

CentralNexus::~CentralNexus() = default;

void CentralNexus::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateRotations(deltaTime);
    updatePulseEffects(deltaTime);
    updateEnergyFlow(deltaTime);
    updateConnectionEffects(deltaTime);
    updateResonanceField(deltaTime);
}

void CentralNexus::render(RenderContext& context) {
    // Set emissive properties for glow effect
    context.pushMaterial();
    
    // Apply core emission based on activity
    vec3 emission = m_coreColor * (0.5f + m_pulseIntensity * 0.5f);
    context.setEmission(emission);
    context.setEmissionIntensity(1.0f + m_pulseIntensity);
    
    // Render with bloom effect
    context.enableBloom(true);
    context.setBloomThreshold(0.3f);
    context.setBloomIntensity(0.8f + m_pulseIntensity * 0.4f);
    
    SceneNode::render(context);
    
    context.popMaterial();
}

void CentralNexus::pulse(float intensity) {
    m_pulseIntensity = clamp(intensity, 0.0f, 2.0f);
    m_pulsePhase = 0.0f;
    
    // Create ripple effect through energy rings
    if (m_innerRings) {
        for (auto& child : m_innerRings->getChildren()) {
            if (auto ring = std::dynamic_pointer_cast<EnergyRing>(child)) {
                ring->triggerRipple(intensity);
            }
        }
    }
    
    // Burst energy particles
    if (m_energyFlow) {
        int burstCount = static_cast<int>(intensity * 50);
        m_energyFlow->burst(burstCount);
    }
    
    // Create shockwave effect
    createShockwave(intensity);
    
    std::cout << "Nexus pulse triggered with intensity: " << intensity << std::endl;
}

void CentralNexus::setConnectionStrength(float strength) {
    m_connectionStrength = clamp(strength, 0.0f, 1.0f);
    
    // Update particle emission rate
    if (m_energyFlow) {
        float baseRate = 20.0f;
        float activityRate = strength * 80.0f;
        m_energyFlow->setEmitRate(baseRate + activityRate);
    }
    
    // Update ring rotation speeds
    updateRingRotationSpeeds();
    
    // Update core brightness
    updateCoreBrightness();
    
    std::cout << "Connection strength set to: " << strength << std::endl;
}

void CentralNexus::addConnectionBeam(const vec3& targetPosition, float intensity) {
    // Create a connection beam to external service
    auto beam = std::make_shared<ConnectionBeam>();
    beam->setStartPosition(getWorldPosition());
    beam->setEndPosition(targetPosition);
    beam->setIntensity(intensity);
    beam->setColor(m_energyColor);
    beam->setDuration(2.0f);
    
    m_connectionBeams.push_back(beam);
    addChild(beam);
    
    // Pulse when connection is made
    pulse(intensity * 0.5f);
}

void CentralNexus::removeConnectionBeam(const vec3& targetPosition) {
    auto it = std::remove_if(m_connectionBeams.begin(), m_connectionBeams.end(),
        [targetPosition](const std::shared_ptr<ConnectionBeam>& beam) {
            return distance(beam->getEndPosition(), targetPosition) < 0.1f;
        });
    
    if (it != m_connectionBeams.end()) {
        for (auto beamIt = it; beamIt != m_connectionBeams.end(); ++beamIt) {
            removeChild(*beamIt);
        }
        m_connectionBeams.erase(it, m_connectionBeams.end());
    }
}

void CentralNexus::setResonanceMode(ResonanceMode mode) {
    m_resonanceMode = mode;
    
    switch (mode) {
        case ResonanceMode::HARMONIC:
            m_coreColor = make_vec3(0.0f, 0.8f, 1.0f);
            m_energyColor = make_vec3(0.2f, 0.9f, 1.0f);
            break;
            
        case ResonanceMode::CREATIVE:
            m_coreColor = make_vec3(1.0f, 0.5f, 0.0f);
            m_energyColor = make_vec3(1.0f, 0.7f, 0.2f);
            break;
            
        case ResonanceMode::PROTECTIVE:
            m_coreColor = make_vec3(0.2f, 0.8f, 0.2f);
            m_energyColor = make_vec3(0.4f, 1.0f, 0.4f);
            break;
            
        case ResonanceMode::ALERT:
            m_coreColor = make_vec3(1.0f, 0.3f, 0.1f);
            m_energyColor = make_vec3(1.0f, 0.5f, 0.3f);
            break;
    }
    
    // Update material colors
    updateMaterialColors();
}

// Private implementation methods

void CentralNexus::createCoreStructure() {
    // Central crystalline core - primary octahedron
    m_coreStructure = std::make_shared<MeshNode>("Core Crystal");
    m_coreStructure->setMesh("octahedron");
    m_coreStructure->setScale(make_vec3(1.5f));
    
    // Create crystal material
    auto coreMaterial = std::make_shared<Material>("nexus_core");
    coreMaterial->setAlbedo(make_vec3(0.1f, 0.3f, 0.5f));
    coreMaterial->setMetallic(0.8f);
    coreMaterial->setRoughness(0.1f);
    coreMaterial->setEmission(m_coreColor);
    coreMaterial->setEmissionStrength(2.0f);
    m_coreStructure->setMaterial(coreMaterial);
    
    addChild(m_coreStructure);
    
    // Secondary core elements - nested geometric shapes
    createSecondaryCoreElements();
    
    std::cout << "Core structure created with crystalline geometry." << std::endl;
}

void CentralNexus::createSecondaryCoreElements() {
    // Inner icosahedron
    auto innerCore = std::make_shared<MeshNode>("Inner Core");
    innerCore->setMesh("icosahedron");
    innerCore->setScale(make_vec3(0.8f));
    
    auto innerMaterial = std::make_shared<Material>("nexus_inner");
    innerMaterial->setAlbedo(make_vec3(0.05f, 0.2f, 0.4f));
    innerMaterial->setMetallic(0.9f);
    innerMaterial->setRoughness(0.05f);
    innerMaterial->setEmission(m_coreColor * 0.7f);
    innerMaterial->setEmissionStrength(1.5f);
    innerCore->setMaterial(innerMaterial);
    
    m_coreStructure->addChild(innerCore);
    
    // Core energy channels - connecting vertices
    createEnergyChannels();
}

void CentralNexus::createEnergyChannels() {
    // Create energy channels connecting octahedron vertices
    const int numChannels = 6; // One for each vertex pair
    
    for (int i = 0; i < numChannels; ++i) {
        auto channel = std::make_shared<EnergyChannel>();
        
        // Calculate vertex positions for octahedron
        float angle = (i / float(numChannels)) * 2.0f * M_PI;
        vec3 startPos = make_vec3(
            cos(angle) * 1.2f,
            (i % 2 == 0) ? 1.2f : -1.2f,
            sin(angle) * 1.2f
        );
        vec3 endPos = make_vec3(
            cos(angle + M_PI) * 1.2f,
            (i % 2 == 0) ? -1.2f : 1.2f,
            sin(angle + M_PI) * 1.2f
        );
        
        channel->setPath(startPos, endPos);
        channel->setColor(m_energyColor);
        channel->setFlowSpeed(2.0f + i * 0.3f);
        channel->setIntensity(0.7f);
        
        m_energyChannels.push_back(channel);
        m_coreStructure->addChild(channel);
    }
}

void CentralNexus::createEnergyRings() {
    // Create container for rotating rings
    m_innerRings = std::make_shared<SceneNode>("Energy Rings");
    
    // Create multiple energy rings at different angles and scales
    const int numRings = 5;
    for (int i = 0; i < numRings; ++i) {
        auto ring = std::make_shared<EnergyRing>();
        
        float scale = 2.0f + i * 0.4f;
        float thickness = 0.08f + i * 0.02f;
        
        ring->setInnerRadius(scale);
        ring->setOuterRadius(scale + thickness);
        ring->setHeight(0.1f);
        
        // Set rotation angle for variety
        quat rotation = simd_quaternion(
            radians(30.0f * i),
            make_vec3(
                sin(i * 0.7f),
                cos(i * 0.5f),
                sin(i * 1.1f)
            )
        );
        ring->setRotation(rotation);
        
        // Set visual properties
        ring->setColor(m_energyColor);
        ring->setGlowIntensity(0.8f - i * 0.1f);
        ring->setRotationSpeed(0.5f + i * 0.2f);
        ring->setFlowDirection(i % 2 == 0 ? 1.0f : -1.0f);
        
        m_energyRings.push_back(ring);
        m_innerRings->addChild(ring);
    }
    
    addChild(m_innerRings);
    
    std::cout << "Created " << numRings << " energy rings." << std::endl;
}

void CentralNexus::createEnergyFlow() {
    // Create primary energy flow particle system
    ParticleEmitter::Config config;
    config.emitShape = ParticleEmitter::Shape::SPHERE;
    config.emitRadius = 1.5f;
    config.emitRate = 50.0f;
    config.particleLifetime = 3.0f;
    config.startSize = 0.05f;
    config.endSize = 0.01f;
    config.startColor = make_vec4(m_energyColor.x, m_energyColor.y, m_energyColor.z, 1.0f);
    config.endColor = make_vec4(m_energyColor.x, m_energyColor.y, m_energyColor.z, 0.0f);
    config.velocity = 2.5f;
    config.velocityVariation = 0.5f;
    config.gravity = make_vec3(0, 0.3f, 0);
    config.turbulence = 0.2f;
    
    m_energyFlow = std::make_shared<ParticleEmitter>(config);
    m_energyFlow->setName("Energy Flow");
    addChild(m_energyFlow);
    
    // Create secondary stream effects
    createEnergyStreams();
    
    std::cout << "Energy flow system created." << std::endl;
}

void CentralNexus::createEnergyStreams() {
    // Create orbital energy streams
    const int numStreams = 3;
    
    for (int i = 0; i < numStreams; ++i) {
        ParticleEmitter::Config streamConfig;
        streamConfig.emitShape = ParticleEmitter::Shape::LINE;
        streamConfig.emitRate = 20.0f;
        streamConfig.particleLifetime = 2.0f;
        streamConfig.startSize = 0.03f;
        streamConfig.endSize = 0.008f;
        streamConfig.startColor = make_vec4(m_energyColor.x * 0.8f, m_energyColor.y * 0.8f, m_energyColor.z, 0.9f);
        streamConfig.endColor = make_vec4(m_energyColor.x * 0.5f, m_energyColor.y * 0.5f, m_energyColor.z * 0.8f, 0.0f);
        streamConfig.velocity = 3.0f;
        streamConfig.inheritVelocity = 0.3f;
        
        auto stream = std::make_shared<ParticleEmitter>(streamConfig);
        stream->setName("Energy Stream " + std::to_string(i));
        
        m_energyStreams.push_back(stream);
        addChild(stream);
    }
}

void CentralNexus::createProtectiveShell() {
    // Outer protective shell (semi-transparent)
    m_outerShell = std::make_shared<MeshNode>("Protective Shell");
    m_outerShell->setMesh("icosahedron");
    m_outerShell->setScale(make_vec3(4.0f));
    
    // Create glass-like material
    auto shellMaterial = std::make_shared<Material>("nexus_shell");
    shellMaterial->setAlbedo(make_vec3(0.8f, 0.9f, 1.0f));
    shellMaterial->setMetallic(0.1f);
    shellMaterial->setRoughness(0.0f);
    shellMaterial->setTransmission(0.9f);
    shellMaterial->setIOR(1.4f);
    shellMaterial->setAlpha(0.2f);
    shellMaterial->setEmission(m_energyColor * 0.1f);
    
    m_outerShell->setMaterial(shellMaterial);
    m_outerShell->setRenderOrder(1000); // Render last for transparency
    
    addChild(m_outerShell);
    
    // Add surface energy patterns
    createShellPatterns();
    
    std::cout << "Protective shell created with energy patterns." << std::endl;
}

void CentralNexus::createShellPatterns() {
    // Create energy patterns on the shell surface
    const int numPatterns = 8;
    
    for (int i = 0; i < numPatterns; ++i) {
        auto pattern = std::make_shared<EnergyPattern>();
        
        // Distribute patterns across icosahedron faces
        float theta = (i / float(numPatterns)) * 2.0f * M_PI;
        float phi = acos(1.0f - 2.0f * (i % 3) / 3.0f);
        
        vec3 position = make_vec3(
            sin(phi) * cos(theta) * 3.8f,
            cos(phi) * 3.8f,
            sin(phi) * sin(theta) * 3.8f
        );
        
        pattern->setPosition(position);
        pattern->setScale(make_vec3(0.5f));
        pattern->setColor(m_energyColor);
        pattern->setAnimationSpeed(1.0f + i * 0.2f);
        pattern->setIntensity(0.6f);
        
        m_shellPatterns.push_back(pattern);
        m_outerShell->addChild(pattern);
    }
}

void CentralNexus::updateRotations(float deltaTime) {
    // Update core rotation phases
    m_rotationPhase += deltaTime * 0.3f;
    
    // Rotate core structure with complex motion
    if (m_coreStructure) {
        quat coreRotation = simd_mul(
            simd_quaternion(m_rotationPhase, make_vec3(0, 1, 0)),
            simd_quaternion(m_rotationPhase * 0.7f, make_vec3(1, 0, 0))
        );
        m_coreStructure->setRotation(coreRotation);
    }
    
    // Update energy rings rotation
    if (m_innerRings) {
        quat ringsRotation = simd_quaternion(
            -m_rotationPhase * 1.5f,
            make_vec3(0, 1, 0)
        );
        m_innerRings->setRotation(ringsRotation);
        
        // Update individual ring rotations
        for (auto& ring : m_energyRings) {
            ring->updateRotation(deltaTime);
        }
    }
    
    // Counter-rotate outer shell slowly
    if (m_outerShell) {
        quat shellRotation = simd_quaternion(
            m_rotationPhase * 0.2f,
            make_vec3(0, 1, 0)
        );
        m_outerShell->setRotation(shellRotation);
    }
}

void CentralNexus::updatePulseEffects(float deltaTime) {
    if (m_pulseIntensity > 0.01f) {
        m_pulsePhase += deltaTime * 8.0f;
        m_pulseIntensity *= pow(0.95f, deltaTime * 60.0f); // Exponential decay
        
        // Apply pulse to core scale
        float pulseScale = 1.0f + m_pulseIntensity * 0.15f * sin(m_pulsePhase);
        if (m_coreStructure) {
            vec3 baseScale = make_vec3(1.5f);
            m_coreStructure->setScale(baseScale * pulseScale);
        }
        
        // Apply pulse to outer shell
        if (m_outerShell) {
            float shellPulse = 1.0f + m_pulseIntensity * 0.08f * sin(m_pulsePhase * 0.8f);
            vec3 shellScale = make_vec3(4.0f);
            m_outerShell->setScale(shellScale * shellPulse);
        }
        
        // Update ring pulse effects
        for (auto& ring : m_energyRings) {
            ring->setPulseIntensity(m_pulseIntensity);
        }
    }
}

void CentralNexus::updateEnergyFlow(float deltaTime) {
    // Update orbital energy streams
    static float orbitPhase = 0.0f;
    orbitPhase += deltaTime * m_connectionStrength;
    
    for (size_t i = 0; i < m_energyStreams.size(); ++i) {
        float streamPhase = orbitPhase + i * (2.0f * M_PI / m_energyStreams.size());
        float radius = 2.5f + sin(streamPhase * 0.5f) * 0.5f;
        float height = sin(streamPhase * 0.3f) * 1.0f;
        
        vec3 streamPos = make_vec3(
            cos(streamPhase) * radius,
            height,
            sin(streamPhase) * radius
        );
        
        m_energyStreams[i]->setEmitPosition(streamPos);
        
        // Set emission direction toward center
        vec3 direction = normalize(-streamPos);
        m_energyStreams[i]->setEmitDirection(direction);
    }
    
    // Update energy channels
    for (auto& channel : m_energyChannels) {
        channel->updateFlow(deltaTime, m_connectionStrength);
    }
    
    // Update main energy flow based on connection strength
    if (m_energyFlow) {
        float flowIntensity = m_connectionStrength * (1.0f + m_pulseIntensity * 0.5f);
        m_energyFlow->setEmitRate(20.0f + flowIntensity * 60.0f);
        
        // Modulate particle color
        vec4 flowColor = make_vec4(
            m_energyColor.x,
            m_energyColor.y,
            m_energyColor.z,
            flowIntensity
        );
        m_energyFlow->setParticleColor(flowColor);
    }
}

void CentralNexus::updateConnectionEffects(float deltaTime) {
    // Update connection beams
    for (auto it = m_connectionBeams.begin(); it != m_connectionBeams.end();) {
        (*it)->update(deltaTime);
        
        if ((*it)->isExpired()) {
            removeChild(*it);
            it = m_connectionBeams.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update shell patterns based on connection activity
    for (auto& pattern : m_shellPatterns) {
        float activity = m_connectionStrength * (1.0f + m_pulseIntensity);
        pattern->setIntensity(0.3f + activity * 0.5f);
        pattern->update(deltaTime);
    }
}

void CentralNexus::updateResonanceField(float deltaTime) {
    // Update resonance field effects based on current mode
    static float resonancePhase = 0.0f;
    resonancePhase += deltaTime;
    
    switch (m_resonanceMode) {
        case ResonanceMode::HARMONIC:
            // Smooth, wavelike resonance
            updateHarmonicResonance(resonancePhase);
            break;
            
        case ResonanceMode::CREATIVE:
            // Dynamic, chaotic resonance
            updateCreativeResonance(resonancePhase);
            break;
            
        case ResonanceMode::PROTECTIVE:
            // Steady, strong resonance
            updateProtectiveResonance(resonancePhase);
            break;
            
        case ResonanceMode::ALERT:
            // Sharp, urgent resonance
            updateAlertResonance(resonancePhase);
            break;
    }
}

void CentralNexus::updateHarmonicResonance(float phase) {
    // Create wavelike effects
    float wave = sin(phase * 2.0f) * 0.5f + 0.5f;
    
    // Modulate ring intensities
    for (size_t i = 0; i < m_energyRings.size(); ++i) {
        float ringPhase = phase + i * 0.5f;
        float intensity = 0.6f + 0.4f * sin(ringPhase);
        m_energyRings[i]->setGlowIntensity(intensity);
    }
}

void CentralNexus::updateCreativeResonance(float phase) {
    // Create chaotic, inspiring effects
    for (size_t i = 0; i < m_energyRings.size(); ++i) {
        float chaos = sin(phase * 3.0f + i) * cos(phase * 2.0f + i * 0.7f);
        float intensity = 0.7f + chaos * 0.3f;
        m_energyRings[i]->setGlowIntensity(abs(intensity));
    }
}

void CentralNexus::updateProtectiveResonance(float phase) {
    // Create steady, strong shield-like effects
    float shield = 0.8f + 0.2f * sin(phase * 0.5f);
    
    for (auto& ring : m_energyRings) {
        ring->setGlowIntensity(shield);
    }
    
    // Strengthen shell patterns
    for (auto& pattern : m_shellPatterns) {
        pattern->setIntensity(0.8f + shield * 0.2f);
    }
}

void CentralNexus::updateAlertResonance(float phase) {
    // Create urgent, pulsing effects
    float alert = (sin(phase * 6.0f) > 0.0f) ? 1.0f : 0.3f;
    
    for (auto& ring : m_energyRings) {
        ring->setGlowIntensity(alert);
    }
}

void CentralNexus::createShockwave(float intensity) {
    // Create expanding shockwave effect
    auto shockwave = std::make_shared<ShockwaveEffect>();
    shockwave->setPosition(getPosition());
    shockwave->setIntensity(intensity);
    shockwave->setMaxRadius(10.0f + intensity * 5.0f);
    shockwave->setDuration(1.5f);
    shockwave->setColor(m_energyColor);
    
    addChild(shockwave);
    
    // Remove after animation completes
    // TODO: Add timed removal system
}

void CentralNexus::updateRingRotationSpeeds() {
    // Update ring rotation speeds based on connection strength
    for (size_t i = 0; i < m_energyRings.size(); ++i) {
        float baseSpeed = 0.5f + i * 0.2f;
        float activitySpeed = m_connectionStrength * 0.8f;
        m_energyRings[i]->setRotationSpeed(baseSpeed + activitySpeed);
    }
}

void CentralNexus::updateCoreBrightness() {
    // Update core material emission based on connection strength
    if (m_coreStructure && m_coreStructure->getMaterial()) {
        float brightness = 1.5f + m_connectionStrength * 1.0f;
        auto material = m_coreStructure->getMaterial();
        material->setEmissionStrength(brightness);
    }
}

void CentralNexus::updateMaterialColors() {
    // Update all material colors to match current resonance mode
    if (m_coreStructure && m_coreStructure->getMaterial()) {
        auto material = m_coreStructure->getMaterial();
        material->setEmission(m_coreColor);
    }
    
    // Update ring colors
    for (auto& ring : m_energyRings) {
        ring->setColor(m_energyColor);
    }
    
    // Update particle system colors
    if (m_energyFlow) {
        vec4 flowColor = make_vec4(m_energyColor.x, m_energyColor.y, m_energyColor.z, 1.0f);
        m_energyFlow->setParticleColor(flowColor);
    }
    
    for (auto& stream : m_energyStreams) {
        vec4 streamColor = make_vec4(m_energyColor.x * 0.8f, m_energyColor.y * 0.8f, m_energyColor.z, 0.9f);
        stream->setParticleColor(streamColor);
    }
}

} // namespace FinalStorm