// src/UI/InteractiveOrb.cpp
// Interactive orb UI element implementation
// Clickable 3D orb for user interaction

#include "UI/InteractiveOrb.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/MathTypes.h"
#include <cmath>

namespace FinalStorm {

InteractiveOrb::InteractiveOrb(float radius)
    : SceneNode("Interactive Orb")
    , m_radius(radius)
    , m_glowColor(make_vec3(0.2f, 0.8f, 1.0f))
    , m_pulseSpeed(2.0f)
    , m_pulsePhase(0.0f) {
}

InteractiveOrb::~InteractiveOrb() = default;

void InteractiveOrb::activate() {
    if (m_onActivate) {
        m_onActivate();
    }
    
    // Visual feedback
    m_pulsePhase = 0.0f;
    m_pulseSpeed = 8.0f; // Quick pulse on activation
}

void InteractiveOrb::onUpdate(float deltaTime) {
    m_pulsePhase += deltaTime * m_pulseSpeed;
    
    // Decay pulse speed back to normal
    if (m_pulseSpeed > 2.0f) {
        m_pulseSpeed = lerp(m_pulseSpeed, 2.0f, deltaTime * 3.0f);
    }
    
    // Gentle rotation
    quat rotation = simd_quaternion(deltaTime * 0.5f, make_vec3(0, 1, 0));
    setRotation(getRotation() * rotation);
}

void InteractiveOrb::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Pulsing glow effect
    float pulse = 0.8f + 0.2f * sinf(m_pulsePhase);
    vec4 color = make_vec4(m_glowColor.x, m_glowColor.y, m_glowColor.z, 1.0f);
    color *= pulse;
    
    context.setColor(color);
    context.drawSphere(m_radius);
    
    context.popTransform();
}

} // namespace FinalStorm