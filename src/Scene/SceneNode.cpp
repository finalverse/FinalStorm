//
// src/Scene/SceneNode.cpp
// Scene graph node implementation using platform-agnostic MathTypes
// Handles 3D transforms, hierarchy management, and rendering coordination
//

#include "SceneNode.h"
#include "../Rendering/Metal/MetalRenderer.h"
#include <algorithm>

#ifdef __APPLE__
    // For SIMD matrix decomposition on Apple platforms
    #include <simd/simd.h>
#else
    // For GLM matrix decomposition on other platforms
    #include <glm/gtx/matrix_decompose.hpp>
    #include <glm/gtx/quaternion.hpp>
#endif

namespace FinalStorm {

SceneNode::SceneNode(const std::string& name)
    : m_name(name)
    , m_parent(nullptr)
    , m_localTransform(make_mat4())
    , m_visible(true)
    , m_transformDirty(true)
    , m_position(vec3_zero())
    , m_rotation(quat(1.0f, 0.0f, 0.0f, 0.0f)) // Identity quaternion
    , m_scale(vec3_one())
{
}

SceneNode::~SceneNode() = default;

void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
    if (!child || child.get() == this) {
        return;
    }
    
    // Remove from previous parent if any
    if (child->m_parent) {
        child->m_parent->removeChild(child);
    }
    
    child->m_parent = this;
    m_children.push_back(child);
}

void SceneNode::removeChild(std::shared_ptr<SceneNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

void SceneNode::removeChild(const std::string& name) {
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [&name](const std::shared_ptr<SceneNode>& node) {
            return node->getName() == name;
        });
    
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

std::shared_ptr<SceneNode> SceneNode::findChild(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->getName() == name) {
            return child;
        }
        
        // Recursively search in grandchildren
        auto found = child->findChild(name);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

mat4 SceneNode::getWorldTransform() const {
    if (m_parent) {
        return m_parent->getWorldTransform() * m_localTransform;
    }
    return m_localTransform;
}

void SceneNode::setPosition(const vec3& position) {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    m_position = position;
    rebuildTransformFromComponents();
}

void SceneNode::setRotation(const quat& rotation) {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    m_rotation = rotation;
    rebuildTransformFromComponents();
}

void SceneNode::setScale(const vec3& scale) {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    m_scale = scale;
    rebuildTransformFromComponents();
}

vec3 SceneNode::getPosition() const {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    return m_position;
}

quat SceneNode::getRotation() const {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    return m_rotation;
}

vec3 SceneNode::getScale() const {
    if (m_transformDirty) {
        updateTransformComponents();
    }
    return m_scale;
}

void SceneNode::translate(const vec3& delta) {
    vec3 currentPos = getPosition();
    setPosition(currentPos + delta);
}

void SceneNode::rotate(const quat& rotation) {
    quat currentRot = getRotation();
    setRotation(currentRot * rotation);
}

void SceneNode::scale(const vec3& scale) {
    vec3 currentScale = getScale();
    setScale(make_vec3(currentScale.x * scale.x, currentScale.y * scale.y, currentScale.z * scale.z));
}

void SceneNode::update(float deltaTime) {
    // Update this node (can be overridden by derived classes)
    
    // Update all children
    for (auto& child : m_children) {
        if (child) {
            child->update(deltaTime);
        }
    }
}

void SceneNode::render(MetalRenderer& renderer) {
    if (!m_visible) {
        return;
    }
    
    // Render this node's content if it has renderable data
    if (m_renderable && m_renderable->isVisible()) {
        m_renderable->render(renderer);
    }
    
    // Render all children
    for (auto& child : m_children) {
        if (child && child->isVisible()) {
            child->render(renderer);
        }
    }
}

void SceneNode::updateTransformComponents() const {
#ifdef __APPLE__
    // Use SIMD matrix decomposition on Apple platforms
    // Note: SIMD doesn't have built-in decomposition, so we'll do it manually
    // This is a simplified decomposition - for production, consider using a more robust method
    
    // Extract translation
    m_position = make_vec3(m_localTransform.columns[3].x, m_localTransform.columns[3].y, m_localTransform.columns[3].z);
    
    // Extract scale
    vec3 col0 = make_vec3(m_localTransform.columns[0].x, m_localTransform.columns[0].y, m_localTransform.columns[0].z);
    vec3 col1 = make_vec3(m_localTransform.columns[1].x, m_localTransform.columns[1].y, m_localTransform.columns[1].z);
    vec3 col2 = make_vec3(m_localTransform.columns[2].x, m_localTransform.columns[2].y, m_localTransform.columns[2].z);
    
    m_scale = make_vec3(length(col0), length(col1), length(col2));
    
    // Extract rotation (simplified - assumes no skew)
    if (m_scale.x != 0.0f && m_scale.y != 0.0f && m_scale.z != 0.0f) {
        mat3 rotMatrix;
        rotMatrix.columns[0] = simd_make_float3(col0.x / m_scale.x, col0.y / m_scale.x, col0.z / m_scale.x);
        rotMatrix.columns[1] = simd_make_float3(col1.x / m_scale.y, col1.y / m_scale.y, col1.z / m_scale.y);
        rotMatrix.columns[2] = simd_make_float3(col2.x / m_scale.z, col2.y / m_scale.z, col2.z / m_scale.z);
        
        // Convert rotation matrix to quaternion (simplified)
        float trace = rotMatrix.columns[0].x + rotMatrix.columns[1].y + rotMatrix.columns[2].z;
        if (trace > 0.0f) {
            float s = sqrt(trace + 1.0f) * 2.0f;
            m_rotation = simd_quaternion(
                (rotMatrix.columns[2].y - rotMatrix.columns[1].z) / s,
                (rotMatrix.columns[0].z - rotMatrix.columns[2].x) / s,
                (rotMatrix.columns[1].x - rotMatrix.columns[0].y) / s,
                0.25f * s
            );
        } else {
            m_rotation = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f); // Identity fallback
        }
    } else {
        m_rotation = simd_quaternion(0.0f, 0.0f, 0.0f, 1.0f); // Identity fallback
    }
#else
    // Use GLM matrix decomposition on other platforms
    vec3 skew;
    vec4 perspective;
    glm::decompose(m_localTransform, m_scale, m_rotation, m_position, skew, perspective);
#endif
    
    m_transformDirty = false;
}

void SceneNode::rebuildTransformFromComponents() {
#ifdef __APPLE__
    // Build transform matrix using SIMD operations
    // T * R * S order
    mat4 translation = matrix_identity_float4x4;
    translation.columns[3] = simd_make_float4(m_position.x, m_position.y, m_position.z, 1.0f);
    
    mat4 rotation = simd_matrix4x4(m_rotation);
    
    mat4 scaling = matrix_identity_float4x4;
    scaling.columns[0].x = m_scale.x;
    scaling.columns[1].y = m_scale.y;
    scaling.columns[2].z = m_scale.z;
    
    m_localTransform = translation * rotation * scaling;
#else
    // Build transform matrix using GLM operations
    mat4 translation = glm::translate(mat4(1.0f), m_position);
    mat4 rotation = glm::mat4_cast(m_rotation);
    mat4 scaling = glm::scale(mat4(1.0f), m_scale);
    
    m_localTransform = translation * rotation * scaling;
#endif
    
    m_transformDirty = false;
}

} // namespace FinalStorm