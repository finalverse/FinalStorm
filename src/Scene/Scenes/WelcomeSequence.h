// ============================================================================
// File: FinalStorm/src/Scene/Scenes/WelcomeSequence.h
// Welcome Sequence - Immersive Onboarding Experience for Finalverse
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "UI/HolographicDisplay.h"
#include "UI/InteractiveOrb.h"
#include "Scene/Scenes/InteractiveGuide.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <queue>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class ConnectionBeam;
class EnergyRing;
class CentralNexus;
class ServiceRing;
class RenderContext;
class AudioEngine;
class Camera;

// ============================================================================
// Welcome Sequence Phases
// ============================================================================

enum class WelcomePhase {
    VOID_AWAKENING,         // Awakening in the cosmic void
    FIRST_LIGHT,           // First glimpse of the Song of Creation
    ECHO_INTRODUCTION,     // Meeting the First Echoes
    NEXUS_REVELATION,      // Discovering the Central Nexus
    SERVICE_DISCOVERY,     // Understanding service visualization
    INTERACTION_TRAINING,  // Learning to interact with the world
    HARMONY_LESSON,        // Understanding the Song and Silence
    FINAL_EMPOWERMENT,     // Becoming a Songweaver
    WORLD_ENTRY            // Entering the full Finalverse experience
};

// ============================================================================
// Welcome Sequence Configuration
// ============================================================================

struct WelcomeConfig {
    // Timing and pacing
    float totalDuration = 300.0f;      // 5 minutes total
    float minPhaseDuration = 15.0f;    // Minimum time per phase
    float maxPhaseDuration = 60.0f;    // Maximum time per phase
    bool allowSkipping = true;
    bool allowPausing = true;
    
    // Visual settings
    bool enableCinematicMode = true;
    bool enableParticleEffects = true;
    bool enableLightingEffects = true;
    float cameraMovementSpeed = 1.0f;
    float transitionSpeed = 1.0f;
    
    // Audio settings
    bool enableNarration = true;
    bool enableMusic = true;
    bool enableSoundEffects = true;
    float narratorVolume = 0.8f;
    float musicVolume = 0.6f;
    float sfxVolume = 0.7f;
    
    // Interaction settings
    bool enableUserInput = true;
    bool requireUserProgression = false; // If true, user must complete each phase
    float interactionTimeout = 30.0f;    // Time before auto-progression
    bool enableGestureControls = true;
    
    // Personalization
    std::string userName = "Songweaver";
    std::string preferredGuideCharacter = "Lumi";
    float userSkillLevel = 0.5f;         // 0.0 = complete beginner, 1.0 = expert
    std::vector<std::string> userInterests;
    
    // Accessibility
    bool enableSubtitles = false;
    bool enableAudioDescription = false;
    bool enableHighContrast = false;
    float textSize = 1.0f;
    bool enableMotionReduction = false;
};

// ============================================================================
// Welcome Phase Data
// ============================================================================

struct WelcomePhaseData {
    WelcomePhase phase;
    std::string title;
    std::string description;
    std::string narrationText;
    std::string audioTrack;
    vec3 cameraPosition;
    vec3 cameraTarget;
    float duration;
    bool requiresInteraction;
    std::string interactionPrompt;
    std::vector<std::string> objectives;
    std::function<bool()> completionCheck;
    std::function<void()> onEnter;
    std::function<void()> onExit;
    std::map<std::string, std::string> metadata;
};

// ============================================================================
// Cinematic Camera Controller
// ============================================================================

class CinematicCamera : public SceneNode {
public:
    enum class CameraMovementType {
        STATIC,             // No movement
        LINEAR,             // Linear interpolation
        SMOOTH,             // Smooth interpolation with easing
        ORBITAL,            // Orbital movement around target
        FOLLOW_PATH,        // Follow predefined path
        DYNAMIC_TRACKING    // Dynamic tracking of target
    };

    CinematicCamera();
    virtual ~CinematicCamera();

