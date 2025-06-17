// Scene.h - Scene graph management

#pragma once

#include "Scene/SceneNode.h"
#include <memory>
#include <vector>

namespace FinalStorm {

class Scene {
public:
    Scene();
    ~Scene();
    
    // Node management
    void addNode(std::shared_ptr<SceneNode> node);
    void removeNode(std::shared_ptr<SceneNode> node);
    void clear();
    
    // Update and render
    void update(float deltaTime);
    void render(class MetalRenderer* renderer);
    
    // Scene queries
    std::shared_ptr<SceneNode> findNode(const std::string& name);
    std::vector<std::shared_ptr<SceneNode>> findNodesByTag(const std::string& tag);
    
    // Root node access
    std::shared_ptr<SceneNode> getRoot() { return m_rootNode; }
    
private:
    std::shared_ptr<SceneNode> m_rootNode;
    
    // Cached lists for performance
    std::vector<std::shared_ptr<class LightNode>> m_lights;
    std::vector<std::shared_ptr<class CameraNode>> m_cameras;
    
    void collectLights(std::shared_ptr<SceneNode> node);
    void collectCameras(std::shared_ptr<SceneNode> node);
};

} // namespace FinalStorm
