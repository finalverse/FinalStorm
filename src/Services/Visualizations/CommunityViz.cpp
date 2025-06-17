#include "Services/Visualizations/CommunityViz.h"

namespace FinalStorm {

CommunityViz::CommunityViz()
    : ServiceEntity(ServiceInfo::fromType(ServiceType::COMMUNITY)) {
}

void CommunityViz::createVisualComponents() {
    ServiceEntity::createVisualComponents();
    // TODO: Add community-specific visuals
}

void CommunityViz::updateVisualEffects(float deltaTime) {
    ServiceEntity::updateVisualEffects(deltaTime);
    // TODO: Update community-specific effects
}

} // namespace FinalStorm