    // SceneNode interface
    void update(float deltaTime) override;

    // Camera control
    void setCamera(std::shared_ptr<Camera> camera);
    std::shared_ptr<Camera> getCamera() const { return m_camera; }

    // Movement control
    void moveTo(const vec3& position, const vec3& target, float duration = 3.0f);
    void setMovementType(CameraMovementType type);
    void setMovementSpeed(float speed);
    void setEasing(bool enableEasing);

    // Orbital movement
    void orbitAround(const vec3& center, float radius, float speed, float height = 0.0f);
    void setOrbitParameters(float radius, float speed, float height);

    // Path following
    void followPath(const std::vector<vec3>& pathPoints, const std::vector<vec3>& lookAtPoints, float duration);
    void addPathPoint(const vec3& position, const vec3& lookAt);
    void clearPath();

    // Dynamic tracking
    void trackTarget(std::shared_ptr<SceneNode> target, float distance = 10.0f, float height = 5.0f);
    void setTrackingParameters(float distance, float height, float smoothing);

    // Effects
    void shake(float intensity, float duration);
    void setFieldOfView(float fov, float duration = 1.0f);
    void fade(float alpha, float duration = 1.0f);

    // State queries
    bool isMoving() const { return m_isMoving; }
    bool isOrbiting() const { return m_isOrbiting; }
    bool isFollowingPath() const { return m_isFollowingPath; }
    bool isTracking() const { return m_isTracking; }

private:
    std::shared_ptr<Camera> m_camera;
    
    // Movement state
    CameraMovementType m_movementType;
    bool m_isMoving;
    bool m_easingEnabled;
    float m_movementSpeed;
    
    // Linear/smooth movement
    vec3 m_startPosition;
    vec3 m_targetPosition;
    vec3 m_startLookAt;
    vec3 m_targetLookAt;
    float m_moveProgress;
    float m_moveDuration;
    
    // Orbital movement
    bool m_isOrbiting;
    vec3 m_orbitCenter;
    float m_orbitRadius;
    float m_orbitSpeed;
    float m_orbitHeight;
    float m_orbitAngle;
    
    // Path following
    bool m_isFollowingPath;
    std::vector<vec3> m_pathPoints;
    std::vector<vec3> m_lookAtPoints;
    int m_currentPathIndex;
    float m_pathProgress;
    float m_pathDuration;
    
    // Target tracking
    bool m_isTracking;
    std::weak_ptr<SceneNode> m_trackTarget;
    float m_trackDistance;
    float m_trackHeight;
    float m_trackSmoothing;
    vec3 m_lastTargetPosition;
    
    // Effects
    bool m_isShaking;
    float m_shakeIntensity;
    float m_shakeDuration;
    float m_shakeTimer;
    vec3 m_shakeOffset;
    
    // Helper methods
    vec3 easeInOutCubic(const vec3& start, const vec3& end, float t) const;
    float easeInOutCubic(float start, float end, float t) const;
    void updateMovement(float deltaTime);
    void updateOrbitalMovement(float deltaTime);
    void updatePathFollowing(float deltaTime);
    void updateTargetTracking(float deltaTime);
    void updateShake(float deltaTime);
};

// ============================================================================
// Welcome Narrator - Voice and text narration system
// ============================================================================

class WelcomeNarrator : public SceneNode {
public:
    enum class NarratorVoice {
        LUMI_GENTLE,        // Gentle, childlike voice (Lumi)
        KAI_WISE,          // Wise, analytical voice (KAI)
        TERRA_WARM,        // Warm, nurturing voice (Terra)
        IGNIS_STRONG,      // Strong, encouraging voice (Ignis)
        NEUTRAL_GUIDE      // Neutral guide voice
    };

    WelcomeNarrator();
    virtual ~WelcomeNarrator();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Narration control
    void speak(const std::string& text, float duration = 0.0f); // 0.0 = auto-duration
    void speakWithVoice(const std::string& text, NarratorVoice voice, float duration = 0.0f);
    void pause();
    void resume();
    void stop();
    void setNarrationSpeed(float speed);

