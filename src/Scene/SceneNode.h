// src/Scene/SceneNode.h
// Scene graph node base class
// Hierarchical scene organization

#pragma once
#include "Core/Math/MathTypes.h"
#include "Core/Math/Transform.h"
#include <string>
#include <vector>
#include <memory>

namespace FinalStorm {

class RenderContext;
class Camera;

class SceneNode {
public:
    SceneNode(const std::string& name = "SceneNode");
    virtual ~SceneNode() = default;
    
    // Hierarchy
    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(std::shared_ptr<SceneNode> child);
    void removeAllChildren();
    
    std::shared_ptr<SceneNode> getParent() const { return m_parent.lock(); }
    const std::vector<std::shared_ptr<SceneNode>>& getChildren() const { return m_children; }
    
    // Transform
    const Transform& getLocalTransform() const { return m_localTransform; }
    Transform& getLocalTransform() { return m_localTransform; }
    
    mat4 getWorldMatrix() const;
    vec3 getWorldPosition() const;
    quat getWorldRotation() const;
    
    // Convenience transform methods
    void setPosition(const vec3& position) { m_localTransform.setPosition(position); }
    void setRotation(const quat& rotation) { m_localTransform.setRotation(rotation); }
    void setScale(const vec3& scale) { m_localTransform.setScale(scale); }
    
    vec3 getPosition() const { return m_localTransform.position; }
    quat getRotation() const { return m_localTransform.rotation; }
    vec3 getScale() const { return m_localTransform.scale; }
    
    void translate(const vec3& delta) { m_localTransform.translate(delta); }
    void rotate(const quat& rotation) { m_localTransform.rotate(rotation); }
    
    // Properties
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    
    // Update and render
    virtual void update(float deltaTime);
    virtual void render(RenderContext& context);
    
    // Frustum culling
    bool isInFrustum(const Camera& camera) const;
    
protected:
    // Override these in derived classes
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender(RenderContext& context) {}
    
private:
    std::string m_name;
    Transform m_localTransform;
    bool m_visible;
    
    std::weak_ptr<SceneNode> m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;
    
    mutable mat4 m_cachedWorldMatrix;
    mutable bool m_worldMatrixDirty;
    
    void markWorldMatrixDirty();
    void updateWorldMatrix() const;
};

using SceneNodePtr = std::shared_ptr<SceneNode>;

} // namespace FinalStorm