#include "Scene/SceneManager.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

SceneManager::SceneManager(std::shared_ptr<WorldManager> world)
    : m_world(std::move(world)),
      m_circleCenter(make_float2(0.0f, 0.0f)),
      m_circleRadius(8.0f)
{
}

void SceneManager::initializeServices()
{
    std::vector<ServiceEntity::ServiceInfo> defaults = {
        {"API Gateway", "http://localhost:8080/health", ServiceEntity::ServiceType::API_GATEWAY, 8080},
        {"AI Orchestra", "http://localhost:3004/health", ServiceEntity::ServiceType::AI_ORCHESTRA, 3004}
    };
    for (const auto& info : defaults)
        addService(info);
}

ServiceEntityPtr SceneManager::addService(const ServiceEntity::ServiceInfo& info)
{
    uint64_t id = static_cast<uint64_t>(m_services.size() + 100);
    auto service = std::make_shared<ServiceEntity>(id, info);

    int index = static_cast<int>(m_services.size());
    float angle = (index / 10.0f) * 2.0f * static_cast<float>(M_PI);
    service->getTransform().position = make_float3(
        cosf(angle) * m_circleRadius,
        0.0f,
        sinf(angle) * m_circleRadius);

    if (m_world)
        m_world->addEntity(service);

    m_services[info.name] = service;
    return service;
}

void SceneManager::removeService(const std::string& name)
{
    auto it = m_services.find(name);
    if (it != m_services.end()) {
        if (m_world)
            m_world->removeEntity(it->second->getId());
        m_services.erase(it);
    }
}

ServiceEntityPtr SceneManager::getService(const std::string& name) const
{
    auto it = m_services.find(name);
    return it != m_services.end() ? it->second : nullptr;
}

void SceneManager::updateServiceMetrics(const std::string& name,
                                        const ServiceEntity::ServiceMetrics& metrics)
{
    if (auto service = getService(name))
        service->updateMetrics(metrics);
}

} // namespace FinalStorm
