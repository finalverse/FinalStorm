// SceneLoader.cpp - Implementation

#include "Scene/SceneLoader.h"
#include "World/WorldManager.h"
#include "Rendering/Metal/MetalRenderer.h"
#include "Core/Networking/FinalverseClient.h"
#include <iostream>

namespace FinalStorm {

SceneLoader::SceneLoader(WorldManager* worldManager, 
                        MetalRenderer* renderer,
                        FinalverseClient* networkClient) 
    : m_worldManager(worldManager),
      m_renderer(renderer),
      m_networkClient(networkClient) {
    
    registerDefaultScenes();
}

SceneLoader::~SceneLoader() {
    if (m_currentScene) {
        m_currentScene->onExit();
    }
}

void SceneLoader::loadScene(const std::string& sceneName) {
    auto it = m_sceneFactories.find(sceneName);
    if (it == m_sceneFactories.end()) {
        std::cerr << "Scene not found: " << sceneName << std::endl;
        return;
    }
    
    // Exit current scene
    if (m_currentScene) {
        m_currentScene->onExit();
    }
    
    // Create and initialize new scene
    m_currentScene = it->second();
    m_currentSceneName = sceneName;
    
    if (m_currentScene) {
        m_currentScene->initialize();
        m_currentScene->onEnter();
        
        // Set the scene in world manager
        m_worldManager->setCurrentScene(m_currentScene.get());
    }
}

void SceneLoader::loadFirstScene() {
    // Load configuration
    FirstSceneConfig config = FirstSceneConfig::loadFromPreferences();
    
    // Create the first scene with config
    auto createFirstScene = [config]() -> std::unique_ptr<Scene> {
        auto scene = std::make_unique<FirstScene>();
        
        // Apply configuration
        // TODO: Add config setters to FirstScene
        
        return scene;
    };
    
    // Register if not already
    registerSceneFactory("TheNexus", createFirstScene);
    
    // Load the scene
    loadScene("TheNexus");
}

void SceneLoader::loadSceneFromServer(const std::string& sceneId) {
    if (!m_networkClient) {
        std::cerr << "No network client available" << std::endl;
        return;
    }
    
    m_isLoading = true;
    m_loadingSceneId = sceneId;
    
    // Request scene data from server
    m_networkClient->requestScene(sceneId, 
        [this](const SceneData& data) {
            // Create scene from server data
            auto scene = std::make_unique<Scene>(data.name);
            
            // TODO: Populate scene from server data
            
            // Register and load
            registerSceneFactory(data.name, [scene = std::move(scene)]() mutable {
                return std::move(scene);
            });
            
            loadScene(data.name);
            m_isLoading = false;
        },
        [this](const std::string& error) {
            std::cerr << "Failed to load scene: " << error << std::endl;
            m_isLoading = false;
        }
    );
}

void SceneLoader::transitionToScene(const std::string& sceneName, float duration) {
    if (m_transitionState != TransitionState::NONE) {
        std::cerr << "Transition already in progress" << std::endl;
        return;
    }
    
    startTransition(sceneName, duration);
}

void SceneLoader::fadeToScene(const std::string& sceneName, float duration) {
    // Fade is a specific type of transition
    transitionToScene(sceneName, duration);
}

void SceneLoader::update(float deltaTime) {
    if (m_transitionState != TransitionState::NONE) {
        updateTransition(deltaTime);
    }
    
    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneLoader::registerSceneFactory(const std::string& name, 
                                     std::function<std::unique_ptr<Scene>()> factory) {
    m_sceneFactories[name] = factory;
}

void SceneLoader::registerDefaultScenes() {
    // Register The Nexus as default first scene
    registerSceneFactory("TheNexus", []() {
        return std::make_unique<FirstScene>();
    });
    
    // Register other built-in scenes
    // TODO: Add more default scenes
}

void SceneLoader::startTransition(const std::string& sceneName, float duration) {
    m_nextSceneName = sceneName;
    m_transitionDuration = duration;
    m_transitionTime = 0.0f;
    m_transitionState = TransitionState::FADE_OUT;
    
    // Start fade out effect
    m_renderer->setFadeAlpha(0.0f);
}

void SceneLoader::updateTransition(float deltaTime) {
    m_transitionTime += deltaTime;
    
    switch (m_transitionState) {
        case TransitionState::FADE_OUT: {
            float t = m_transitionTime / (m_transitionDuration * 0.5f);
            t = glm::clamp(t, 0.0f, 1.0f);
            m_renderer->setFadeAlpha(t);
            
            if (t >= 1.0f) {
                m_transitionState = TransitionState::LOADING;
                m_transitionTime = 0.0f;
                
                // Load the next scene
                auto it = m_sceneFactories.find(m_nextSceneName);
                if (it != m_sceneFactories.end()) {
                    m_nextScene = it->second();
                    if (m_nextScene) {
                        m_nextScene->initialize();
                    }
                }
            }
            break;
        }
        
        case TransitionState::LOADING: {
            // Could show loading screen here
            m_transitionState = TransitionState::FADE_IN;
            m_transitionTime = 0.0f;
            completeTransition();
            break;
        }
        
        case TransitionState::FADE_IN: {
            float t = m_transitionTime / (m_transitionDuration * 0.5f);
            t = glm::clamp(t, 0.0f, 1.0f);
            m_renderer->setFadeAlpha(1.0f - t);
            
            if (t >= 1.0f) {
                m_transitionState = TransitionState::NONE;
                m_renderer->setFadeAlpha(0.0f);
            }
            break;
        }
        
        default:
            break;
    }
}

void SceneLoader::completeTransition() {
    // Exit current scene
    if (m_currentScene) {
        m_currentScene->onExit();
    }
    
    // Switch to next scene
    m_currentScene = std::move(m_nextScene);
    m_currentSceneName = m_nextSceneName;
    
    if (m_currentScene) {
        m_currentScene->onEnter();
        m_worldManager->setCurrentScene(m_currentScene.get());
    }
}

// ScenePreloader Implementation
ScenePreloader::ScenePreloader(MetalRenderer* renderer) 
    : m_renderer(renderer) {
}

void ScenePreloader::preloadFirstScene() {
    preloadSceneAssets("TheNexus");
}

void ScenePreloader::preloadSceneAssets(const std::string& sceneName) {
    if (m_loadedScenes.find(sceneName) != m_loadedScenes.end()) {
        return; // Already loaded
    }
    
    m_isLoading = true;
    m_currentLoadingScene = sceneName;
    m_loadProgress = 0.0f;
    
    // Load assets in stages
    loadTextures(sceneName);
    m_loadProgress = 0.33f;
    
    loadMeshes(sceneName);
    m_loadProgress = 0.66f;
    
    loadShaders(sceneName);
    m_loadProgress = 1.0f;
    
    m_loadedScenes.insert(sceneName);
    m_isLoading = false;
}

bool ScenePreloader::areAssetsLoaded(const std::string& sceneName) const {
    return m_loadedScenes.find(sceneName) != m_loadedScenes.end();
}

void ScenePreloader::loadTextures(const std::string& sceneName) {
    if (sceneName == "TheNexus") {
        // Preload textures for The Nexus scene
        m_renderer->loadTexture("noise", "assets/textures/noise.png");
        m_renderer->loadTexture("starfield", "assets/textures/starfield.png");
        m_renderer->loadCubemap("nebula_skybox", "assets/textures/skybox/nebula");
        
        // Particle textures
        m_renderer->loadTexture("glow_particle", "assets/textures/particles/glow.png");
        m_renderer->loadTexture("sparkle", "assets/textures/particles/sparkle.png");
        
        // UI textures
        m_renderer->loadTexture("ui_border", "assets/textures/ui/border.png");
        m_renderer->loadTexture("ui_glow", "assets/textures/ui/glow.png");
    }
}

void ScenePreloader::loadMeshes(const std::string& sceneName) {
    if (sceneName == "TheNexus") {
        // Generate procedural meshes
        m_renderer->generateMesh(MeshLibrary::OCTAHEDRON, "octahedron");
        m_renderer->generateMesh(MeshLibrary::ICOSAHEDRON, "icosahedron");
        m_renderer->generateMesh(MeshLibrary::TORUS, "torus");
        m_renderer->generateMesh(MeshLibrary::CYLINDER, "cylinder");
        m_renderer->generateMesh(MeshLibrary::SPHERE, "sphere", 32, 32);
        m_renderer->generateMesh(MeshLibrary::CUBE, "cube");
        
        // Special meshes
        m_renderer->generateMesh(Mesh    void loadTextures(const std::string& sceneName);
    void loadMeshes(const std::string& sceneName);
    void loadShaders(const std::string& sceneName);
};

/**
 * FirstSceneConfig - Configuration for the first scene
 */
struct FirstSceneConfig {
    // Visual settings
    bool showWelcomeSequence = true;
    float nexusScale = 1.0f;
    float serviceRingRadius = 12.0f;
    
    // Environment settings
    glm::vec3 nebulaPrimaryColor = glm::vec3(0.1f, 0.2f, 0.4f);
    glm::vec3 nebulaSecondaryColor = glm::vec3(0.4f, 0.1f, 0.3f);
    float nebulaDensity = 0.7f;
    
    // Service settings
    bool autoDiscoverServices = true;
    float serviceUpdateInterval = 1.0f;
    
    // Camera settings
    glm::vec3 defaultCameraPosition = glm::vec3(15, 10, 15);
    glm::vec3 defaultCameraLookAt = glm::vec3(0, 0, 0);
    float defaultCameraFOV = 60.0f;
    
    // User preferences
    static FirstSceneConfig loadFromPreferences();
    void saveToPreferences() const;
};

} // namespace FinalStorm