    // Voice and audio
    void setNarratorVoice(NarratorVoice voice);
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine);
    void setVolume(float volume);
    void enableSubtitles(bool enable);
    void setSubtitlePosition(const vec3& position);

    // Visual representation
    void setNarratorPosition(const vec3& position);
    void showNarratorCharacter(bool show);
    void setNarratorCharacter(std::shared_ptr<GuideCharacter> character);

    // Subtitle display
    void setSubtitleDisplay(std::shared_ptr<HolographicDisplay> display);
    void setSubtitleStyle(const vec3& color, float size);

    // State queries
    bool isSpeaking() const { return m_isSpeaking; }
    bool isPaused() const { return m_isPaused; }
    float getRemainingTime() const { return m_remainingDuration; }
    std::string getCurrentText() const { return m_currentText; }

private:
    NarratorVoice m_currentVoice;
    std::shared_ptr<AudioEngine> m_audioEngine;
    std::shared_ptr<GuideCharacter> m_narratorCharacter;
    std::shared_ptr<HolographicDisplay> m_subtitleDisplay;
    
    // Narration state
    bool m_isSpeaking;
    bool m_isPaused;
    std::string m_currentText;
    float m_narrationSpeed;
    float m_volume;
    float m_totalDuration;
    float m_remainingDuration;
    
    // Queue for multiple narrations
    struct NarrationEntry {
        std::string text;
        NarratorVoice voice;
        float duration;
    };
    std::queue<NarrationEntry> m_narrationQueue;
    
    // Subtitle settings
    bool m_subtitlesEnabled;
    vec3 m_subtitleColor;
    float m_subtitleSize;
    vec3 m_subtitlePosition;
    
    // Audio IDs for different voices
    std::map<NarratorVoice, std::string> m_voiceAudioIds;
    
    void processNarrationQueue();
    void updateSubtitles();
    std::string getVoiceAudioId(NarratorVoice voice) const;
    float calculateNarrationDuration(const std::string& text) const;
};

// ============================================================================
// WelcomeSequence - Main welcome experience controller
// ============================================================================

class WelcomeSequence : public SceneNode {
public:
    using PhaseCompletionCallback = std::function<void(WelcomePhase phase, bool completed)>;
    using SequenceCompletionCallback = std::function<void(bool completed, float completionTime)>;
    using UserInteractionCallback = std::function<void(const std::string& interaction)>;

    WelcomeSequence(const WelcomeConfig& config = WelcomeConfig{});
    virtual ~WelcomeSequence();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Sequence control
    void startSequence();
    void pauseSequence();
    void resumeSequence();
    void stopSequence();
    void skipCurrentPhase();
    void skipToPhase(WelcomePhase phase);
    void restartSequence();

    // Configuration
    void setConfig(const WelcomeConfig& config);
    const WelcomeConfig& getConfig() const { return m_config; }
    void updateConfig(const WelcomeConfig& config);

    // Component setup
    void setCentralNexus(std::shared_ptr<CentralNexus> nexus);
    void setServiceRing(std::shared_ptr<ServiceRing> serviceRing);
    void setInteractiveGuide(std::shared_ptr<InteractiveGuide> guide);
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine);
    void setMainCamera(std::shared_ptr<Camera> camera);

    // User interaction
    void onUserAction(const std::string& action, const vec3& position = vec3_zero());
    void onUserGesture(const std::string& gesture);
    void onUserVoiceCommand(const std::string& command);
    void onObjectInteraction(const std::string& objectName);

    // Progress tracking
    WelcomePhase getCurrentPhase() const { return m_currentPhase; }
    float getPhaseProgress() const { return m_phaseProgress; }
    float getOverallProgress() const { return m_overallProgress; }
    bool isComplete() const { return m_isComplete; }
    bool isActive() const { return m_isActive; }
    float getElapsedTime() const { return m_elapsedTime; }

    // Callbacks
    void setPhaseCompletionCallback(PhaseCompletionCallback callback) { m_phaseCompletionCallback = callback; }
    void setSequenceCompletionCallback(SequenceCompletionCallback callback) { m_sequenceCompletionCallback = callback; }
    void setUserInteractionCallback(UserInteractionCallback callback) { m_userInteractionCallback = callback; }

    // Customization
    void setWelcomeMessage(const std::string& message);
    void setUserName(const std::string& name);
    void enablePersonalization(bool enable);
    void addCustomPhase(const WelcomePhaseData& phaseData);

    // Analytics
    void recordUserChoice(const std::string& choice);
    void recordInteractionTime(float time);
    std::map<std::string, float> getAnalytics() const;

