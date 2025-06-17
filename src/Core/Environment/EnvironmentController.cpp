#include "EnvironmentController.h"

namespace FinalStorm {

EnvironmentController::EnvironmentController()
{
    m_state.skyboxTint = float3{0.0f, 0.1f, 0.2f};
    m_state.ambientParticleRate = 10.0f;
    m_state.groundPulse = 0.0f;
}

void EnvironmentController::updateFromHealth(float healthScore)
{
    healthScore = fmaxf(0.0f, fminf(1.0f, healthScore));

    // Skybox goes from cool blue to warning red as health decreases
    float3 healthyColor = float3{0.0f, 0.1f, 0.2f};
    float3 warningColor = float3{0.2f, 0.0f, 0.0f};
    m_state.skyboxTint = healthyColor * healthScore + warningColor * (1.0f - healthScore);

    // Particle rate increases as health declines
    m_state.ambientParticleRate = 10.0f + (1.0f - healthScore) * 40.0f;

    // Ground pulse strength rises with instability
    m_state.groundPulse = 0.5f + (1.0f - healthScore) * 0.5f;
}

} // namespace FinalStorm
