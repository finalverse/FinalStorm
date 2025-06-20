// src/Scene/SceneManager.cpp
// Scene management implementation
// Handles scene updates, rendering, and camera control

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

SceneManager::SceneManager(std::shared_ptr<Scene> scn, std::shared_ptr<Renderer> rend)
    : scene(scn)
    , renderer(rend) {
    
    // Setup default camera
    camera.setPosition(float3(0, 5, 10));
    camera.lookAt(float3(0, 5, 10), float3(0, 0, 0), float3(0, 1, 0));
    camera.setFOV(Math::radians(60.0f));
    camera.setNearFar(0.1f, 1000.0f);
}

SceneManager::~SceneManager() = default;

void SceneManager::update(float deltaTime) {
    if (scene) {
        scene->update(deltaTime);
    }
    
    // Update camera if needed
    updateCamera(deltaTime);
}

void SceneManager::render() {
    if (!scene || !renderer) {
        return;
    }
    
    // Create render context
    auto context = renderer->createRenderContext();
    if (!context) {
        return;
    }
    
    // Set view/projection matrices
    renderer->setViewProjectionMatrix(camera.getViewMatrix(), camera.getProjectionMatrix());
    
    // Set camera info in context
    context->setCamera(camera);
    
    // Render the scene
    scene->render(*context);
}

void SceneManager::onResize(uint32_t width, uint32_t height) {
    if (width > 0 && height > 0) {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        camera.setAspectRatio(aspectRatio);
    }
}

void SceneManager::updateCamera(float deltaTime) {
    // Camera animation or user control can be implemented here
}

} // namespace FinalStorm