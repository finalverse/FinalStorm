//
// src/Scene/SceneNode.h
// Base Scene Graph Node for FinalStorm 3D hierarchy
// Uses platform-agnostic MathTypes (SIMD on Apple, GLM elsewhere)
//

#pragma once

#include "../Core/Math/MathTypes.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

// Forward declarations
namespace FinalStorm {
    class MetalRenderer;
    class Renderable;
}

namespace FinalStorm {

class SceneNode {
public:
    SceneNode(const std::string& name = "");
    virtual ~SceneNode();
    
    // Identity
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }
    
    // Hierarchy management
    void addChild(std::shared_ptr<SceneNode> child);
    void removeChild(std::shared_ptr<SceneNode> child);
    void removeChild(const std::string& name);
    std::shared_ptr<SceneNode> findChild(const std::string& name) const;
    const std::vector<std::shared_ptr<SceneNode>>& getChildren() const { return m_children; }
    
    SceneNode* getParent() const { return m_parent; }
    
    // Transform operations using MathTypes
    void setLocalTransform(const mat4& transform) { m_localTransform = transform; markTransformDirty(); }
    const mat4& getLocalTransform() const { return m_localTransform; }
    mat4 getWorldTransform() const;
    
    void setPosition(const vec3& position);
    void setRotation(const quat& rotation);
    void setScale(const vec3& scale);
    
    vec3 getPosition() const;
    quat getRotation() const;
    vec3 getScale() const;
    
    // Convenience transform methods
    void translate(const vec3& delta);
    void rotate(const quat& rotation);
    void scale(const vec3& scale);
    
    // Rendering
    void setRenderable(std::shared_ptr<Renderable> renderable) { m_renderable = renderable; }
    std::shared_ptr<Renderable> getRenderable() const { return m_renderable; }
    
    // Visibility
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    
    // Updates
    virtual void update(float deltaTime);
    virtual void render(MetalRenderer& renderer);
    
    // Service metrics (for ServiceEntity)
    virtual void updateMetrics(const std::map<std::string, float>& metrics) {}
    
protected:
    std::string m_name;
    SceneNode* m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;
    
    mat4 m_localTransform;
    std::shared_ptr<Renderable> m_renderable;
    bool m_visible;
    
    // Cached transform components
    mutable bool m_transformDirty;
    mutable vec3 m_position;
    mutable quat m_rotation;
    mutable vec3 m_scale;
    
    void markTransformDirty() const { m_transformDirty = true; }
    void updateTransformComponents() const;
    void rebuildTransformFromComponents();
};

// Base class for renderable objects
class Renderable {
public:
    virtual ~Renderable() = default;
    virtual void render(MetalRenderer& renderer) = 0;
    virtual bool isVisible() const { return m_visible; }
    virtual void setVisible(bool visible) { m_visible = visible; }
    
protected:
    bool m_visible = true;
};

} // namespace FinalStorm