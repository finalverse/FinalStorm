//
// SceneManager.h - Scene Management System
// FinalStorm 3D Client for Finalverse
//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

// Forward declarations
namespace FinalStorm {
    class Scene;
    class MetalRenderer;
}

namespace FinalStorm {

class SceneManager {
public:
    SceneManager();
    ~SceneManager();
    
    // Scene management
    void addScene(const std::string& name, std::shared_ptr<Scene> scene);
    void removeScene(const std::string& name);
    std::shared_ptr<Scene> getScene(const std::string& name) const;
    
    // Current scene
    void setCurrentScene(std::shared_ptr<Scene> scene);
    void setCurrentScene(const std::string& name);
    std::shared_ptr<Scene> getCurrentScene() const { return m_currentScene; }
    
    // Updates
    void update(float deltaTime);
    void render(MetalRenderer& renderer);
    
    // Utilities
    void clearAllScenes();
    size_t getSceneCount() const { return m_scenes.size(); }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
    std::shared_ptr<Scene> m_currentScene;
};

} // namespace FinalStorm