protected:
    // Phase management
    void initializePhases();
    void enterPhase(WelcomePhase phase);
    void updateCurrentPhase(float deltaTime);
    void exitPhase(WelcomePhase phase);
    void transitionToNextPhase();
    bool checkPhaseCompletion();

    // Individual phase implementations
    void updateVoidAwakening(float deltaTime);
    void updateFirstLight(float deltaTime);
    void updateEchoIntroduction(float deltaTime);
    void updateNexusRevelation(float deltaTime);
    void updateServiceDiscovery(float deltaTime);
    void updateInteractionTraining(float deltaTime);
    void updateHarmonyLesson(float deltaTime);
    void updateFinalEmpowerment(float deltaTime);
    void updateWorldEntry(float deltaTime);

    // Phase setup methods
    void setupVoidAwakening();
    void setupFirstLight();
    void setupEchoIntroduction();
    void setupNexusRevelation();
    void setupServiceDiscovery();
    void setupInteractionTraining();
    void setupHarmonyLesson();
    void setupFinalEmpowerment();
    void setupWorldEntry();

    // Visual effects for phases
    void createVoidEnvironment();
    void createFirstLightEffect();
    void introduceEchoCharacters();
    void revealNexus();
    void demonstrateServices();
    void showInteractionPossibilities();
    void visualizeHarmonyAndSilence();
    void grantSongweavingPowers();
    void openToFullWorld();

    // Audio and narration
    void playPhaseNarration(WelcomePhase phase);
    void playPhaseMusic(WelcomePhase phase);
    void playTransitionEffect();

    // User guidance
    void showPhaseInstructions(WelcomePhase phase);
    void highlightInteractableElements();
    void provideContextualHelp();

    // Event handling
    void dispatchPhaseCompletion(WelcomePhase phase, bool completed);
    void dispatchSequenceCompletion(bool completed);
    void dispatchUserInteraction(const std::string& interaction);

