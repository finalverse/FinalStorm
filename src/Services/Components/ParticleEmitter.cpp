#include "Services/Components/ParticleEmitter.h"

namespace FinalStorm {

ParticleEmitter::ParticleEmitter(const Config& config) 
    : Entity("ParticleEmitter"), m_config(config) {
}

void ParticleEmitter::update(float deltaTime) {
    Entity::update(deltaTime);
    // TODO: Update particle positions
}

void ParticleEmitter::render(MetalRenderer* renderer) {
    // TODO: Render particles
}

void ParticleEmitter::burst(int count) {
    // TODO: Emit burst of particles
}

void ParticleEmitter::setEmitRate(float rate) {
    m_config.emitRate = rate;
}

} // namespace FinalStorm
