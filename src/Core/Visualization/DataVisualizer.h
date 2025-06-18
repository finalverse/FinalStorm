#pragma once

#include "Core/Environment/EnvironmentController.h"

namespace FinalStorm {

struct ServiceMetrics {
    float cpuUsage;        // 0-100 percent
    float memoryUsage;     // 0-100 percent
    int activeConnections; // arbitrary count
};

class DataVisualizer {
public:
    DataVisualizer() = default;

    // Convert service metrics into a normalized health score [0,1]
    float computeHealthScore(const ServiceMetrics& metrics) const;
};

} // namespace FinalStorm