private:
    // Configuration
    WelcomeConfig m_config;

    // Sequence state
    bool m_isActive;
    bool m_isPaused;
    bool m_isComplete;
    WelcomePhase m_currentPhase;
    float m_phaseProgress;
    float m_overallProgress;
    float m_elapsedTime;
    float m_phaseStartTime;
    uint64_t m_sequenceStartTime;

    // Phase data
    std::map<WelcomePhase, WelcomePhaseData> m_phaseData;
    std::vector<WelcomePhase> m_phaseOrder;
    int m_currentPhaseIndex;

    // Components
    std::shared_ptr<CinematicCamera> m_cinematicCamera;
    std::shared_ptr<WelcomeNarrator> m_narrator;
    std::shared_ptr<CentralNexus> m_centralNexus;
    std::shared_ptr<ServiceRing> m_serviceRing;
    std::shared_ptr<InteractiveGuide> m_interactiveGuide;
    std::shared_ptr<AudioEngine> m_audioEngine;

    // Visual elements
    std::vector<std::shared_ptr<ParticleEmitter>> m_phaseParticles;
    std::vector<std::shared_ptr<ConnectionBeam>> m_phaseConnections;
    std::vector<std::shared_ptr<EnergyRing>> m_phaseRings;
    std::shared_ptr<HolographicDisplay> m_instructionDisplay;
    std::shared_ptr<HolographicDisplay> m_progressDisplay;

    // Echo characters for introduction
    std::shared_ptr<GuideCharacter> m_lumiCharacter;
    std::shared_ptr<GuideCharacter> m_kaiCharacter;
    std::shared_ptr<GuideCharacter> m_terraCharacter;
    std::shared_ptr<GuideCharacter> m_ignisCharacter;

    // Audio assets
    std::map<WelcomePhase, std::string> m_phaseMusic;
    std::map<WelcomePhase, std::string> m_phaseNarration;
    std::string m_transitionSound;

    // User interaction tracking
    std::vector<std::string> m_userChoices;
    std::vector<float> m_interactionTimes;
    std::map<std::string, int> m_interactionCounts;
    float m_totalInteractionTime;

    // Callbacks
    PhaseCompletionCallback m_phaseCompletionCallback;
    SequenceCompletionCallback m_sequenceCompletionCallback;
    UserInteractionCallback m_userInteractionCallback;

    // Personalization
    bool m_personalizationEnabled;
    std::string m_userName;
    std::string m_welcomeMessage;
    std::vector<std::string> m_customizationChoices;

    // Helper methods
    WelcomePhaseData createPhaseData(WelcomePhase phase, const std::string& title, const std::string& description);
    void updateProgress();
    void cleanupPhaseElements();
    float calculatePhaseDuration(WelcomePhase phase) const;
    bool shouldAutoProgress() const;
    void schedulePhaseTransition(float delay);
};

// ============================================================================
// WelcomeSequenceBuilder - Builder pattern for creating custom welcome sequences
// ============================================================================

class WelcomeSequenceBuilder {
public:
    WelcomeSequenceBuilder();

    // Basic configuration
    WelcomeSequenceBuilder& setTotalDuration(float duration);
    WelcomeSequenceBuilder& enableSkipping(bool enable);
    WelcomeSequenceBuilder& enableNarration(bool enable);
    WelcomeSequenceBuilder& setUserName(const std::string& name);
    WelcomeSequenceBuilder& setUserSkillLevel(float level);

    // Phase customization
    WelcomeSequenceBuilder& customizePhase(WelcomePhase phase, const WelcomePhaseData& data);
    WelcomeSequenceBuilder& setPhaseDuration(WelcomePhase phase, float duration);
    WelcomeSequenceBuilder& setPhaseNarration(WelcomePhase phase, const std::string& text);
    WelcomeSequenceBuilder& setPhaseMusic(WelcomePhase phase, const std::string& audioTrack);
    WelcomeSequenceBuilder& requirePhaseInteraction(WelcomePhase phase, bool required);

    // Audio and visual settings
    WelcomeSequenceBuilder& setNarratorVoice(WelcomeNarrator::NarratorVoice voice);
    WelcomeSequenceBuilder& setCameraMovementSpeed(float speed);
    WelcomeSequenceBuilder& enableCinematicMode(bool enable);
    WelcomeSequenceBuilder& enableParticleEffects(bool enable);

    // Accessibility
    WelcomeSequenceBuilder& enableSubtitles(bool enable);
    WelcomeSequenceBuilder& enableHighContrast(bool enable);
    WelcomeSequenceBuilder& setTextSize(float size);
    WelcomeSequenceBuilder& enableMotionReduction(bool enable);

    // Build the configuration
    WelcomeConfig build();
    std::shared_ptr<WelcomeSequence> buildSequence();

private:
    WelcomeConfig m_config;
    std::map<WelcomePhase, WelcomePhaseData> m_customPhases;
    WelcomeNarrator::NarratorVoice m_narratorVoice;
};

// ============================================================================
// Predefined Welcome Sequences
// ============================================================================

class WelcomeSequencePresets {
public:
    // Standard presets
    static WelcomeConfig createStandardWelcome();
    static WelcomeConfig createQuickWelcome();
    static WelcomeConfig createDetailedWelcome();
    static WelcomeConfig createAccessibleWelcome();

