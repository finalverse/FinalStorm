#pragma once

#include "World/Entity.h"
#include "Services/ServiceMetrics.h"
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
        BEHAVIOR_AI,
        WEBSOCKET_GATEWAY,
        DATABASE
    };

    struct ServiceInfo {
        std::string name;
        std::string url;
        ServiceType type;
        int port;
        bool isHealthy = true;
    };

    ServiceEntity(const ServiceInfo& info);
    virtual ~ServiceEntity() = default;

    void updateMetrics(const ServiceMetrics& metrics);
    void update(float deltaTime) override;

    const ServiceInfo& getInfo() const { return serviceInfo; }
    const ServiceMetrics& getMetrics() const { return metrics; }
    float getActivityLevel() const { return activityLevel; }
    float getHealthLevel() const { return healthLevel; }

    ServiceType getServiceType() const { return serviceInfo.type; }

protected:
    virtual void updateVisuals();
    virtual void updateAnimation(float deltaTime);

private:
    ServiceInfo serviceInfo;
    ServiceMetrics metrics;
    float activityLevel;
    float healthLevel;
    float animationTime;
};

using ServiceEntityPtr = std::shared_ptr<ServiceEntity>;

} // namespace FinalStorm