// ============================================================================
// File: FinalStorm/src/Services/Components/ParticleEmitter.cpp
// Component Systems Implementation - The Building Blocks of Finalverse
// ============================================================================

#include "Services/Components/ParticleEmitter.h"
#include "Services/Components/ConnectionBeam.h"
#include "Services/Components/EnergyRing.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Math.h"
#include <iostream>
#include <algorithm>
#include <random>

namespace FinalStorm {

// ============================================================================
// ParticleEmitter Implementation - Advanced particle system
// ============================================================================

ParticleEmitter::ParticleEmitter(const Config& config) 
    : SceneNode("Particle Emitter")
    , m_config(config)
    , m_isEmitting(true)
    , m_emissionTime(0.0f)
    , m_particleCount(0)
    , m_windForce(vec3_zero())
    , m_globalTime(0.0f) {
    
    // Reserve particle memory for better performance
    m_particles.reserve(1000);
    
    // Initialize random number generator
    m_randomEngine.seed(std::random_device{}());
    
    std::cout << "ParticleEmitter created with emission rate: " << m_config.emitRate << std::endl;
}

ParticleEmitter::~ParticleEmitter() = default;

void ParticleEmitter::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    m_globalTime += deltaTime;
    
    updateEmission(deltaTime);
    updateParticles(deltaTime);
    removeDeadParticles();
}

void ParticleEmitter::render(RenderContext& context) {
    if (m_particles.empty()) return;
    
    // Set up particle rendering state
    context.pushBlendMode(BlendMode::ADDITIVE);
    context.enableDepthWrite(false);
    
    // Render particles as point sprites or quads
    renderParticles(context);
    
    context.enableDepthWrite(true);
    context.popBlendMode();
}

void ParticleEmitter::burst(int count) {
    for (int i = 0; i < count; ++i) {
        emitParticle();
    }
    
    std::cout << "Particle burst: " << count << " particles emitted." << std::endl;
}

void ParticleEmitter::setEmitRate(float rate) {
    m_config.emitRate = std::max(0.0f, rate);
}

void ParticleEmitter::setParticleColor(const vec4& color) {
    m_config.startColor = color;
}

void ParticleEmitter::setEmitPosition(const vec3& position) {
    m_config.emitPosition = position;
}

void ParticleEmitter::setEmitDirection(const vec3& direction) {
    m_config.emitDirection = normalize(direction);
}

void ParticleEmitter::setEmitLine(const vec3& start, const vec3& end) {
    m_config.emitShape = Shape::LINE;
    m_config.lineStart = start;
    m_config.lineEnd = end;
}

void ParticleEmitter::addForce(const vec3& force) {
    m_windForce = m_windForce + force;
}

void ParticleEmitter::setWindForce(const vec3& wind) {
    m_windForce = wind;
}

void ParticleEmitter::stop() {
    m_isEmitting = false;
}

void ParticleEmitter::start() {
    m_isEmitting = true;
}

// Private implementation methods

void ParticleEmitter::updateEmission(float deltaTime) {
    if (!m_isEmitting || m_config.emitRate <= 0.0f) return;
    
    m_emissionTime += deltaTime;
    
    // Calculate number of particles to emit this frame
    float emissionInterval = 1.0f / m_config.emitRate;
    int particlesToEmit = static_cast<int>(m_emissionTime / emissionInterval);
    
    if (particlesToEmit > 0) {
        m_emissionTime -= particlesToEmit * emissionInterval;
        
        for (int i = 0; i < particlesToEmit; ++i) {
            emitParticle();
        }
    }
}

