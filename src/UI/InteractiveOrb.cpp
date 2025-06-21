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
    , m_color(make_vec3(0.2f, 0.8f, 1.0f))
    , m_glowIntensity(1.0f)
    , m_basePulseRate(2.0f)
    , m_currentPulseRate(2.0f)
    , m_pulsePhase(0.0f)
    , m_floatEnabled(false)
    , m_floatSpeed(0.5f)
    , m_floatRange(0.5f)
    , m_floatPhase(0.0f)
    , m_basePosition(make_vec3(0.0f, 0.0f, 0.0f))
    , m_interactable(true)
    , m_holographic(false) {
}

InteractiveOrb::~InteractiveOrb() = default;

void InteractiveOrb::activate() {
    if (!m_interactable)
        return;

    if (m_onActivate) {
        m_onActivate();
    }

    // Visual feedback
    m_pulsePhase = 0.0f;
    m_currentPulseRate = m_basePulseRate * 4.0f; // Quick pulse on activation
}

void InteractiveOrb::onUpdate(float deltaTime) {
    m_pulsePhase += deltaTime * m_currentPulseRate;

    // Decay pulse rate back to base
    if (std::abs(m_currentPulseRate - m_basePulseRate) > 0.01f) {
        m_currentPulseRate = lerp(m_currentPulseRate, m_basePulseRate, deltaTime * 3.0f);
    }

    // Floating animation
    if (m_floatEnabled) {
        m_floatPhase += deltaTime * m_floatSpeed;
        vec3 pos = m_basePosition;
        pos.y += sinf(m_floatPhase) * m_floatRange;
        setPosition(pos);
    } else {
        m_basePosition = getPosition();
    }

    // Gentle rotation
    quat rotation = simd_quaternion(deltaTime * 0.5f, make_vec3(0, 1, 0));
    setRotation(getRotation() * rotation);
}

void InteractiveOrb::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());

    // Pulsing glow effect
    float pulse = 0.8f + 0.2f * sinf(m_pulsePhase);
    vec4 color = make_vec4(m_color.x, m_color.y, m_color.z, 1.0f);
    color *= m_glowIntensity * pulse;

    context.setColor(color);
    context.drawSphere(m_radius);

    context.popTransform();
}

void InteractiveOrb::enableFloat(bool enable) {
    m_floatEnabled = enable;
    if (enable) {
        m_basePosition = getPosition();
    }
}

} // namespace FinalStorm

