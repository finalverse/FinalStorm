// src/World/WorldManager.cpp
// World management implementation
// Handles entity lifecycle and spatial organization

#include "World/WorldManager.h"
#include "World/Entity.h"
#include <algorithm>

namespace FinalStorm {

WorldManager::WorldManager() = default;
WorldManager::~WorldManager() = default;

void WorldManager::update(float deltaTime) {
    // Update all active entities
    for (auto& entity : entities) {
        if (entity->isActive()) {
            entity->update(deltaTime);
        }
    }
    
    // Remove inactive entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const std::shared_ptr<Entity>& entity) {
                return !entity->isActive();
            }),
        entities.end()
    );
}

void WorldManager::addEntity(std::shared_ptr<Entity> entity) {
    if (entity) {
        entities.push_back(entity);
    }
}

void WorldManager::removeEntity(uint32_t entityId) {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entityId](const std::shared_ptr<Entity>& entity) {
            return entity->getId() == entityId;
        });
    
    if (it != entities.end()) {
        (*it)->setActive(false);
    }
}

std::shared_ptr<Entity> WorldManager::getEntity(uint32_t entityId) const {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entityId](const std::shared_ptr<Entity>& entity) {
            return entity->getId() == entityId;
        });
    
    return (it != entities.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<Entity>> WorldManager::getEntitiesInRadius(
    const float3& center, float radius) const {
    
    std::vector<std::shared_ptr<Entity>> result;
    float radiusSq = radius * radius;
    
    for (const auto& entity : entities) {
        float3 pos = entity->getPosition();
        float3 delta = pos - center;
        float distSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        
        if (distSq <= radiusSq) {
            result.push_back(entity);
        }
    }
    
    return result;
}

} // namespace FinalStorm