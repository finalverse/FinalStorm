// src/Services/Components/ParticleEmitter.cpp
// Particle emitter implementation
// Manages particle systems for visual effects

#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include <algorithm>

namespace FinalStorm {

ParticleEmitter::ParticleEmitter(int maxParticles)
    : maxParticleCount(maxParticles)
    , emissionRate(10.0f)
    , particleLifetime(2.0f)
    , emissionTimer(0.0f)
    , gravity(0, -1, 0)
    , startVelocity(0, 1, 0)
    , velocityVariation(0.5f, 0.5f, 0.5f)
    , startSize(0.1f)
    , endSize(0.05f)
    , startColor(1, 1, 1, 1)
    , endColor(1, 1, 1, 0) {
    
    particles.reserve(maxParticleCount);
}

ParticleEmitter::~ParticleEmitter() = default;

void ParticleEmitter::update(float deltaTime) {
    // Update existing particles
    for (auto& particle : particles) {
        particle.age += deltaTime;
        particle.velocity += gravity * deltaTime;
        particle.position += particle.velocity * deltaTime;
        
        // Update size and color based on age
        float t = particle.age / particle.lifetime;
        particle.size = Math::lerp(startSize, endSize, t);
        particle.color = startColor * (1.0f - t) + endColor * t;
    }
    
    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.age >= p.lifetime; }),
        particles.end()
    );
    
    // Emit new particles
    emissionTimer += deltaTime;
    float emissionInterval = 1.0f / emissionRate;
    
    while (emissionTimer >= emissionInterval && particles.size() < maxParticleCount) {
        emitParticle();
        emissionTimer -= emissionInterval;
    }
}

void ParticleEmitter::render(RenderContext& context) {
    for (const auto& particle : particles) {
        context.pushTransform(float4x4(1.0f));
        context.translate(particle.position);
        context.setColor(particle.color);
        context.drawSphere(particle.size);
        context.popTransform();
    }
}

void ParticleEmitter::emitParticle() {
    Particle particle;
    particle.position = float3(0, 0, 0); // Emit from origin
    particle.velocity = startVelocity + float3(
        (rand() / float(RAND_MAX) - 0.5f) * velocityVariation.x,
        (rand() / float(RAND_MAX) - 0.5f) * velocityVariation.y,
        (rand() / float(RAND_MAX) - 0.5f) * velocityVariation.z
    );
    particle.size = startSize;
    particle.color = startColor;
    particle.age = 0.0f;
    particle.lifetime = particleLifetime;
    
    particles.push_back(particle);
}

} // namespace FinalStorm