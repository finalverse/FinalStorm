// src/Services/ServiceMetrics.cpp
// Service metrics implementation
// Utility functions for service metrics

#include "Services/ServiceMetrics.h"
#include <algorithm>

namespace FinalStorm {

ServiceMetrics ServiceMetrics::lerp(const ServiceMetrics& a, const ServiceMetrics& b, float t) {
    ServiceMetrics result;
    
    result.cpuUsage = a.cpuUsage + (b.cpuUsage - a.cpuUsage) * t;
    result.memoryUsage = a.memoryUsage + (b.memoryUsage - a.memoryUsage) * t;
    result.requestsPerSecond = static_cast<uint32_t>(a.requestsPerSecond + (b.requestsPerSecond - a.requestsPerSecond) * t);
    result.averageLatency = a.averageLatency + (b.averageLatency - a.averageLatency) * t;
    result.errorRate = a.errorRate + (b.errorRate - a.errorRate) * t;
    result.activeConnections = static_cast<uint32_t>(a.activeConnections + (b.activeConnections - a.activeConnections) * t);
    
    return result;
}

float ServiceMetrics::getHealthScore() const {
    // Calculate overall health score (0-1)
    float healthScore = 1.0f;
    
    // Penalize high CPU usage
    if (cpuUsage > 80.0f) {
        healthScore *= (100.0f - cpuUsage) / 20.0f;
    }
    
    // Penalize high memory usage
    if (memoryUsage > 90.0f) {
        healthScore *= (100.0f - memoryUsage) / 10.0f;
    }
    
    // Penalize high error rate
    healthScore *= std::max(0.0f, (100.0f - errorRate) / 100.0f);
    
    // Penalize high latency
    if (averageLatency > 1000.0f) { // > 1 second
        healthScore *= std::max(0.0f, (2000.0f - averageLatency) / 1000.0f);
    }
    
    return std::max(0.0f, std::min(1.0f, healthScore));
}

float ServiceMetrics::getActivityScore() const {
    // Calculate activity level (0-1) based on requests per second
    return std::min(1.0f, requestsPerSecond / 1000.0f);
}

bool ServiceMetrics::isHealthy() const {
    return getHealthScore() > 0.7f;
}

std::string ServiceMetrics::toString() const {
    return "CPU: " + std::to_string(cpuUsage) + "%, " +
           "Memory: " + std::to_string(memoryUsage) + "%, " +
           "RPS: " + std::to_string(requestsPerSecond) + ", " +
           "Latency: " + std::to_string(averageLatency) + "ms, " +
           "Errors: " + std::to_string(errorRate) + "%, " +
           "Connections: " + std::to_string(activeConnections);
}

} // namespace FinalStorm