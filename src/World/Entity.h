//
//  Entity.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include "Core/Math/MathTypes.h"
#include "Core/Math/Transform.h"
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
    Entity(EntityType type);
    virtual ~Entity();
    
    uint32_t getId() const { return id; }
    EntityType getType() const { return type; }
    
    const Transform& getTransform() const { return transform; }
    Transform& getTransform() { return transform; }
    
    const std::string& getMeshName() const { return meshName; }
    void setMeshName(const std::string& name) { meshName = name; }
    
    bool isActive() const { return active; }
    void setActive(bool isActive) { active = isActive; }
    
    virtual void update(float deltaTime);
    bool isInFrustum(const mat4& viewProjectionMatrix) const;
    
protected:
    uint32_t id;
    EntityType type;
    Transform transform;
    std::string meshName;
    bool active;
    
private:
    static uint32_t nextId;
};

using EntityPtr = std::shared_ptr<Entity>;

// Player Entity
class PlayerEntity : public Entity {
public:
    PlayerEntity();
    
    void update(float deltaTime) override;
    void move(const vec3& direction);
    
    float getHealth() const { return health; }
    void setHealth(float health) { this->health = health; }
    
private:
    float health;
    float moveSpeed;
    vec3 velocity;
};

// NPC Entity
class NPCEntity : public Entity {
public:
    NPCEntity(const std::string& npcType);
    
    void update(float deltaTime) override;
    
    const std::string& getNPCType() const { return npcType; }
    AIState getAIState() const { return aiState; }
    void setAIState(AIState state) { aiState = state; }
    
    void addPatrolPoint(const vec3& point) { patrolPath.push_back(point); }
    
private:
    std::string npcType;
    AIState aiState;
    std::vector<vec3> patrolPath;
    size_t currentPatrolIndex;
    uint32_t targetId;
};

} // namespace FinalStorm