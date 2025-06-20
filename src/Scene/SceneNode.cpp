// src/Scene/SceneNode.cpp
// Base scene graph node implementation
// Provides hierarchical transform management and scene traversal

#include "Scene/SceneNode.h"
#include <algorithm>

namespace FinalStorm {

uint32_t SceneNode::nextId = 1;

SceneNode::SceneNode(const std::string& nodeName)
    : id(nextId++)
    , name(nodeName)
    , parent(nullptr)
    , visible(true)
    , worldTransformDirty(true) {
}

SceneNode::~SceneNode() {
    // Remove from parent if attached
    if (parent) {
        parent->removeChild(this);
    }
    
    // Clear children
    children.clear();
}

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (!child || child.get() == this) {
        return;
    }
    
    // Remove from previous parent
    if (child->parent) {
        child->parent->removeChild(child.get());
    }
    
    // Add to our children
    children.push_back(child);
    child->parent = this;
    child->worldTransformDirty = true;
}

void SceneNode::removeChild(SceneNode* child) {
    auto it = std::find_if(children.begin(), children.end(),
        [child](const std::shared_ptr<SceneNode>& node) {
            return node.get() == child;
        });
    
    if (it != children.end()) {
        (*it)->parent = nullptr;
        (*it)->worldTransformDirty = true;
        children.erase(it);
    }
}

std::shared_ptr<SceneNode> SceneNode::findChild(const std::string& childName) const {
    for (const auto& child : children) {
        if (child->name == childName) {
            return child;
        }
    }
    return nullptr;
}

void SceneNode::update(float deltaTime) {
    // Update this node
    onUpdate(deltaTime);
    
    // Update children
    for (auto& child : children) {
        if (child->visible) {
            child->update(deltaTime);
        }
    }
}

void SceneNode::render(RenderContext& context) {
    if (!visible) {
        return;
    }
    
    // Render this node
    onRender(context);
    
    // Render children
    for (auto& child : children) {
        child->render(context);
    }
}

void SceneNode::setLocalTransform(const Transform& t) {
    localTransform = t;
    worldTransformDirty = true;
    markChildrenDirty();
}

void SceneNode::setPosition(const float3& pos) {
    localTransform.setPosition(pos);
    worldTransformDirty = true;
    markChildrenDirty();
}

void SceneNode::setRotation(const quaternion& rot) {
    localTransform.setRotation(rot);
    worldTransformDirty = true;
    markChildrenDirty();
}

void SceneNode::setScale(const float3& scale) {
    localTransform.setScale(scale);
    worldTransformDirty = true;
    markChildrenDirty();
}

Transform SceneNode::getWorldTransform() const {
    if (worldTransformDirty) {
        updateWorldTransform();
    }
    return worldTransform;
}

float4x4 SceneNode::getWorldMatrix() const {
    return getWorldTransform().getMatrix();
}

void SceneNode::updateWorldTransform() const {
    if (parent) {
        float4x4 parentWorld = parent->getWorldMatrix();
        float4x4 localMatrix = localTransform.getMatrix();
        float4x4 worldMatrix = parentWorld * localMatrix;
        
        // Decompose world matrix back to transform
        // This is simplified - proper decomposition would extract rotation/scale properly
        worldTransform.setPosition(float3(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]));
        // TODO: Proper matrix decomposition for rotation and scale
    } else {
        worldTransform = localTransform;
    }
    worldTransformDirty = false;
}

void SceneNode::markChildrenDirty() {
    for (auto& child : children) {
        child->worldTransformDirty = true;
        child->markChildrenDirty();
    }
}

} // namespace FinalStorm