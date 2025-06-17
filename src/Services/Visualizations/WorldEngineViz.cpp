#include "Services/Visualizations/WorldEngineViz.h"

namespace FinalStorm {

WorldEngineViz::WorldEngineViz()
    : ServiceEntity(ServiceInfo::fromType(ServiceType::WORLD_ENGINE)) {
}

void WorldEngineViz::createVisualComponents() {
    ServiceEntity::createVisualComponents();
    // TODO: Add world engine-specific visuals
}

void WorldEngineViz::updateVisualEffects(float deltaTime) {
    ServiceEntity::updateVisualEffects(deltaTime);
    // TODO: Update world engine-specific effects
}

} // namespace FinalStorm