    // Skill-based presets
    static WelcomeConfig createBeginnerWelcome();
    static WelcomeConfig createIntermediateWelcome();
    static WelcomeConfig createExpertWelcome();

    // Theme-based presets
    static WelcomeConfig createFinalverseWelcome();
    static WelcomeConfig createTechnicalWelcome();
    static WelcomeConfig createArtisticWelcome();

    // Special presets
    static WelcomeConfig createDemoWelcome();
    static WelcomeConfig createTestingWelcome();
    static WelcomeConfig createKioskWelcome(); // For public demonstrations

private:
    static WelcomeConfig getBaseConfig();
    static void applySkillLevel(WelcomeConfig& config, float skillLevel);
    static void applyAccessibilityOptions(WelcomeConfig& config);
    static void applyTheme(WelcomeConfig& config, const std::string& theme);
};

// ============================================================================
// Welcome Analytics - Track user experience and optimize onboarding
// ============================================================================

class WelcomeAnalytics {
public:
    struct CompletionData {
        std::string userId;
        uint64_t timestamp;
        float totalDuration;
        std::vector<float> phaseDurations;
        std::vector<bool> phaseCompletions;
        std::vector<bool> phaseSkips;
        int totalInteractions;
        std::vector<std::string> userChoices;
        float userSkillLevel;
        bool completed;
        std::string exitReason;
    };

    WelcomeAnalytics();
    virtual ~WelcomeAnalytics();

    // Data collection
    void recordWelcomeStart(const std::string& userId, float userSkillLevel);
    void recordPhaseCompletion(WelcomePhase phase, float duration, bool completed, bool skipped);
    void recordUserInteraction(const std::string& interaction);
    void recordUserChoice(const std::string& choice);
    void recordWelcomeCompletion(bool completed, const std::string& exitReason = "");

    // Analytics queries
    float getAverageCompletionTime() const;
    float getCompletionRate() const;
    std::vector<WelcomePhase> getMostSkippedPhases() const;
    std::vector<WelcomePhase> getMostDifficultPhases() const;
    float getAveragePhaseTime(WelcomePhase phase) const;
    std::map<std::string, int> getCommonUserChoices() const;

    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    float getRecommendedPhaseDuration(WelcomePhase phase) const;
    bool shouldPhaseRequireInteraction(WelcomePhase phase) const;
    float getOptimalNarrationSpeed() const;

    // Reporting
    void generateAnalyticsReport() const;
    void exportData(const std::string& filename) const;
    void clearData();

private:
    std::vector<CompletionData> m_completionHistory;
    CompletionData m_currentSession;
    bool m_sessionActive;
    
    // Helper methods
    void updateCurrentSession();
    float calculateVariance(const std::vector<float>& data) const;
    std::vector<float> getPhaseTimeData(WelcomePhase phase) const;
};

// ============================================================================
// Welcome Sequence Manager - Global management of welcome experiences
// ============================================================================

class WelcomeSequenceManager {
public:
    static WelcomeSequenceManager& getInstance();

    // Sequence management
    void registerSequence(const std::string& name, const WelcomeConfig& config);
    void unregisterSequence(const std::string& name);
    std::vector<std::string> getAvailableSequences() const;
    
    // Sequence creation
    std::shared_ptr<WelcomeSequence> createSequence(const std::string& sequenceName = "standard");
    std::shared_ptr<WelcomeSequence> createCustomSequence(const WelcomeConfig& config);
    std::shared_ptr<WelcomeSequence> createAdaptiveSequence(float userSkillLevel, const std::vector<std::string>& userPreferences);

    // Global settings
    void setDefaultNarratorVoice(WelcomeNarrator::NarratorVoice voice);
    void setGlobalAudioEnabled(bool enabled);
    void setGlobalSubtitlesEnabled(bool enabled);
    void setDefaultSkillLevel(float level);