void ParticleEmitter::emitParticle() {
    if (m_particles.size() >= 1000) return; // Max particle limit
    
    Particle particle;
    
    // Set emission position based on shape
    particle.position = calculateEmissionPosition();
    particle.velocity = calculateEmissionVelocity();
    
    // Set particle properties
    particle.life = m_config.particleLifetime;
    particle.maxLife = m_config.particleLifetime;
    particle.size = m_config.startSize;
    particle.color = m_config.startColor;
    particle.rotation = 0.0f;
    particle.rotationSpeed = generateRandomFloat(-2.0f, 2.0f);
    
    // Add some variation
    if (m_config.sizeVariation > 0.0f) {
        float variation = generateRandomFloat(-m_config.sizeVariation, m_config.sizeVariation);
        particle.size *= (1.0f + variation);
    }
    
    if (m_config.colorVariation > 0.0f) {
        vec3 colorVar = make_vec3(
            generateRandomFloat(-m_config.colorVariation, m_config.colorVariation),
            generateRandomFloat(-m_config.colorVariation, m_config.colorVariation),
            generateRandomFloat(-m_config.colorVariation, m_config.colorVariation)
        );
        particle.color = make_vec4(
            clamp(particle.color.x + colorVar.x, 0.0f, 1.0f),
            clamp(particle.color.y + colorVar.y, 0.0f, 1.0f),
            clamp(particle.color.z + colorVar.z, 0.0f, 1.0f),
            particle.color.w
        );
    }
    
    m_particles.push_back(particle);
    m_particleCount++;
}

vec3 ParticleEmitter::calculateEmissionPosition() {
    vec3 worldPos = getWorldPosition();
    vec3 localPos = m_config.emitPosition;
    
    switch (m_config.emitShape) {
        case Shape::POINT:
            return worldPos + localPos;
            
        case Shape::SPHERE: {
            // Generate random point on/in sphere
            float theta = generateRandomFloat(0.0f, 2.0f * M_PI);
            float phi = generateRandomFloat(0.0f, M_PI);
            float radius = generateRandomFloat(0.0f, m_config.emitRadius);
            
            vec3 spherePoint = make_vec3(
                radius * sin(phi) * cos(theta),
                radius * cos(phi),
                radius * sin(phi) * sin(theta)
            );
            return worldPos + localPos + spherePoint;
        }
        
        case Shape::BOX: {
            vec3 boxPoint = make_vec3(
                generateRandomFloat(-m_config.emitSize.x * 0.5f, m_config.emitSize.x * 0.5f),
                generateRandomFloat(-m_config.emitSize.y * 0.5f, m_config.emitSize.y * 0.5f),
                generateRandomFloat(-m_config.emitSize.z * 0.5f, m_config.emitSize.z * 0.5f)
            );
            return worldPos + localPos + boxPoint;
        }
        
        case Shape::LINE: {
            float t = generateRandomFloat(0.0f, 1.0f);
            vec3 linePoint = lerp(m_config.lineStart, m_config.lineEnd, t);
            return linePoint;
        }
        
        case Shape::CIRCLE: {
            float angle = generateRandomFloat(0.0f, 2.0f * M_PI);
            float radius = generateRandomFloat(0.0f, m_config.emitRadius);
            vec3 circlePoint = make_vec3(
                radius * cos(angle),
                0.0f,
                radius * sin(angle)
            );
            return worldPos + localPos + circlePoint;
        }
        
        default:
            return worldPos + localPos;
    }
}

vec3 ParticleEmitter::calculateEmissionVelocity() {
    vec3 baseVelocity = m_config.emitDirection * m_config.velocity;
    
    // Add velocity variation
    if (m_config.velocityVariation > 0.0f) {
        vec3 variation = make_vec3(
            generateRandomFloat(-1.0f, 1.0f),
            generateRandomFloat(-1.0f, 1.0f),
            generateRandomFloat(-1.0f, 1.0f)
        );
        variation = normalize(variation) * m_config.velocityVariation * m_config.velocity;
        baseVelocity = baseVelocity + variation;
    }
    
    return baseVelocity;
}

