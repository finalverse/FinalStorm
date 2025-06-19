//
// SceneManager.cpp - Scene Management Implementation
// FinalStorm 3D Client for Finalverse
//

#include "SceneManager.h"
#include "Scene.h"
#include "../Rendering/Metal/MetalRenderer.h"
#include <iostream>

namespace FinalStorm {

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
    clearAllScenes();
}

void SceneManager::addScene(const std::string& name, std::shared_ptr<Scene> scene) {
    if (!scene) {
        std::cerr << "Warning: Attempted to add null scene with name: " << name << std::endl;
        return;
    }
    
    m_scenes[name] = scene;
    scene->setName(name);
    
    // If this is the first scene, make it current
    if (!m_currentScene) {
        m_currentScene = scene;
    }
}

void SceneManager::removeScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        // If we're removing the current scene, clear it
        if (m_currentScene == it->second) {
            m_currentScene.reset();
        }
        
        m_scenes.erase(it);
    }
}

std::shared_ptr<Scene> SceneManager::getScene(const std::string& name) const {
    auto it = m_scenes.find(name);
    return (it != m_scenes.end()) ? it->second : nullptr;
}

void SceneManager::setCurrentScene(std::shared_ptr<Scene> scene) {
    if (scene) {
        m_currentScene = scene;
        
        // Make sure the scene is in our collection
        bool found = false;
        for (const auto& pair : m_scenes) {
            if (pair.second == scene) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            // Add it with a default name
            std::string sceneName = scene->getName();
            if (sceneName.empty()) {
                sceneName = "Scene_" + std::to_string(m_scenes.size());
            }
            addScene(sceneName, scene);
        }
    } else {
        m_currentScene.reset();
    }
}

void SceneManager::setCurrentScene(const std::string& name) {
    auto scene = getScene(name);
    if (scene) {
        m_currentScene = scene;
    } else {
        std::cerr << "Warning: Scene '" << name << "' not found!" << std::endl;
    }
}

void SceneManager::update(float deltaTime) {
    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::render(MetalRenderer& renderer) {
    if (m_currentScene) {
        m_currentScene->render(renderer);
    }
}

void SceneManager::clearAllScenes() {
    m_currentScene.reset();
    m_scenes.clear();
}

} // namespace FinalStorm