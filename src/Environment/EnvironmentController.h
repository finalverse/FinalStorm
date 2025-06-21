#pragma once

#include "Core/Math/Math.h"
#include "Scene/SceneNode.h"
#include <string>

namespace FinalStorm {

class RenderContext;

struct EnvironmentState {
    float3 skyboxTint;
    float ambientParticleRate;
    float groundPulse;
    float energyLevel;
    float harmonyLevel;
};

class EnvironmentController : public SceneNode {
public:
    EnvironmentController();
    ~EnvironmentController() override;

    // Update environment parameters from a normalized health score [0,1]
    void updateFromHealth(float healthScore);

    const EnvironmentState& getState() const { return m_state; }

    // Environment customization
    void setSkyboxTheme(const std::string& theme);
    void setGroundGridStyle(const std::string& style);
    void setAmbientParticles(bool enabled);
    void setQuantumFluctuations(float amount);
    void setEnergyLevel(float level);
    void setHarmonyLevel(float level);

private:
    EnvironmentState m_state;
    std::string m_skyboxTheme;
    std::string m_groundGridStyle;
    bool m_ambientParticles = false;
    float m_quantumFluctuations = 0.0f;
    float m_energyLevel = 0.0f;
    float m_harmonyLevel = 0.0f;
    float systemHealth;
    float timeOfDay;
    float4 skyColor;

    void updateSkyColor();

protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
};

} // namespace FinalStorm
