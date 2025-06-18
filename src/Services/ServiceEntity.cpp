#include "Services/ServiceEntity.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

ServiceEntity::ServiceEntity(uint64_t id, const ServiceInfo& info)
    : Entity(id, EntityType::Object),
      m_info(info),
      m_activityLevel(0.0f)
{
    m_meshName = "service";
}

void ServiceEntity::updateMetrics(const ServiceMetrics& metrics)
{
    m_metrics = metrics;
    // Basic heuristic for activity level
    m_activityLevel = (metrics.cpuUsage + metrics.memoryUsage) * 0.5f / 100.0f;
    m_activityLevel = fmaxf(0.0f, fminf(1.0f, m_activityLevel));
}

void ServiceEntity::update(float deltaTime)
{
    Entity::update(deltaTime);
    (void)deltaTime; // placeholder for future animations based on activity
}

} // namespace FinalStorm
