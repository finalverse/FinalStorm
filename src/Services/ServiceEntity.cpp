// src/Services/ServiceEntity.cpp
// Service entity base class implementation
// Represents a Finalverse service as a 3D entity

#include "Services/ServiceEntity.h"
#include "Core/Math/Math.h"
//#include "Core/Math/Constants.h"
#include <algorithm>
#include <cmath>

namespace FinalStorm {

ServiceEntity::ServiceEntity(const ServiceInfo& info)
    : Entity(EntityType::Object)
    , serviceInfo(info)
    , activityLevel(0.0f)
    , healthLevel(1.0f)
    , animationTime(0.0f) {
    
    setMeshName("service_" + info.name);
    
    // Set initial position based on service type (can be overridden)
    float angle = static_cast<float>(info.type) * (2.0f * PI / 12.0f); // 12 service types
    float radius = 10.0f;
    transform.setPosition(make_vec3(
        cos(angle) * radius,
        0.0f,
        sin(angle) * radius
    ));
}

void ServiceEntity::updateMetrics(const ServiceMetrics& newMetrics) {
    metrics = newMetrics;
    
    // Update derived values based on metrics
    healthLevel = std::max(0.0f, 1.0f - (metrics.errorRate / 100.0f));
    activityLevel = std::min(1.0f, metrics.requestsPerSecond / 1000.0f);
    
    // Update visuals based on new metrics
    updateVisuals();
}

void ServiceEntity::update(float deltaTime) {
    Entity::update(deltaTime);
    
    animationTime += deltaTime;
    updateAnimation(deltaTime);
}

void ServiceEntity::updateAnimation(float deltaTime) {
    // Simple pulsing animation based on activity
    float targetScale = 1.0f + activityLevel * 0.2f;
    vec3 currentScale = transform.scale;
    vec3 targetScaleVec = make_vec3(targetScale, targetScale, targetScale);
    vec3 newScale = lerp(currentScale, targetScaleVec, deltaTime * 5.0f);
    transform.setScale(newScale);
    
    // Gentle rotation based on health
    float rotationSpeed = healthLevel * 0.5f;
    transform.rotateEuler(make_vec3(0.0f, rotationSpeed * deltaTime, 0.0f));
}

void ServiceEntity::updateVisuals() {
    // Base implementation - derived classes can override for specific visual effects
    // This method is called when metrics change
}

} // namespace FinalStorm