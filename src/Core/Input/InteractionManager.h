#pragma once

#include "Core/Math/Math.h"
#include <memory>

namespace FinalStorm {

class Camera;
class SceneNode;

class InteractionManager {
public:
    InteractionManager();

    void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
    void setSceneRoot(std::shared_ptr<SceneNode> root) { m_sceneRoot = root; }

    void handleTouchBegan(const float2& point, const float2& viewport);
    void handleTouchMoved(const float2& point, const float2& viewport);
    void handleTouchEnded(const float2& point, const float2& viewport);
    void handlePinch(float scale);

    struct Ray { float3 origin; float3 direction; };
    Ray screenPointToRay(const float2& point, const float2& viewport) const;

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<SceneNode> m_sceneRoot;
    float2 m_lastPoint;
};

} // namespace FinalStorm