void ParticleEmitter::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        // Update life
        particle.life -= deltaTime;
        
        if (particle.life <= 0.0f) continue;
        
        // Calculate life ratio for interpolation
        float lifeRatio = particle.life / particle.maxLife;
        float progress = 1.0f - lifeRatio;
        
        // Update position
        particle.position = particle.position + particle.velocity * deltaTime;
        
        // Apply forces
        vec3 totalForce = m_config.gravity + m_windForce;
        
        // Add turbulence
        if (m_config.turbulence > 0.0f) {
            vec3 turbulenceForce = calculateTurbulence(particle.position, m_globalTime) * m_config.turbulence;
            totalForce = totalForce + turbulenceForce;
        }
        
        particle.velocity = particle.velocity + totalForce * deltaTime;
        
        // Update size
        particle.size = lerp(m_config.startSize, m_config.endSize, progress);
        
        // Update color
        particle.color = lerpVec4(m_config.startColor, m_config.endColor, progress);
        
        // Update rotation
        particle.rotation += particle.rotationSpeed * deltaTime;
        
        // Apply velocity inheritance for moving emitters
        if (m_config.inheritVelocity > 0.0f) {
            // This would require tracking emitter velocity
            // Simplified implementation
        }
    }
}

vec3 ParticleEmitter::calculateTurbulence(const vec3& position, float time) {
    // Simple Perlin-like noise for turbulence
    float x = sin(position.x * 0.1f + time * 2.0f) * cos(position.y * 0.08f + time * 1.5f);
    float y = cos(position.y * 0.1f + time * 1.8f) * sin(position.z * 0.07f + time * 2.2f);
    float z = sin(position.z * 0.1f + time * 1.9f) * cos(position.x * 0.09f + time * 1.7f);
    
    return make_vec3(x, y, z);
}

void ParticleEmitter::removeDeadParticles() {
    auto it = std::remove_if(m_particles.begin(), m_particles.end(),
        [](const Particle& p) { return p.life <= 0.0f; });
    
    if (it != m_particles.end()) {
        m_particles.erase(it, m_particles.end());
    }
}

void ParticleEmitter::renderParticles(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    for (const auto& particle : m_particles) {
        context.pushTransform(mat4_identity());
        context.translate(particle.position);
        
        // Billboard rotation to face camera
        context.billboardToCamera();
        
        // Apply particle rotation
        if (particle.rotation != 0.0f) {
            context.rotate(simd_quaternion(particle.rotation, make_vec3(0, 0, 1)));
        }
        
        // Set particle color and render
        context.setColor(particle.color);
        context.drawParticleQuad(particle.size);
        
        context.popTransform();
    }
    
    context.popTransform();
}

float ParticleEmitter::generateRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_randomEngine);
}

vec4 ParticleEmitter::lerpVec4(const vec4& a, const vec4& b, float t) {
    return make_vec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
}

// ============================================================================
// ConnectionBeam Implementation - Dynamic connection visualization
// ============================================================================

ConnectionBeam::ConnectionBeam() : SceneNode("Connection Beam") {
    m_startPosition = vec3_zero();
    m_endPosition = make_vec3(0, 0, 1);
    m_color = make_vec3(0.4f, 0.8f, 1.0f);
    m_intensity = 1.0f;
    m_thickness = 0.02f;
    m_flowSpeed = 2.0f;
    m_flowDirection = 1.0f;
    m_duration = 0.0f;
    m_maxDuration = 0.0f;
    m_isExpired = false;
    m_connectionId = 0;
    m_flowPhase = 0.0f;
    m_pulsePhase = 0.0f;
    m_segments = 32;
    
    createBeamGeometry();
    createFlowEffects();
    
    std::cout << "ConnectionBeam created." << std::endl;
}

ConnectionBeam::~ConnectionBeam() = default;

void ConnectionBeam::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateFlow(deltaTime);
    updatePulse(deltaTime);
    updateDuration(deltaTime);
    updateGeometry();
}

void ConnectionBeam::render(RenderContext& context) {
    if (m_isExpired) return;
    
    context.pushTransform(getWorldMatrix());
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    renderBeam(context);
    renderFlowEffects(context);
    
    context.popBlendMode();
    context.popTransform();
}

void ConnectionBeam::setStartPosition(const vec3& pos) {
    m_startPosition = pos;
    m_geometryDirty = true;
}

void ConnectionBeam::setEndPosition(const vec3& pos) {
    m_endPosition = pos;
    m_geometryDirty = true;
}

