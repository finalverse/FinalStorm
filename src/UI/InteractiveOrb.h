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
    
    void setOnActivate(std::function<void()> callback) { m_onActivate = callback; }
    void setGlowColor(const vec3& color) { m_glowColor = color; }
    void setPulseSpeed(float speed) { m_pulseSpeed = speed; }
    
    void activate();
    
protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
    
private:
    float m_radius;
    vec3 m_glowColor;
    float m_pulseSpeed;
    float m_pulsePhase;
    std::function<void()> m_onActivate;
};

} // namespace FinalStorm