//
//  Entity.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include "../Math/Math.h"
#include "../Math/Transform.h"  // Add this include
#include <string>
#include <memory>
#include <vector>

namespace FinalStorm {

enum class EntityType {
    Player,
    NPC,
    Object,
    Echo,
    Terrain
};

enum class AIState {
    Idle,
    Patrol,
    Chase,
    Attack
};

class Entity {
public:
    Entity(uint64_t id, EntityType type);
    virtual ~Entity() = default;
    
    uint64_t getId() const { return m_id; }
    EntityType getType() const { return m_type; }
    
    const Transform& getTransform() const { return m_transform; }
    Transform& getTransform() { return m_transform; }
    
    const std::string& getMeshName() const { return m_meshName; }
    void setMeshName(const std::string& name) { m_meshName = name; }
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    
    virtual void update(float deltaTime);
    bool isInFrustum(const float4x4& viewProjectionMatrix) const;
    
protected:
    uint64_t m_id;
    EntityType m_type;
    Transform m_transform;
    std::string m_meshName;
    bool m_visible;
};

using EntityPtr = std::shared_ptr<Entity>;

// Player Entity
class PlayerEntity : public Entity {
public:
    PlayerEntity(uint64_t id);
    
    void update(float deltaTime) override;
    void move(const float3& direction);
    
    float getHealth() const { return m_health; }
    void setHealth(float health) { m_health = health; }
    
private:
    float m_health;
    float m_moveSpeed;
    float3 m_velocity;
};

// NPC Entity
class NPCEntity : public Entity {
public:
    NPCEntity(uint64_t id, const std::string& npcType);
    
    void update(float deltaTime) override;
    
    const std::string& getNPCType() const { return m_npcType; }
    AIState getAIState() const { return m_aiState; }
    void setAIState(AIState state) { m_aiState = state; }
    
    void addPatrolPoint(const float3& point) { m_patrolPath.push_back(point); }
    
private:
    std::string m_npcType;
    AIState m_aiState;
    std::vector<float3> m_patrolPath;
    size_t m_currentPatrolIndex;
    uint64_t m_targetId;
};

} // namespace FinalStorm