void ConnectionBeam::setColor(const vec3& color) {
    m_color = color;
}

void ConnectionBeam::setIntensity(float intensity) {
    m_intensity = clamp(intensity, 0.0f, 2.0f);
}

void ConnectionBeam::setThickness(float thickness) {
    m_thickness = std::max(0.001f, thickness);
    m_geometryDirty = true;
}

void ConnectionBeam::setFlowSpeed(float speed) {
    m_flowSpeed = speed;
}

void ConnectionBeam::setFlowDirection(float direction) {
    m_flowDirection = direction;
}

void ConnectionBeam::setDuration(float duration) {
    m_maxDuration = duration;
    m_duration = 0.0f;
    m_isExpired = false;
}

void ConnectionBeam::setConnectionId(uint32_t id) {
    m_connectionId = id;
}

void ConnectionBeam::pulse(float intensity) {
    m_pulsePhase = 0.0f;
    m_pulseIntensity = intensity;
}

bool ConnectionBeam::isExpired() const {
    return m_isExpired;
}

// Private implementation methods

void ConnectionBeam::createBeamGeometry() {
    // Create beam mesh with segments for smooth curves
    m_beamMesh = std::make_shared<BeamMesh>();
    m_beamMesh->setSegmentCount(m_segments);
    updateBeamMesh();
    
    // Create beam material
    m_beamMaterial = std::make_shared<Material>("connection_beam");
    m_beamMaterial->setAlbedo(m_color);
    m_beamMaterial->setEmission(m_color);
    m_beamMaterial->setEmissionStrength(m_intensity);
    m_beamMaterial->setAlpha(0.8f);
}

void ConnectionBeam::createFlowEffects() {
    // Create flow particle system
    ParticleEmitter::Config flowConfig;
    flowConfig.emitShape = ParticleEmitter::Shape::LINE;
    flowConfig.emitRate = 20.0f;
    flowConfig.particleLifetime = 1.0f;
    flowConfig.startSize = 0.015f;
    flowConfig.endSize = 0.005f;
    flowConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 1.0f);
    flowConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    flowConfig.velocity = 3.0f;
    flowConfig.inheritVelocity = 0.8f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(flowConfig);
    m_flowParticles->setName("Beam Flow");
    addChild(m_flowParticles);
    
    updateFlowLine();
}

void ConnectionBeam::updateFlow(float deltaTime) {
    m_flowPhase += deltaTime * m_flowSpeed * m_flowDirection;
    
    // Update flow particle emission
    if (m_flowParticles) {
        updateFlowLine();
        
        // Modulate emission based on intensity
        float emissionRate = 15.0f * m_intensity;
        m_flowParticles->setEmitRate(emissionRate);
        
        // Update particle color
        vec4 flowColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.8f * m_intensity);
        m_flowParticles->setParticleColor(flowColor);
    }
}

void ConnectionBeam::updatePulse(float deltaTime) {
    if (m_pulseIntensity > 0.0f) {
        m_pulsePhase += deltaTime * 6.0f;
        m_pulseIntensity *= pow(0.95f, deltaTime * 60.0f); // Exponential decay
    }
}

void ConnectionBeam::updateDuration(float deltaTime) {
    if (m_maxDuration > 0.0f) {
        m_duration += deltaTime;
        if (m_duration >= m_maxDuration) {
            m_isExpired = true;
        }
    }
}

void ConnectionBeam::updateGeometry() {
    if (m_geometryDirty) {
        updateBeamMesh();
        updateFlowLine();
        m_geometryDirty = false;
    }
}

