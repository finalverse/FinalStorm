//
//  Entity.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include "../Math/Math.h"
#include <string>
#include <memory>

namespace FinalStorm {

enum class EntityType {
    Player,
    NPC,
    Object,
    Echo,
    Terrain
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
    
    virtual void update(float deltaTime) {}
    
protected:
    uint64_t m_id;
    EntityType m_type;
    Transform m_transform;
    std::string m_meshName;
    bool m_visible;
};

using EntityPtr = std::shared_ptr<Entity>;

} // namespace FinalStorm
