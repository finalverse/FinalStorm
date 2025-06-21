// src/Scene/SceneNode.cpp
// Scene graph node implementation
// Hierarchical scene organization

#include "Scene/SceneNode.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Camera.h"
#include <algorithm>

namespace FinalStorm {

SceneNode::SceneNode(const std::string& name)
    : m_name(name)
    , m_visible(true)
    , m_worldMatrixDirty(true) {
}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (!child) return;
    
    // Remove from previous parent
    if (auto oldParent = child->getParent()) {
        oldParent->removeChild(child);
    }
    
    // Add to this node
    child->m_parent = std::weak_ptr<SceneNode>(shared_from_this());
    m_children.push_back(child);
    child->markWorldMatrixDirty();
}

void SceneNode::removeChild(std::shared_ptr<SceneNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent.reset();
        m_children.erase(it);
    }
}

void SceneNode::removeAllChildren() {
    for (auto& child : m_children) {
        child->m_parent.reset();
    }
    m_children.clear();
}

mat4 SceneNode::getWorldMatrix() const {
    if (m_worldMatrixDirty) {
        updateWorldMatrix();
    }
    return m_cachedWorldMatrix;
}

vec3 SceneNode::getWorldPosition() const {
    mat4 worldMat = getWorldMatrix();
    return make_vec3(worldMat.columns[3].x, worldMat.columns[3].y, worldMat.columns[3].z);
}

quat SceneNode::getWorldRotation() const {
    // Extract rotation from world matrix
    // For now, just return local rotation (simplified)
    if (auto parent = getParent()) {
        return simd_mul(parent->getWorldRotation(), m_localTransform.rotation);
    }
    return m_localTransform.rotation;
}

void SceneNode::update(float deltaTime) {
    if (!m_visible) return;
    
    // Update this node
    onUpdate(deltaTime);
    
    // Update children
    for (auto& child : m_children) {
        child->update(deltaTime);
    }
}

void SceneNode::render(RenderContext& context) {
    if (!m_visible) return;
    
    // Render this node
    onRender(context);
    
    // Render children
    for (auto& child : m_children) {
        child->render(context);
    }
}

bool SceneNode::isInFrustum(const Camera& camera) const {
    // Simple point-in-frustum test using world position
    vec3 worldPos = getWorldPosition();
    return camera.isInFrustum(worldPos);
}

void SceneNode::markWorldMatrixDirty() {
    m_worldMatrixDirty = true;
    
    // Mark all children dirty too
    for (auto& child : m_children) {
        child->markWorldMatrixDirty();
    }
}

void SceneNode::updateWorldMatrix() const {
    if (auto parent = getParent()) {
        m_cachedWorldMatrix = simd_mul(parent->getWorldMatrix(), m_localTransform.getMatrix());
    } else {
        m_cachedWorldMatrix = m_localTransform.getMatrix();
    }
    m_worldMatrixDirty = false;
}

} // namespace FinalStorm