void ConnectionBeam::updateBeamMesh() {
    if (!m_beamMesh) return;
    
    // Calculate beam parameters
    vec3 direction = m_endPosition - m_startPosition;
    float length = magnitude(direction);
    vec3 normalizedDir = normalize(direction);
    
    // Generate beam vertices with slight curve for organic feel
    std::vector<vec3> centerLine;
    for (int i = 0; i <= m_segments; ++i) {
        float t = i / float(m_segments);
        vec3 point = lerp(m_startPosition, m_endPosition, t);
        
        // Add subtle curve
        float curvature = sin(t * M_PI) * length * 0.02f;
        vec3 perpendicular = cross(normalizedDir, make_vec3(0, 1, 0));
        if (magnitude(perpendicular) < 0.1f) {
            perpendicular = cross(normalizedDir, make_vec3(1, 0, 0));
        }
        perpendicular = normalize(perpendicular);
        
        point = point + perpendicular * curvature;
        centerLine.push_back(point);
    }
    
    m_beamMesh->updateCenterLine(centerLine, m_thickness);
}

void ConnectionBeam::updateFlowLine() {
    if (m_flowParticles) {
        m_flowParticles->setEmitLine(m_startPosition, m_endPosition);
        
        // Set flow direction
        vec3 flowDir = normalize(m_endPosition - m_startPosition);
        if (m_flowDirection < 0.0f) {
            flowDir = -flowDir;
        }
        m_flowParticles->setEmitDirection(flowDir);
    }
}

void ConnectionBeam::renderBeam(RenderContext& context) {
    if (!m_beamMesh) return;
    
    // Calculate current intensity with pulse
    float currentIntensity = m_intensity;
    if (m_pulseIntensity > 0.0f) {
        currentIntensity += m_pulseIntensity * sin(m_pulsePhase);
    }
    
    // Apply fade-out if expiring
    if (m_maxDuration > 0.0f && m_duration > 0.0f) {
        float fadeRatio = 1.0f - (m_duration / m_maxDuration);
        currentIntensity *= fadeRatio;
    }
    
    // Set beam color and intensity
    vec4 beamColor = make_vec4(m_color.x, m_color.y, m_color.z, currentIntensity);
    context.setColor(beamColor);
    context.setEmission(m_color * currentIntensity);
    
    // Render beam mesh
    m_beamMesh->render(context);
}

void ConnectionBeam::renderFlowEffects(RenderContext& context) {
    // Flow effects are rendered by the particle system automatically
    // Additional flow visualization could be added here
    
    // Render flow indicators along the beam
    if (m_intensity > 0.5f) {
        renderFlowIndicators(context);
    }
}

void ConnectionBeam::renderFlowIndicators(RenderContext& context) {
    // Render small indicators moving along the beam
    int indicatorCount = 3;
    vec3 direction = m_endPosition - m_startPosition;
    float length = magnitude(direction);
    
    for (int i = 0; i < indicatorCount; ++i) {
        float offset = (i / float(indicatorCount)) + m_flowPhase * 0.1f;
        offset = fmod(offset, 1.0f);
        
        if (m_flowDirection < 0.0f) {
            offset = 1.0f - offset;
        }
        
        vec3 indicatorPos = m_startPosition + direction * offset;
        
        context.pushTransform(mat4_identity());
        context.translate(indicatorPos);
        
        float indicatorIntensity = m_intensity * 0.8f;
        vec4 indicatorColor = make_vec4(m_color.x, m_color.y, m_color.z, indicatorIntensity);
        context.setColor(indicatorColor);
        
        context.drawGlowSphere(0.03f);
        
        context.popTransform();
    }
}

// ============================================================================
// EnergyRing Implementation - Rotating energy rings for the nexus
// ============================================================================

EnergyRing::EnergyRing() : SceneNode("Energy Ring") {
    m_innerRadius = 2.0f;
    m_outerRadius = 2.2f;
    m_height = 0.1f;
    m_color = make_vec3(0.4f, 0.8f, 1.0f);
    m_glowIntensity = 1.0f;
    m_rotationSpeed = 1.0f;
    m_flowDirection = 1.0f;
    m_segments = 64;
    m_pulseIntensity = 0.0f;
    m_pulsePhase = 0.0f;
    m_rotationPhase = 0.0f;
    m_rotationAxis = make_vec3(0, 1, 0);
    m_distortionAmount = 0.0f;
    
    createRingGeometry();
    createRingEffects();
    
    std::cout << "EnergyRing created with radius: " << m_innerRadius << std::endl;
}

