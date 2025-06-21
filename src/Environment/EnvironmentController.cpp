// src/Environment/EnvironmentController.cpp
// Environment controller implementation
// Manages skybox, lighting, and atmospheric effects

#include "Environment/EnvironmentController.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

EnvironmentController::EnvironmentController()
    : SceneNode("Environment")
    , m_ambientParticles(false)
    , m_quantumFluctuations(0.0f)
    , m_energyLevel(0.0f)
    , m_harmonyLevel(0.0f)
    , systemHealth(1.0f)
    , timeOfDay(0.5f)
    , skyColor(0.2f, 0.4f, 0.6f, 1.0f) {
    m_state.skyboxTint = float3(0.0f);
    m_state.ambientParticleRate = 0.0f;
    m_state.groundPulse = 1.0f;
    m_state.energyLevel = 0.0f;
    m_state.harmonyLevel = 0.0f;
}

EnvironmentController::~EnvironmentController() = default;

void EnvironmentController::onUpdate(float deltaTime) {
    // Update time of day
    timeOfDay += deltaTime * 0.01f; // Slow day/night cycle
    if (timeOfDay > 1.0f) {
        timeOfDay -= 1.0f;
    }
    
    // Update sky color based on system health and time
    updateSkyColor();
}

void EnvironmentController::onRender(RenderContext& context) {
    // Render skybox
    context.pushTransform(float4x4(1.0f)); // Identity for skybox
    context.setColor(skyColor);
    context.drawSkybox();
    context.popTransform();
    
    // Render ground grid
    context.pushTransform(float4x4(1.0f));
    context.setColor(float4(0.1f, 0.2f, 0.3f, 0.5f));
    context.drawGrid(100.0f, 1.0f);
    context.popTransform();
}

void EnvironmentController::setSkyboxTheme(const std::string& theme) {
    m_skyboxTheme = theme;
    if (theme == "quantum_void") {
        m_state.skyboxTint = float3(0.1f, 0.0f, 0.2f);
    } else if (theme == "sunrise") {
        m_state.skyboxTint = float3(0.6f, 0.3f, 0.2f);
    } else {
        m_state.skyboxTint = float3(0.0f);
    }
}

void EnvironmentController::setGroundGridStyle(const std::string& style) {
    m_groundGridStyle = style;
    m_state.groundPulse = (style == "hexagonal") ? 1.5f : 1.0f;
}

void EnvironmentController::setAmbientParticles(bool enabled) {
    m_ambientParticles = enabled;
    m_state.ambientParticleRate = enabled ? 20.0f : 0.0f;
}

void EnvironmentController::setQuantumFluctuations(float amount) {
    m_quantumFluctuations = amount;
}

void EnvironmentController::setEnergyLevel(float level) {
    m_energyLevel = Math::clamp(level, 0.0f, 1.0f);
    m_state.energyLevel = m_energyLevel;
}

void EnvironmentController::setHarmonyLevel(float level) {
    m_harmonyLevel = Math::clamp(level, 0.0f, 1.0f);
    m_state.harmonyLevel = m_harmonyLevel;
}

void EnvironmentController::updateFromHealth(float healthScore) {
    float h = Math::clamp(healthScore, 0.0f, 1.0f);
    setEnergyLevel(h);
    setHarmonyLevel(h);
    setAmbientParticles(h > 0.3f);
    systemHealth = h;
    updateSkyColor();
}

void EnvironmentController::setSystemHealth(float health) {
    systemHealth = Math::clamp(health, 0.0f, 1.0f);
}

void EnvironmentController::updateSkyColor() {
    // Base sky color varies with time of day
    float3 dayColor(0.4f, 0.6f, 1.0f);
    float3 nightColor(0.05f, 0.05f, 0.2f);
    float3 baseColor = dayColor * timeOfDay + nightColor * (1.0f - timeOfDay);
    
    // Tint based on system health
    float3 healthyTint(0.0f, 0.2f, 0.3f);
    float3 unhealthyTint(0.3f, 0.0f, 0.0f);
    float3 tint = healthyTint * systemHealth + unhealthyTint * (1.0f - systemHealth);
    
    skyColor = float4(baseColor + tint + m_state.skyboxTint, 1.0f);
}

} // namespace FinalStorm