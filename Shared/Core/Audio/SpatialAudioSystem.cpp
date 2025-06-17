#include "SpatialAudioSystem.h"

namespace FinalStorm {

SpatialAudioSystem::SpatialAudioSystem(std::shared_ptr<AudioEngine> engine)
    : m_audioEngine(std::move(engine))
{
    if (!m_audioEngine)
        return;

    m_audioEngine->initialize();
    // Create ambient looping source
    m_ambientSource = m_audioEngine->createSource();
    if (m_ambientSource) {
        m_ambientSource->setLooping(true);
        if (auto clip = m_audioEngine->getAudioClip("ambient")) {
            m_ambientSource->setClip(clip);
        }
        m_ambientSource->play();
    }
}

void SpatialAudioSystem::registerService(const std::shared_ptr<ServiceNode>& service)
{
    if (!service || !m_audioEngine)
        return;

    auto source = m_audioEngine->createSource();
    if (source) {
        source->setLooping(true);
        if (auto clip = m_audioEngine->getAudioClip("service")) {
            source->setClip(clip);
        }
        source->play();
        m_serviceSources[service.get()] = source;
    }
}

void SpatialAudioSystem::unregisterService(const std::shared_ptr<ServiceNode>& service)
{
    if (!service || !m_audioEngine)
        return;

    auto it = m_serviceSources.find(service.get());
    if (it != m_serviceSources.end()) {
        m_audioEngine->removeSource(it->second);
        m_serviceSources.erase(it);
    }
}

void SpatialAudioSystem::update(float deltaTime,
                                const EnvironmentController& env,
                                const float3& listenerPos,
                                const float3& listenerForward)
{
    if (!m_audioEngine)
        return;

    // Update listener parameters
    m_audioEngine->setListenerPosition(listenerPos);
    m_audioEngine->setListenerOrientation(listenerForward, float3{0.f, 1.f, 0.f});

    // Update service source positions
    for (auto it = m_serviceSources.begin(); it != m_serviceSources.end(); ) {
        ServiceNode* node = it->first;
        auto& source = it->second;
        if (!node) {
            it = m_serviceSources.erase(it);
            continue;
        }
        source->setPosition(node->getWorldPosition());
        ++it;
    }

    // Ambient volume reacts to environment state
    if (m_ambientSource) {
        float volume = env.getState().ambientParticleRate / 50.0f;
        if (volume > 1.0f) volume = 1.0f;
        m_ambientSource->setVolume(volume);
    }

    m_audioEngine->update(deltaTime);
}

} // namespace FinalStorm
