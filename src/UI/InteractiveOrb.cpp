// src/UI/InteractiveOrb.cpp
// Interactive orb implementation
// Handles user interaction and visual feedback

#include "UI/InteractiveOrb.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

InteractiveOrb::InteractiveOrb(float r)
    : SceneNode("Interactive Orb")
    , radius(r)
    , glowColor(0.2f, 0.8f, 1.0f)
    , pulseSpeed(2.0f)
    , pulsePhase(0.0f) {
}

InteractiveOrb::~InteractiveOrb() = default;

void InteractiveOrb::activate() {
    if (onActivate) {
        onActivate();
    }
}

void InteractiveOrb::onUpdate(float deltaTime) {
    pulsePhase += deltaTime * pulseSpeed;
}

void InteractiveOrb::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Calculate pulse
    float pulse = 0.8f + 0.2f * sinf(pulsePhase);
    
    // Render orb
    context.setColor(float4(glowColor.x, glowColor.y, glowColor.z, 1.0f) * pulse);
    context.drawSphere(radius);
    
    // Render glow effect
    context.setColor(float4(glowColor.x, glowColor.y, glowColor.z, 0.3f));
    context.drawSphere(radius * 1.2f);
    
    context.popTransform();
}

} // namespace FinalStorm