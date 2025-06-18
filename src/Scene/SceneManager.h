#pragma once

#include "World/WorldManager.h"
#include "Services/ServiceEntity.h"
#include <map>
#include <memory>

namespace FinalStorm {

class SceneManager {
public:
    explicit SceneManager(std::shared_ptr<WorldManager> world);

    void initializeServices();

    ServiceEntityPtr addService(const ServiceEntity::ServiceInfo& info);
    void removeService(const std::string& name);
    ServiceEntityPtr getService(const std::string& name) const;

    void updateServiceMetrics(const std::string& name,
                              const ServiceEntity::ServiceMetrics& metrics);

private:
    std::shared_ptr<WorldManager> m_world;
    std::map<std::string, ServiceEntityPtr> m_services;

    float2 m_circleCenter;
    float  m_circleRadius;
};

} // namespace FinalStorm