    // Analytics integration
    void setAnalytics(std::shared_ptr<WelcomeAnalytics> analytics);
    std::shared_ptr<WelcomeAnalytics> getAnalytics() const;
    void enableAnalytics(bool enable);

    // User preferences
    void setUserPreferences(const std::map<std::string, std::string>& preferences);
    void setAccessibilityOptions(const std::map<std::string, bool>& options);

    // Optimization
    void optimizeSequencesBasedOnAnalytics();
    void updateSequenceBasedOnFeedback(const std::string& sequenceName, const std::vector<std::string>& feedback);

private:
    WelcomeSequenceManager() = default;
    ~WelcomeSequenceManager() = default;

    std::map<std::string, WelcomeConfig> m_registeredSequences;
    std::shared_ptr<WelcomeAnalytics> m_analytics;
    
    // Global settings
    WelcomeNarrator::NarratorVoice m_defaultNarratorVoice = WelcomeNarrator::NarratorVoice::LUMI_GENTLE;
    bool m_globalAudioEnabled = true;
    bool m_globalSubtitlesEnabled = false;
    float m_defaultSkillLevel = 0.5f;
    bool m_analyticsEnabled = true;
    
    // User settings
    std::map<std::string, std::string> m_userPreferences;
    std::map<std::string, bool> m_accessibilityOptions;
    
    void initializeDefaultSequences();
    WelcomeConfig adaptConfigForUser(const WelcomeConfig& baseConfig, float skillLevel, const std::vector<std::string>& preferences) const;
};

// ============================================================================
// Interactive Welcome Elements - Special interactive components for welcome sequence
// ============================================================================

class WelcomeInteractiveElement : public SceneNode {
public:
    enum class ElementType {
        TOUCH_TO_CONTINUE,
        GESTURE_PROMPT,
        VOICE_ACTIVATION,
        GAZE_TARGET,
        PROXIMITY_TRIGGER,
        CHOICE_SELECTOR
    };

    WelcomeInteractiveElement(ElementType type, const std::string& prompt);
    virtual ~WelcomeInteractiveElement();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;
    void onInteraction(const vec3& worldPosition, const std::string& interactionType) override;

    // Element configuration
    void setPromptText(const std::string& text);
    void setInteractionRadius(float radius);
    void setTimeout(float timeout);
    void setRequiredGesture(const std::string& gesture);
    void setChoices(const std::vector<std::string>& choices);

    // Visual customization
    void setPulseRate(float rate);
    void setGlowColor(const vec3& color);
    void setPromptPosition(const vec3& position);
    void enableVisualCues(bool enable);

    // State management
    void activate();
    void deactivate();
    void reset();
    bool isActivated() const { return m_isActivated; }
    bool isCompleted() const { return m_isCompleted; }
    std::string getResult() const { return m_result; }

    // Callbacks
    void setCompletionCallback(std::function<void(const std::string&)> callback);
    void setTimeoutCallback(std::function<void()> callback);

private:
    ElementType m_elementType;
    std::string m_promptText;
    float m_interactionRadius;
    float m_timeout;
    std::string m_requiredGesture;
    std::vector<std::string> m_choices;
    
    // State
    bool m_isActivated;
    bool m_isCompleted;
    std::string m_result;
    float m_timeRemaining;
    
    // Visual properties
    float m_pulseRate;
    vec3 m_glowColor;
    vec3 m_promptPosition;
    bool m_visualCuesEnabled;
    
    // Visual components
    std::shared_ptr<InteractiveOrb> m_interactionOrb;
    std::shared_ptr<HolographicDisplay> m_promptDisplay;
    std::shared_ptr<ParticleEmitter> m_highlightParticles;
    
    // Animation state
    float m_pulsePhase;
    float m_glowPhase;
    
    // Callbacks
    std::function<void(const std::string&)> m_completionCallback;
    std::function<void()> m_timeoutCallback;
    
    void createVisualComponents();
    void updateVisualEffects(float deltaTime);
    void handleInteractionByType(const std::string& interactionType);
};

} // namespace FinalStorm