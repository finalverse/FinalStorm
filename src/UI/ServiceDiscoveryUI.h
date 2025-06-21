// src/UI/ServiceDiscoveryUI.h
// Service discovery UI header
// Interactive orb for discovering and connecting to services

#pragma once
#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"

namespace FinalStorm {

class RenderContext;

class ServiceDiscoveryUI : public SceneNode {
public:
    ServiceDiscoveryUI();
    virtual ~ServiceDiscoveryUI();
    
    void toggleExpanded();
    bool isExpanded() const { return m_isExpanded; }
    
protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
    
private:
    void renderServiceRing(RenderContext& context);
    
    bool m_isExpanded;
    float m_animationTime;
};

} // namespace FinalStorm