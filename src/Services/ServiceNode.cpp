// src/Services/ServiceNode.cpp
// Service node implementation
// Base class for service representations in the scene graph

#include "Services/ServiceNode.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

ServiceNode::ServiceNode(const std::string& name)
    : SceneNode(name)
    , glowIntensity(1.0f)
    , pulseSpeed(1.0f)
    , baseColor(0.2f, 0.8f, 1.0f)
    , animationTime(0.0f) {
}

ServiceNode::~ServiceNode() = default;

void ServiceNode::updateMetrics(float activity, float health) {
    activityLevel = Math::clamp(activity, 0.0f, 1.0f);
    healthLevel = Math::clamp(health, 0.0f, 1.0f);
    
    // Update glow based on health
    glowIntensity = 0.5f + healthLevel * 0.5f;
    
    // Update pulse speed based on activity
    pulseSpeed = 0.5f + activityLevel * 2.0f;
}

void ServiceNode::onUpdate(float deltaTime) {
    animationTime += deltaTime;
    
    // Pulse effect
    float pulse = 1.0f + sinf(animationTime * pulseSpeed) * 0.1f * activityLevel;
    setScale(float3(pulse, pulse, pulse));
}

void ServiceNode::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Set color based on health
    float4 color = float4(baseColor.x, baseColor.y, baseColor.z, 1.0f);
    color *= glowIntensity;
    context.setColor(color);
    
    // Default cube rendering - derived classes override this
    context.drawCube(1.0f);
    
    context.popTransform();
}

} // namespace FinalStorm