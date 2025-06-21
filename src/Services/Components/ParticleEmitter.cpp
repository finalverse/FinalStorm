// ============================================================================
// File: FinalStorm/src/Services/Components/ParticleEmitter.cpp
// Component Systems Implementation - The Building Blocks of Finalverse
// ============================================================================

#include "Services/Components/ParticleEmitter.h"
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

}
