#pragma once

#include "Core/Math/MathTypes.h"
#include <string>

namespace FinalStorm {

// Forward declaration
class Renderer;

// Base class for service nodes in the scene graph
class ServiceNode {
public:
    ServiceNode(const std::string& name = "ServiceNode");
    virtual ~ServiceNode();
    
    // Update metrics that affect visualization
    void updateMetrics(float activity, float health);
    
    // Update animation
    virtual void update(float deltaTime);
    
    // Render the service visualization
    virtual void render(Renderer* renderer);
    
    // Getters
    const std::string& getName() const { return name; }
    float getGlowIntensity() const { return glowIntensity; }
    vec3 getCurrentColor() const;
    vec3 getCurrentScale() const { return currentScale; }
    
protected:
    std::string name;
    float glowIntensity;
    float pulseSpeed;
    vec3 baseColor;
    float animationTime;
    float activityLevel;
    float healthLevel;
    vec3 currentScale;
};

} // namespace FinalStorm