EnergyRing::~EnergyRing() = default;

void EnergyRing::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateRotation(deltaTime);
    updatePulse(deltaTime);
    updateDistortion(deltaTime);
    updateEffects(deltaTime);
}

void EnergyRing::render(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    renderRing(context);
    renderGlow(context);
    
    context.popBlendMode();
    context.popTransform();
}

void EnergyRing::setInnerRadius(float radius) {
    m_innerRadius = std::max(0.1f, radius);
    m_geometryDirty = true;
}

void EnergyRing::setOuterRadius(float radius) {
    m_outerRadius = std::max(m_innerRadius + 0.05f, radius);
    m_geometryDirty = true;
}

void EnergyRing::setHeight(float height) {
    m_height = std::max(0.01f, height);
    m_geometryDirty = true;
}

void EnergyRing::setColor(const vec3& color) {
    m_color = color;
    updateMaterialColor();
}

void EnergyRing::setGlowIntensity(float intensity) {
    m_glowIntensity = clamp(intensity, 0.0f, 2.0f);
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

void EnergyRing::triggerRipple(float intensity) {
    // Create ripple effect
    auto ripple = std::make_shared<RingRipple>();
    ripple->setRadius(m_innerRadius);
    ripple->setIntensity(intensity);
    ripple->setColor(m_color);
    ripple->setDuration(1.5f);
    
    addChild(ripple);
    
    // Trigger pulse
    setPulseIntensity(intensity);
    
    std::cout << "Ring ripple triggered with intensity: " << intensity << std::endl;
}

void EnergyRing::setPulseIntensity(float intensity) {
    m_pulseIntensity = clamp(intensity, 0.0f, 2.0f);
    m_pulsePhase = 0.0f;
}

void EnergyRing::setDistortionAmount(float amount) {
    m_distortionAmount = clamp(amount, 0.0f, 1.0f);
}

// Private implementation methods

void EnergyRing::createRingGeometry() {
    // Create ring mesh
    m_ringMesh = std::make_shared<RingMesh>();
    m_ringMesh->setInnerRadius(m_innerRadius);
    m_ringMesh->setOuterRadius(m_outerRadius);
    m_ringMesh->setHeight(m_height);
    m_ringMesh->setSegments(m_segments);
    m_ringMesh->build();
    
    // Create ring material
    m_ringMaterial = std::make_shared<Material>("energy_ring");
    m_ringMaterial->setAlbedo(m_color);
    m_ringMaterial->setEmission(m_color);
    m_ringMaterial->setEmissionStrength(m_glowIntensity);
    m_ringMaterial->setAlpha(0.7f);
    m_ringMaterial->setMetallic(0.1f);
    m_ringMaterial->setRoughness(0.2f);
}

void EnergyRing::createRingEffects() {
    // Create energy flow particles around the ring
    ParticleEmitter::Config flowConfig;
    flowConfig.emitShape = ParticleEmitter::Shape::CIRCLE;
    flowConfig.emitRadius = (m_innerRadius + m_outerRadius) * 0.5f;
    flowConfig.emitRate = 25.0f;
    flowConfig.particleLifetime = 2.0f;
    flowConfig.startSize = 0.02f;
    flowConfig.endSize = 0.005f;
    flowConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.8f);
    flowConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    flowConfig.velocity = 1.5f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(flowConfig);
    m_flowParticles->setName("Ring Flow");
    addChild(m_flowParticles);
    
    // Create spark effects
    createSparkEffects();
}

void EnergyRing::createSparkEffects() {
    // Occasional sparks along the ring
    ParticleEmitter::Config sparkConfig;
    sparkConfig.emitShape = ParticleEmitter::Shape::CIRCLE;
    sparkConfig.emitRadius = m_outerRadius;
    sparkConfig.emitRate = 5.0f;
    sparkConfig.particleLifetime = 0.8f;
    sparkConfig.startSize = 0.03f;
    sparkConfig.endSize = 0.01f;
    sparkConfig.startColor = make_vec4(1.0f, 1.0f, 1.0