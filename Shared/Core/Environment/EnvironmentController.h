#pragma once

#include "../Math/Math.h"

namespace FinalStorm {

struct EnvironmentState {
    float3 skyboxTint;
    float ambientParticleRate;
    float groundPulse;
};

class EnvironmentController {
public:
    EnvironmentController();

    // Update environment parameters from a normalized health score [0,1]
    void updateFromHealth(float healthScore);

    const EnvironmentState& getState() const { return m_state; }

private:
    EnvironmentState m_state;
};

} // namespace FinalStorm
