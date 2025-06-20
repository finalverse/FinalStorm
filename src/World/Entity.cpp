// src/World/Entity.cpp
// Base entity class implementation
// Represents objects in the game world with position and behavior

#include "World/Entity.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

uint32_t Entity::nextId = 1;

Entity::Entity(EntityType entityType)
    : id(nextId++)
    , type(entityType)
    , active(true)
    , transform() {
}

Entity::~Entity() = default;

void Entity::update(float deltaTime) {
    // Base implementation - derived classes override
}

bool Entity::isInFrustum(const mat4& viewProjectionMatrix) const {
    // Simple frustum culling using transform position
    vec3 position = transform.position;
    vec4 clipSpace = simd_mul(viewProjectionMatrix, make_vec4(position.x, position.y, position.z, 1.0f));
    
    // Perspective divide
    if (clipSpace.w != 0) {
        clipSpace.x /= clipSpace.w;
        clipSpace.y /= clipSpace.w;
        clipSpace.z /= clipSpace.w;
    }
    
    // Check if point is in NDC space
    return clipSpace.x >= -1.0f && clipSpace.x <= 1.0f &&
           clipSpace.y >= -1.0f && clipSpace.y <= 1.0f &&
           clipSpace.z >= 0.0f && clipSpace.z <= 1.0f;
}

// PlayerEntity implementation
PlayerEntity::PlayerEntity()
    : Entity(EntityType::Player)
    , health(100.0f)
    , moveSpeed(5.0f)
    , velocity(make_vec3(0.0f, 0.0f, 0.0f)) {
    setMeshName("player");
}

void PlayerEntity::update(float deltaTime) {
    Entity::update(deltaTime);
    
    // Apply velocity to position
    if (length(velocity) > 0.0f) {
        transform.translate(velocity * deltaTime);
        // Apply friction
        velocity *= 0.9f;
    }
}

void PlayerEntity::move(const vec3& direction) {
    velocity += direction * moveSpeed;
}

// NPCEntity implementation
NPCEntity::NPCEntity(const std::string& npcType)
    : Entity(EntityType::NPC)
    , npcType(npcType)
    , aiState(AIState::Idle)
    , currentPatrolIndex(0)
    , targetId(0) {
    setMeshName("npc_" + npcType);
}

void NPCEntity::update(float deltaTime) {
    Entity::update(deltaTime);
    
    // Simple AI behavior based on state
    switch (aiState) {
        case AIState::Patrol:
            if (!patrolPath.empty()) {
                vec3 targetPos = patrolPath[currentPatrolIndex];
                vec3 currentPos = transform.position;
                vec3 direction = normalize(targetPos - currentPos);
                
                float distance = length(targetPos - currentPos);
                if (distance < 1.0f) {
                    // Reached patrol point, move to next
                    currentPatrolIndex = (currentPatrolIndex + 1) % patrolPath.size();
                } else {
                    // Move towards patrol point
                    transform.translate(direction * 2.0f * deltaTime);
                }
            }
            break;
        case AIState::Idle:
        case AIState::Chase:
        case AIState::Attack:
        default:
            // TODO: Implement other AI states
            break;
    }
}

} // namespace FinalStorm