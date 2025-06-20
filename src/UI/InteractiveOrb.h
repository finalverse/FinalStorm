// src/UI/InteractiveOrb.h
// Interactive orb UI element
// Clickable 3D orb for user interaction

#pragma once
#include "Scene/SceneNode.h"
#include <functional>

namespace FinalStorm {

class InteractiveOrb : public SceneNode {
public:
    InteractiveOrb(float radius = 0.5f);
    ~InteractiveOrb() override;
    
    void setOnActivate(std::function<void()> callback) { onActivate = callback; }
    void setGlowColor(const float3& color) { glowColor = color; }
    void setPulseSpeed(float speed) { pulseSpeed = speed; }
    
    void activate();
    
protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
    
private:
    float radius;
    float3 glowColor;
    float pulseSpeed;
    float pulsePhase;
    std::function<void()> onActivate;
};

} // namespace FinalStorm