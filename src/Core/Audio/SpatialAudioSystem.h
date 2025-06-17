#pragma once

#include "AudioEngine.h"
#include "../../Scene/ServiceNode.h"
#include "../Environment/EnvironmentController.h"
#include <unordered_map>
#include <memory>

namespace FinalStorm {

// Simple manager that provides positional audio for services and ambient state.
class SpatialAudioSystem {
public:
    explicit SpatialAudioSystem(std::shared_ptr<AudioEngine> engine);

    // Register/unregister a service node to emit positional sound
    void registerService(const std::shared_ptr<ServiceNode>& service);
    void unregisterService(const std::shared_ptr<ServiceNode>& service);

    // Update all audio sources and listener
    void update(float deltaTime,
                const EnvironmentController& env,
                const float3& listenerPos,
                const float3& listenerForward);

private:
    std::shared_ptr<AudioEngine> m_audioEngine;
    std::unordered_map<ServiceNode*, std::shared_ptr<AudioSource>> m_serviceSources;
    std::shared_ptr<AudioSource> m_ambientSource;
};

} // namespace FinalStorm
