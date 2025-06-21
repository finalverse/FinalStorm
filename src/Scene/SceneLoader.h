// SceneLoader.h - Scene management and loading for FinalStorm
#pragma once

#include "Scene/Scene.h"
#include "Scene/Scenes/FirstScene.h"
#include <memory>
#include <map>
#include <string>

namespace FinalStorm {

    // Forward declarations
    class WorldManager;
    class MetalRenderer;
    class FinalverseClient;

    /**
     * SceneLoader - Manages scene loading and transitions
     */
    class SceneLoader {
    public:
        SceneLoader(WorldManager* worldManager, 
                    MetalRenderer* renderer,
                    FinalverseClient* networkClient);
        ~SceneLoader();
        
        // Scene management
        void loadScene(const std::string& sceneName);
        void loadFirstScene();
        void loadSceneFromServer(const std::string& sceneId);
        
        // Scene transitions
        void transitionToScene(const std::string& sceneName, float duration = 1.0f);
        void fadeToScene(const std::string& sceneName, float duration = 0.5f);
        
        // Current scene access
        Scene* getCurrentScene() { return m_currentScene.get(); }
        const std::string& getCurrentSceneName() const { return m_currentSceneName; }
        
        // Scene registration
        void registerSceneFactory(const std::string& name, 
                                std::function<std::unique_ptr<Scene>()> factory);
        
        // Update
        void update(float deltaTime);
        
    private:
        WorldManager* m_worldManager;
        MetalRenderer* m_renderer;
        FinalverseClient* m_networkClient;
        
        // Scene management
        std::unique_ptr<Scene> m_currentScene;
        std::unique_ptr<Scene> m_nextScene;
        std::string m_currentSceneName;
        std::string m_nextSceneName;
        
        // Scene factories
        std::map<std::string, std::function<std::unique_ptr<Scene>()>> m_sceneFactories;
        
        // Transition state
        enum class TransitionState {
            NONE,
            FADE_OUT,
            LOADING,
            FADE_IN
        };
        
        TransitionState m_transitionState = TransitionState::NONE;
        float m_transitionTime = 0.0f;
        float m_transitionDuration = 0.0f;
        
        // Loading state
        bool m_isLoading = false;
        std::string m_loadingSceneId;
        
        void registerDefaultScenes();
        void startTransition(const std::string& sceneName, float duration);
        void updateTransition(float deltaTime);
        void completeTransition();
    };

    /**
     * ScenePreloader - Preloads scene assets
     */
    class ScenePreloader {
    public:
        ScenePreloader(MetalRenderer* renderer);
        
        // Preload scene assets
        void preloadFirstScene();
        void preloadSceneAssets(const std::string& sceneName);
        
        // Check if assets are loaded
        bool areAssetsLoaded(const std::string& sceneName) const;
        
        // Progress tracking
        float getLoadProgress() const { return m_loadProgress; }
        bool isLoading() const { return m_isLoading; }
        
    private:
        MetalRenderer* m_renderer;
        
        // Loading state
        bool m_isLoading = false;
        float m_loadProgress = 0.0f;
        std::string m_currentLoadingScene;
        
        // Loaded assets
        std::set<std::string> m_loadedScenes;
        
            void loadTextures(const std::string& sceneName);
        void loadMeshes(const std::string& sceneName);
        void loadShaders(const std::string& sceneName);
    };

} // namespace FinalStorm