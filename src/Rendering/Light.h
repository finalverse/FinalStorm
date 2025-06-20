// Rendering/Light.h
#pragma once

#include "Core/Math/MathTypes.h"
#include "Core/Transform.h"

namespace FinalStorm {

enum class LightType {
    Directional,
    Point,
    Spot
};

class Light {
public:
    Light(LightType type = LightType::Point);
    virtual ~Light() = default;
    
    // Type
    LightType getType() const { return m_type; }
    
    // Transform
    Transform& getTransform() { return m_transform; }
    const Transform& getTransform() const { return m_transform; }
    
    void setPosition(const vec3& position) { m_transform.setPosition(position); }
    vec3 getPosition() const { return m_transform.getPosition(); }
    
    void setDirection(const vec3& direction);
    vec3 getDirection() const;
    
    // Color and intensity
    void setColor(const vec3& color) { m_color = color; }
    const vec3& getColor() const { return m_color; }
    
    void setIntensity(float intensity) { m_intensity = intensity; }
    float getIntensity() const { return m_intensity; }
    
    vec3 getFinalColor() const { return m_color * m_intensity; }
    
    // Attenuation (for point and spot lights)
    void setRange(float range) { m_range = range; }
    float getRange() const { return m_range; }
    
    void setAttenuation(float constant, float linear, float quadratic);
    void getAttenuation(float& constant, float& linear, float& quadratic) const;
    
    // Spot light parameters
    void setSpotAngle(float innerAngle, float outerAngle);
    float getInnerSpotAngle() const { return m_innerSpotAngle; }
    float getOuterSpotAngle() const { return m_outerSpotAngle; }
    
    // Shadows
    void setCastShadows(bool castShadows) { m_castShadows = castShadows; }
    bool getCastShadows() const { return m_castShadows; }
    
    void setShadowBias(float bias) { m_shadowBias = bias; }
    float getShadowBias() const { return m_shadowBias; }
    
    // Enable/disable
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
    // For GPU buffer
    struct GPUData {
        vec4 positionAndType;     // xyz = position, w = type
        vec4 directionAndRange;   // xyz = direction, w = range
        vec4 colorAndIntensity;   // xyz = color, w = intensity
        vec4 attenuationAndSpot;  // x = constant, y = linear, z = quadratic, w = cos(spotAngle)
        vec4 spotParams;          // x = cos(innerAngle), y = cos(outerAngle), zw = unused
    };
    
    GPUData getGPUData() const;
    
private:
    LightType m_type;
    Transform m_transform;
    
    vec3 m_color = vec3(1.0f);
    float m_intensity = 1.0f;
    
    // Attenuation
    float m_range = 10.0f;
    float m_constantAttenuation = 1.0f;
    float m_linearAttenuation = 0.09f;
    float m_quadraticAttenuation = 0.032f;
    
    // Spot light
    float m_innerSpotAngle = 30.0f;
    float m_outerSpotAngle = 45.0f;
    
    // Shadows
    bool m_castShadows = false;
    float m_shadowBias = 0.005f;
    
    bool m_enabled = true;
};

// Common light setups
class DirectionalLight : public Light {
public:
    DirectionalLight() : Light(LightType::Directional) {
        setDirection(vec3(0, -1, 0));
        setIntensity(1.0f);
    }
};

class PointLight : public Light {
public:
    PointLight() : Light(LightType::Point) {
        setRange(10.0f);
        setIntensity(1.0f);
    }
};

class SpotLight : public Light {
public:
    SpotLight() : Light(LightType::Spot) {
        setDirection(vec3(0, -1, 0));
        setSpotAngle(30.0f, 45.0f);
        setRange(10.0f);
        setIntensity(1.0f);
    }
};

} // namespace FinalStorm