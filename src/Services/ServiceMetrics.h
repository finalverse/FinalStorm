// src/Services/ServiceMetrics.h
// Service metrics definitions
// Data structures for service health and performance

#pragma once
#include <cstdint>

namespace FinalStorm {

struct ServiceMetrics {
    float cpuUsage;           // 0-100%
    float memoryUsage;        // 0-100%
    uint32_t requestsPerSecond;
    float averageLatency;     // milliseconds
    float errorRate;          // 0-100%
    uint32_t activeConnections;
    
    ServiceMetrics()
        : cpuUsage(0)
        , memoryUsage(0)
        , requestsPerSecond(0)
        , averageLatency(0)
        , errorRate(0)
        , activeConnections(0) {}
};

} // namespace FinalStorm