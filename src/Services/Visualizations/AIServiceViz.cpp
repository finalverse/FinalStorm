#include "Services/Visualizations/AIServiceViz.h"

namespace FinalStorm {

AIServiceViz::AIServiceViz(ServiceType type) 
    : ServiceEntity(ServiceInfo::fromType(type)) {
}

void AIServiceViz::createVisualComponents() {
    ServiceEntity::createVisualComponents();
    // TODO: Add AI-specific visuals
}

void AIServiceViz::updateVisualEffects(float deltaTime) {
    ServiceEntity::updateVisualEffects(deltaTime);
    // TODO: Update AI-specific effects
}

} // namespace FinalStorm
