#include "Scene/Scene.h"
#include "Rendering/Metal/MetalRenderer.h"

namespace FinalStorm {

Scene::Scene(const std::string& name) 
    : m_name(name),
      m_rootNode(std::make_shared<SceneNode>("Root")) {
}

Scene::~Scene() = default;

void Scene::initialize() {
    // Override in derived classes
}

void Scene::onEnter() {
    // Override in derived classes
}

void Scene::onExit() {
    // Override in derived classes
}

void Scene::update(float deltaTime) {
    if (m_rootNode) {
        m_rootNode->update(deltaTime);
    }
}

void Scene::render(MetalRenderer* renderer) {
    // Update light and camera lists
    collectLights(m_rootNode);
    collectCameras(m_rootNode);
    
    // Render the scene
    if (m_rootNode) {
        m_rootNode->render(renderer);
    }
}

void Scene::addNode(std::shared_ptr<SceneNode> node) {
    if (m_rootNode && node) {
        m_rootNode->addChild(node);
    }
}

void Scene::removeNode(std::shared_ptr<SceneNode> node) {
    if (m_rootNode && node) {
        m_rootNode->removeChild(node);
    }
}

void Scene::clear() {
    if (m_rootNode) {
        m_rootNode->clearChildren();
    }
    m_lights.clear();
    m_cameras.clear();
}

std::shared_ptr<SceneNode> Scene::findNode(const std::string& name) {
    if (!m_rootNode) return nullptr;
    return m_rootNode->findChild(name);
}

std::vector<std::shared_ptr<SceneNode>> Scene::findNodesByTag(const std::string& tag) {
    std::vector<std::shared_ptr<SceneNode>> result;
    if (m_rootNode) {
        m_rootNode->findChildrenByTag(tag, result);
    }
    return result;
}

CameraNode* Scene::getActiveCamera() {
    if (!m_cameras.empty()) {
        return m_cameras[0].get();
    }
    return nullptr;
}

void Scene::collectLights(std::shared_ptr<SceneNode> node) {
    if (!node) return;
    
    if (auto light = std::dynamic_pointer_cast<LightNode>(node)) {
        m_lights.push_back(light);
    }
    
    for (const auto& child : node->getChildren()) {
        collectLights(child);
    }
}

void Scene::collectCameras(std::shared_ptr<SceneNode> node) {
    if (!node) return;
    
    if (auto camera = std::dynamic_pointer_cast<CameraNode>(node)) {
        m_cameras.push_back(camera);
    }
    
    for (const auto& child : node->getChildren()) {
        collectCameras(child);
    }
}

} // namespace FinalStorm
