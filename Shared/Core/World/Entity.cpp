//
//  Entity.cpp
//  FinalStorm
//
//  Share/World/Entity.cpp
//
//  Created by Wenyan Qin on 2025-06-16.
//

#include "Entity.h"

namespace FinalStorm {

Entity::Entity(uint64_t id, EntityType type)
    : m_id(id)
    , m_type(type)
    , m_visible(true)
    , m_meshName("default")
{
}

void Entity::update(float deltaTime) {
    // Base implementation - derived classes can override
    // For example, animate position, check collisions, etc.
}

bool Entity::isInFrustum(const float4x4& viewProjectionMatrix) const {
    // Simple frustum culling - can be optimized
    float4 worldPos = simd_make_float4(m_transform.position.x, m_transform.position.y, m_transform.position.z, 1.0f);
    float4 clipPos = simd_mul(viewProjectionMatrix, worldPos);
    
    // Check if within clip space
    if (clipPos.w > 0) {
        float3 ndc = simd_make_float3(clipPos.x, clipPos.y, clipPos.z) / clipPos.w;
        return (fabs(ndc.x) <= 1.0f && fabs(ndc.y) <= 1.0f && ndc.z >= 0.0f && ndc.z <= 1.0f);
    }
    
    return false;
}

// Player Entity
PlayerEntity::PlayerEntity(uint64_t id)
    : Entity(id, EntityType::Player)
    , m_health(100.0f)
    , m_moveSpeed(5.0f)
    , m_velocity(simd_make_float3(0.0f, 0.0f, 0.0f))
{
    m_meshName = "player";
}

void PlayerEntity::update(float deltaTime) {
    Entity::update(deltaTime);
    
    // Update player-specific logic
    if (simd_length(m_velocity) > 0.0f) {
        m_transform.position += m_velocity * deltaTime;
        m_velocity *= 0.9f; // Apply friction
    }
}

void PlayerEntity::move(const float3& direction) {
    m_velocity = simd_normalize(direction) * m_moveSpeed;
}

// NPC Entity
NPCEntity::NPCEntity(uint64_t id, const std::string& npcType)
    : Entity(id, EntityType::NPC)
    , m_npcType(npcType)
    , m_aiState(AIState::Idle)
    , m_currentPatrolIndex(0)
    , m_targetId(0)
{
    m_meshName = "npc_" + npcType;
}

void NPCEntity::update(float deltaTime) {
    Entity::update(deltaTime);
    
    // Simple AI behavior
    switch (m_aiState) {
        case AIState::Idle:
            // Look for nearby players
            break;
            
        case AIState::Patrol:
            // Move along patrol path
            if (!m_patrolPath.empty()) {
                // Move towards next waypoint
                size_t nextIndex = (m_currentPatrolIndex + 1) % m_patrolPath.size();
                float3 target = m_patrolPath[nextIndex];
                float3 direction = target - m_transform.position;
                
                if (simd_length(direction) < 1.0f) {
                    // Reached waypoint, move to next
                    m_currentPatrolIndex = nextIndex;
                } else {
                    // Move towards waypoint
                    float3 moveDir = simd_normalize(direction);
                    m_transform.position += moveDir * 2.0f * deltaTime; // 2.0f is patrol speed
                }
            }
            break;
            
        case AIState::Chase:
            // Chase target
            // TODO: Implement chase logic when we have target tracking
            break;
            
        case AIState::Attack:
            // Attack target
            // TODO: Implement attack logic
            break;
    }
}

} // namespace FinalStorm