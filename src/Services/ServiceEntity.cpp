// src/Services/ServiceEntity.cpp
// Service entity base class implementation
// Represents a Finalverse service as a 3D entity

#include "Services/ServiceEntity.h"
#include "Scene/SceneNode.h"

namespace FinalStorm {

ServiceEntity::ServiceEntity(ServiceType serviceType, const std::string& serviceName)
    : SceneNode(serviceName)
    , type(serviceType)
    , health(1.0f)
    , activityLevel(0.0f) {
}

ServiceEntity::~ServiceEntity() = default;

void ServiceEntity::updateMetrics(const ServiceMetrics& newMetrics) {
    metrics = newMetrics;
    
    // Update visual properties based on metrics
    health = 1.0f - (metrics.errorRate / 100.0f);
    activityLevel = std::min(1.0f, metrics.requestsPerSecond / 1000.0f);
    
    // Update visuals
    updateVisuals();
}

void ServiceEntity::onUpdate(float deltaTime) {
    // Base update - derived classes can override
    updateAnimation(deltaTime);
}

void ServiceEntity::updateAnimation(float deltaTime) {
    // Simple pulsing animation based on activity
    float targetScale = 1.0f + activityLevel * 0.2f;
    float3 currentScale = getLocalTransform().getScale();
    float3 newScale = currentScale + (float3(targetScale) - currentScale) * deltaTime * 5.0f;
    setScale(newScale);
}

void ServiceEntity::updateVisuals() {
    // Derived classes implement specific visual updates
}

} // namespace FinalStorm