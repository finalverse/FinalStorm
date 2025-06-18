#include "Visualization/DataVisualizer.h"

namespace FinalStorm {

float DataVisualizer::computeHealthScore(const ServiceMetrics& metrics) const
{
    // Simple heuristic: average CPU and memory usage gives load level
    float load = (metrics.cpuUsage + metrics.memoryUsage) * 0.5f / 100.0f;
    load = fmaxf(0.0f, fminf(1.0f, load));
    return 1.0f - load; // higher load -> lower health
}

} // namespace FinalStorm
