#include "Scene/SceneNode.h"
#include "Renderer/Renderer.h"
#include <algorithm>

namespace FinalStorm {

namespace {
static float4 quatMultiply(const float4& a, const float4& b)
{
    return make_float4(
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    );
}
}

SceneNode::SceneNode()
    : m_position(make_float3(0.0f)),
      m_rotation(make_float4(0.0f,0.0f,0.0f,1.0f)),
      m_scale(make_float3(1.0f)),
      m_dirty(true),
      m_visible(true)
{
    m_worldMatrix = matrix_identity();
}

void SceneNode::setPosition(const float3& pos)
{
    m_position = pos;
    markDirty();
}

void SceneNode::setRotation(const float4& rot)
{
    m_rotation = rot;
    markDirty();
}

void SceneNode::setScale(const float3& s)
{
    m_scale = s;
    markDirty();
}

void SceneNode::translate(const float3& delta)
{
    m_position += delta;
    markDirty();
}

void SceneNode::rotate(const float4& quat)
{
    m_rotation = quatMultiply(quat, m_rotation);
    markDirty();
}

void SceneNode::addChild(NodePtr child)
{
    if (!child || child.get() == this) return;
    child->m_parent = shared_from_this();
    child->markDirty();
    m_children.push_back(child);
}

void SceneNode::removeChild(NodePtr child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent.reset();
        m_children.erase(it);
    }
}

void SceneNode::clearChildren()
{
    for (auto& c : m_children) {
        if (c) c->m_parent.reset();
    }
    m_children.clear();
}

float4x4 SceneNode::getWorldMatrix() const
{
    if (m_dirty)
        updateWorldTransform();
    return m_worldMatrix;
}

float3 SceneNode::getWorldPosition() const
{
    float4x4 m = getWorldMatrix();
    return make_float3(m.columns[3].x, m.columns[3].y, m.columns[3].z);
}

void SceneNode::updateWorldTransform() const
{
    float4x4 local = simd_mul(simd_mul(matrix_translation(m_position),
                                       matrix_rotation(m_rotation)),
                               matrix_scale(m_scale));
    if (auto p = m_parent.lock()) {
        m_worldMatrix = simd_mul(p->getWorldMatrix(), local);
    } else {
        m_worldMatrix = local;
    }
    m_dirty = false;
}

void SceneNode::markDirty()
{
    m_dirty = true;
    for (auto& child : m_children) {
        if (child)
            child->markDirty();
    }
}

void SceneNode::update(float deltaTime)
{
    (void)deltaTime;
    if (m_entity) {
        const Transform& t = m_entity->getTransform();
        m_position = t.position;
        m_rotation = t.rotation;
        m_scale = t.scale;
        markDirty();
    }

    onUpdate(deltaTime);

    for (auto& child : m_children) {
        if (child)
            child->update(deltaTime);
    }
}

void SceneNode::render(Renderer* renderer)
{
    if (!m_visible) return;

    updateWorldTransform();

    if (renderer && m_entity && m_entity->isVisible()) {
        renderer->drawMesh(m_entity->getMeshName(), m_worldMatrix);
    }

    onRender(renderer);

    for (auto& child : m_children) {
        if (child)
            child->render(renderer);
    }
}

} // namespace FinalStorm
