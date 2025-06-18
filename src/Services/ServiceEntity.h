#pragma once

#include "World/Entity.h"
#include <string>
#include <memory>

namespace FinalStorm {

class ServiceEntity : public Entity {
public:
    enum class ServiceType {
        API_GATEWAY,
        AI_ORCHESTRA,
        SONG_ENGINE,
        ECHO_ENGINE,
        WORLD_ENGINE,
        HARMONY_SERVICE,
        ASSET_SERVICE,
        COMMUNITY,
        SILENCE_SERVICE,
        PROCEDURAL_GEN,
        BEHAVIOR_AI
    };

    struct ServiceInfo {
        std::string name;
        std::string url;
        ServiceType type;
        int port;
        bool isHealthy = true;
    };

    struct ServiceMetrics {
        float cpuUsage = 0.0f;
        float memoryUsage = 0.0f;
        int activeConnections = 0;
        float responseTime = 0.0f;
        float requestsPerSecond = 0.0f;
    };

    ServiceEntity(uint64_t id, const ServiceInfo& info);

    void updateMetrics(const ServiceMetrics& metrics);
    void update(float deltaTime) override;

    const ServiceInfo& getInfo() const { return m_info; }
    const ServiceMetrics& getMetrics() const { return m_metrics; }
    float getActivityLevel() const { return m_activityLevel; }

private:
    ServiceInfo m_info;
    ServiceMetrics m_metrics;
    float m_activityLevel;
};

using ServiceEntityPtr = std::shared_ptr<ServiceEntity>;

} // namespace FinalStorm
