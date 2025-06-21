// src/UI/InteractiveOrb.h
// Interactive orb UI element
// Clickable 3D orb for user interaction

#pragma once
#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include <functional>

namespace FinalStorm {

class RenderContext;

class InteractiveOrb : public SceneNode {
public:
    InteractiveOrb(float radius = 0.5f);
    ~InteractiveOrb() override;

    // Basic properties
    void setRadius(float radius) { m_radius = radius; }
    void setColor(const vec3& color) { m_color = color; }
    void setGlowIntensity(float intensity) { m_glowIntensity = intensity; }
    void setPulseRate(float rate) { m_basePulseRate = rate; }

    // Animation controls
    void enableFloat(bool enable);
    void setFloatSpeed(float speed) { m_floatSpeed = speed; }
    void setFloatRange(float range) { m_floatRange = range; }

    // Interactivity
    void setInteractable(bool interactable) { m_interactable = interactable; }
    void setOnActivateCallback(std::function<void()> callback) { m_onActivate = callback; }

    // Visual effects
    void enableHolographicShader(bool enable) { m_holographic = enable; }

    void activate();
    
protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
    
private:
    // Visual properties
    float m_radius;
    vec3  m_color;
    float m_glowIntensity;

    // Pulse animation
    float m_basePulseRate;
    float m_currentPulseRate;
    float m_pulsePhase;

    // Floating animation
    bool  m_floatEnabled;
    float m_floatSpeed;
    float m_floatRange;
    float m_floatPhase;
    vec3  m_basePosition;

    // Interaction
    bool m_interactable;
    bool m_holographic;
    std::function<void()> m_onActivate;
};

} // namespace FinalStorm
