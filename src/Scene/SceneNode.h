#pragma once

#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"
#include "World/Entity.h"
#include <memory>
#include <vector>

namespace FinalStorm {

class Renderer; // forward declaration

// Basic scene graph node with transform hierarchy
class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    using NodePtr = std::shared_ptr<SceneNode>;
    using NodeList = std::vector<NodePtr>;

    SceneNode();
    virtual ~SceneNode() = default;

    // Transform operations
    void setPosition(const float3& pos);
    void setRotation(const float4& rot);
    void setScale(const float3& s);
    void translate(const float3& delta);
    void rotate(const float4& quat);

    // Hierarchy management
    void addChild(NodePtr child);
    void removeChild(NodePtr child);
    void clearChildren();
    const NodeList& getChildren() const { return m_children; }

    SceneNode* getParent() const { return m_parent.lock().get(); }

    // World transform helpers
    float4x4 getWorldMatrix() const;
    float3 getWorldPosition() const;

    // Entity attachment
    void setEntity(EntityPtr entity) { m_entity = entity; }
    EntityPtr getEntity() const { return m_entity; }

    bool isVisible() const { return m_visible; }
    void setVisible(bool v) { m_visible = v; }

    // Update and render
    virtual void update(float deltaTime);
    virtual void render(Renderer* renderer);

protected:
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender(Renderer* renderer) {}

    void markDirty();
    void updateWorldTransform() const;

    std::weak_ptr<SceneNode> m_parent;
    NodeList m_children;

    float3 m_position;
    float4 m_rotation;
    float3 m_scale;

    mutable float4x4 m_worldMatrix;
    mutable bool m_dirty;
    bool m_visible;

    EntityPtr m_entity;
};

} // namespace FinalStorm
