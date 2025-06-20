// src/Scene/Scene.cpp
// Main scene container implementation
// Manages the scene graph root and provides scene-wide operations

#include "Scene/Scene.h"

namespace FinalStorm {

Scene::Scene()
    : root(std::make_shared<SceneNode>("Root"))
    , ambientLight(0.2f, 0.2f, 0.2f) {
}

Scene::~Scene() {
    clear();
}

void Scene::update(float deltaTime) {
    if (root) {
        root->update(deltaTime);
    }
}

void Scene::render(RenderContext& context) {
    if (root) {
        root->render(context);
    }
}

void Scene::clear() {
    if (root) {
        root->removeAllChildren();
    }
    lights.clear();
}

void Scene::addLight(std::shared_ptr<Light> light) {
    lights.push_back(light);
}

void Scene::removeLight(std::shared_ptr<Light> light) {
    auto it = std::find(lights.begin(), lights.end(), light);
    if (it != lights.end()) {
        lights.erase(it);
    }
}

} // namespace FinalStorm