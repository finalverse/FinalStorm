#include "Core/Input/InteractionManager.h"
#include "Core/Math/Math.h"
#include "Scene/SceneNode.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

InteractionManager::InteractionManager()
    : m_lastPoint(simd_make_float2(0.0f, 0.0f)) {}

void InteractionManager::handleTouchBegan(const float2& point, const float2& viewport)
{
    m_lastPoint = point;
    (void)viewport;
}

void InteractionManager::handleTouchMoved(const float2& point, const float2& viewport)
{
    m_lastPoint = point;
    (void)viewport;
}

void InteractionManager::handleTouchEnded(const float2& point, const float2& viewport)
{
    m_lastPoint = point;
    (void)viewport;
}

void InteractionManager::handlePinch(float scale)
{
    (void)scale;
}

InteractionManager::Ray InteractionManager::screenPointToRay(const float2& point, const float2& viewport) const
{
    Ray ray{};
    if (!m_camera) return ray;

    float2 ndc = simd_make_float2((2.0f * point.x / viewport.x) - 1.0f,
                                  1.0f - (2.0f * point.y / viewport.y));
    float4 clip = simd_make_float4(ndc.x, ndc.y, 1.0f, 1.0f);
    float4 view = simd_mul(simd_inverse(m_camera->getProjectionMatrix()), clip);
    view = simd_make_float4(view.x, view.y, 1.0f, 0.0f);
    float4 world = simd_mul(simd_inverse(m_camera->getViewMatrix()), view);
    ray.origin = m_camera->getPosition();
    ray.direction = simd_normalize(simd_make_float3(world.x, world.y, world.z));
    return ray;
}

} // namespace FinalStorm
