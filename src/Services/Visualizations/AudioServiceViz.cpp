#include "Services/Visualizations/AudioServiceViz.h"

namespace FinalStorm {

AudioServiceViz::AudioServiceViz(ServiceType type)
    : ServiceEntity(ServiceInfo::fromType(type)) {
}

void AudioServiceViz::createVisualComponents() {
    ServiceEntity::createVisualComponents();
    // TODO: Add audio-specific visuals
}

void AudioServiceViz::updateVisualEffects(float deltaTime) {
    ServiceEntity::updateVisualEffects(deltaTime);
    // TODO: Update audio-specific effects
}

} // namespace FinalStorm
