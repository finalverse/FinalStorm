// src/Services/ServiceNode.cpp
// Service node implementation
// Base class for service representations in the scene graph

#include "Services/ServiceNode.h"
#include "Core/Math/Math.h"
#include <cmath>

namespace FinalStorm {

ServiceNode::ServiceNode(const std::string& name)
    : name(name)
    , glowIntensity(1.0f)
    , pulseSpeed(1.0f)
    , baseColor(make_vec3(0.2f, 0.8f, 1.0f))
    , animationTime(0.0f)
    , activityLevel(0.0f)
    , healthLevel(1.0f) {
}

ServiceNode::~ServiceNode() = default;

void ServiceNode::updateMetrics(float activity, float health) {
    activityLevel = clamp(activity, 0.0f, 1.0f);
    healthLevel = clamp(health, 0.0f, 1.0f);
    
    // Update glow based on health
    glowIntensity = 0.5f + healthLevel * 0.5f;
    
    // Update pulse speed based on activity
    pulseSpeed = 0.5f + activityLevel * 2.0f;
}

void ServiceNode::update(float deltaTime) {
    animationTime += deltaTime;
    
    // Pulse effect
    float pulse = 1.0f + sinf(animationTime * pulseSpeed) * 0.1f * activityLevel;
    currentScale = make_vec3(pulse, pulse, pulse);
}

void ServiceNode::render(Renderer* renderer) {
    // Base implementation - derived classes can override
    // For now, this is just a placeholder
}

vec3 ServiceNode::getCurrentColor() const {
    // Calculate current color based on health and activity
    vec3 color = baseColor;
    
    // Modify color based on health (red = bad, green = good)
    if (healthLevel < 0.5f) {
        // Interpolate to red for low health
        float t = healthLevel * 2.0f; // 0-1 range
        color = lerp(make_vec3(1.0f, 0.0f, 0.0f), baseColor, t);
    } else {
        // Interpolate to green for high health
        float t = (healthLevel - 0.5f) * 2.0f; // 0-1 range
        color = lerp(baseColor, make_vec3(0.0f, 1.0f, 0.0f), t * 0.3f);
    }
    
    return color * glowIntensity;
}

} // namespace FinalStorm