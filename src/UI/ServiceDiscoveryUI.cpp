// src/UI/ServiceDiscoveryUI.cpp
// Service discovery UI implementation
// Interactive orb for discovering and connecting to services

#include "UI/ServiceDiscoveryUI.h"
#include "UI/InteractiveOrb.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

ServiceDiscoveryUI::ServiceDiscoveryUI()
    : SceneNode("Service Discovery")
    , isExpanded(false)
    , animationTime(0.0f) {
}

ServiceDiscoveryUI::~ServiceDiscoveryUI() = default;

void ServiceDiscoveryUI::onUpdate(float deltaTime) {
    animationTime += deltaTime;
    
    // Rotate the discovery orb
    setRotation(quaternion(animationTime * 0.5f, float3(0, 1, 0)));
}

void ServiceDiscoveryUI::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Render central orb
    float pulse = 0.8f + 0.2f * sinf(animationTime * 2.0f);
    context.setColor(float4(0.2f, 0.8f, 1.0f, 1.0f) * pulse);
    context.drawSphere(0.5f);
    
    // Render service icons if expanded
    if (isExpanded) {
        renderServiceRing(context);
    }
    
    context.popTransform();
}

void ServiceDiscoveryUI::toggleExpanded() {
    isExpanded = !isExpanded;
}

void ServiceDiscoveryUI::renderServiceRing(RenderContext& context) {
    const float radius = 3.0f;
    const int serviceCount = 8;
    
    for (int i = 0; i < serviceCount; ++i) {
        float angle = (float(i) / float(serviceCount)) * 2.0f * M_PI;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(x, 0, z));
        context.setColor(float4(0.5f, 0.8f, 1.0f, 0.8f));
        context.drawCube(0.3f);
        context.popTransform();
    }
}

} // namespace FinalStorm