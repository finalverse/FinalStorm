// ============================================================================
// File: FinalStorm/src/Scene/Scenes/InteractiveGuide.h
// Interactive Guide System - Intuitive Tutorial for Finalverse Navigation
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "UI/HolographicDisplay.h"
#include "UI/InteractiveOrb.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <queue>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class ConnectionBeam;
class RenderContext;
class AudioEngine;

// ============================================================================
// Guide Step Types - Different types of tutorial interactions
// ============================================================================

enum class GuideStepType {
    INTRODUCTION,       // Welcome and orientation
    POINT_AND_CLICK,   // Point to something and explain
    GESTURE_DEMO,      // Show gesture/interaction
    INTERACTIVE_TASK,  // User must complete an action
    INFORMATION,       // Display information panel
    AUDIO_NARRATION,   // Audio-only guidance
    VISUAL_HIGHLIGHT,  // Highlight specific elements
    MINI_GAME,         // Interactive mini-game tutorial
    ASSESSMENT,        // Test user understanding
    CELEBRATION        // Completion celebration
};

// ============================================================================
// Guide Step Configuration
// ============================================================================

struct GuideStep {
    std::string id;
    GuideStepType type;
    std::string title;
    std::string description;
    std::string audioNarration;
    vec3 focusPosition;
    float focusRadius;
    float duration; // 0.0 = wait for user action
    bool skippable;
    bool repeatable;
    
    // Visual properties
    vec3 highlightColor = make_vec3(1.0f, 1.0f, 0.0f);
    float highlightIntensity = 1.0f;
    bool showPointer = true;
    bool showTextPanel = true;
    
    // Interactive properties
    std::string targetObjectName;
    std::string requiredAction; // "click", "gesture", "voice", etc.
    std::vector<std::string> validActions;
    float timeLimit = 0.0f; // 0.0 = no time limit
    
    // Completion criteria
    std::function<bool()> completionCheck;
    std::function<void()> onComplete;
    std::function<void()> onSkip;
    std::function<void()> onTimeout;
    
    // Dependencies
    std::vector<std::string> prerequisites;
    std::vector<std::string> unlocks;
    
    // Metadata
    std::map<std::string, std::string> metadata;
};

// ============================================================================
// Guide Progress Tracking
// ============================================================================

struct GuideProgress {
    std::string currentStepId;
    int currentStepIndex = 0;
    int totalSteps = 0;
    float currentStepProgress = 0.0f;
    float overallProgress = 0.0f;
    bool isComplete = false;
    bool isSkipped = false;
    
    std::vector<std::string> completedSteps;
    std::vector<std::string> skippedSteps;
    std::map<std::string, float> stepScores;
    float overallScore = 0.0f;
    
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    uint64_t totalDuration = 0;
};

// ============================================================================
// Guide Pointer - Visual pointing system
// ============================================================================

class GuidePointer : public SceneNode {
public:
    enum class PointerStyle {
        ARROW,          // Classic arrow pointer
        BEAM,           // Energy beam from guide to target
        PULSE,          // Pulsing highlight around target
        ORBIT,          // Orbiting particles around target
        GLOW,           // Glowing outline
        SPOTLIGHT       // Light cone pointing to target
    };

    GuidePointer(PointerStyle style = PointerStyle::ARROW);
    virtual ~GuidePointer();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Pointer control
    void setTarget(const vec3& targetPosition);
    void setSourcePosition(const vec3& sourcePosition);
    void setStyle(PointerStyle style);
    void setColor(const vec3& color);
    void setIntensity(float intensity);
    void setPulseRate(float rate);
    void setAnimationSpeed(float speed);

    // Visibility and animation
    void show(float fadeInTime = 0.5f);
    void hide(float fadeOutTime = 0.5f);
    void pulse(float intensity = 1.5f);
    void setVisible(bool visible) override;

    // State queries
    PointerStyle getStyle() const { return m_style; }
    vec3 getTarget() const { return m_targetPosition; }
    bool isPointingAt(const vec3& position, float tolerance = 1.0f) const;

private:
    PointerStyle m_style;
    vec3 m_targetPosition;
    vec3 m_sourcePosition;
    vec3 m_color;
    float m_intensity;
    float m_pulseRate;
    float m_animationSpeed;
    
    // Animation state
    float m_animationPhase;
    float m_pulsePhase;
    float m_fadeAlpha;
    bool m_isVisible;
    bool m_isFading;
    float m_fadeDirection;
    float m_fadeSpeed;
    
    // Visual components
    std::shared_ptr<ParticleEmitter> m_particles;
    std::shared_ptr<ConnectionBeam> m_beam;
    
    void createVisualComponents();
    void updateAnimation(float deltaTime);
    void updateFading(float deltaTime);
    void renderPointer(RenderContext& context);
};

// ============================================================================
// Guide Character - Virtual assistant/mascot
// ============================================================================

class GuideCharacter : public SceneNode {
public:
    enum class CharacterType {
        LUMI_ECHO,      // Lumi from Finalverse lore
        HOLOGRAM,       // Generic holographic assistant
        ENERGY_BEING,   // Abstract energy creature
        FLOATING_ORB,   // Simple orb with personality
        AI_AVATAR       // AI representation
    };

    enum class CharacterState {
        IDLE,
        SPEAKING,
        POINTING,
        CELEBRATING,
        THINKING,
        CONCERNED,
        EXCITED
    };

    GuideCharacter(CharacterType type = CharacterType::LUMI_ECHO);
    virtual ~GuideCharacter();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Character control
    void setCharacterType(CharacterType type);
    void setState(CharacterState state);
    void setMood(float happiness, float excitement, float concern);
    void setSize(float size);
    void setColor(const vec3& color);

    // Animation and behavior
    void speak(const std::string& text, float duration = 3.0f);
    void pointTo(const vec3& position);
    void celebrate();
    void expressConfusion();
    void expressEncouragement();
    void floatTo(const vec3& position, float duration = 2.0f);
    void orbitAround(const vec3& center, float radius, float speed);

    // Interaction
    void setFollowUser(bool follow);
    void setLookAtUser(bool lookAt);
    void reactToUserAction(const std::string& action);
    void onUserSuccess();
    void onUserMistake();

    // State queries
    CharacterType getType() const { return m_characterType; }
    CharacterState getState() const { return m_currentState; }
    bool isSpeaking() const { return m_isSpeaking; }
    bool isMoving() const { return m_isMoving; }

private:
    CharacterType m_characterType;
    CharacterState m_currentState;
    CharacterState m_previousState;
    
    // Mood and personality
    float m_happiness;
    float m_excitement;
    float m_concern;
    vec3 m_baseColor;
    float m_size;
    
    // Animation state
    float m_animationPhase;
    float m_bobPhase;
    float m_glowPhase;
    bool m_isSpeaking;
    bool m_isMoving;
    bool m_followUser;
    bool m_lookAtUser;
    
    // Movement
    vec3 m_targetPosition;
    vec3 m_startPosition;
    float m_moveProgress;
    float m_moveDuration;
    bool m_isOrbiting;
    vec3 m_orbitCenter;
    float m_orbitRadius;
    float m_orbitSpeed;
    float m_orbitPhase;
    
    // Visual components
    std::shared_ptr<InteractiveOrb> m_characterOrb;
    std::shared_ptr<ParticleEmitter> m_expressionParticles;
    std::shared_ptr<HolographicDisplay> m_speechBubble;
    
    void createCharacterVisuals();
    void updateAnimation(float deltaTime);
    void updateMovement(float deltaTime);
    void updateExpression(float deltaTime);
    void applyMoodToVisuals();
};

// ============================================================================
// InteractiveGuide - Main guide system
// ============================================================================

class InteractiveGuide : public SceneNode {
public:
    using StepCompletionCallback = std::function<void(const std::string& stepId, bool completed)>;
    using GuideCompletionCallback = std::function<void(const GuideProgress& progress)>;
    using UserActionCallback = std::function<void(const std::string& action, const vec3& position)>;

    InteractiveGuide();
    virtual ~InteractiveGuide();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Guide setup
    void loadGuideSequence(const std::string& sequenceName);
    void addStep(const GuideStep& step);
    void removeStep(const std::string& stepId);
    void clearSteps();
    void setGuideTitle(const std::string& title);
    void setGuideDescription(const std::string& description);

    // Guide execution
    void startGuide();
    void pauseGuide();
    void resumeGuide();
    void stopGuide();
    void restartGuide();
    void skipCurrentStep();
    void goToStep(const std::string& stepId);
    void goToStep(int stepIndex);
    void nextStep();
    void previousStep();

    // User interaction
    void onUserAction(const std::string& action, const vec3& position = vec3_zero());
    void onObjectInteraction(const std::string& objectName, const std::string& action);
    void onGestureDetected(const std::string& gesture);
    void onVoiceCommand(const std::string& command);

    // Configuration
    void setGuideCharacter(std::shared_ptr<GuideCharacter> character);
    void setGuidePointer(std::shared_ptr<GuidePointer> pointer);
    void setAudioEngine(std::shared_ptr<AudioEngine> audioEngine);
    void enableAudioNarration(bool enable);
    void enableVisualHighlights(bool enable);
    void enableUserInput(bool enable);
    void setGuideSpeed(float speed); // Animation/transition speed multiplier

    // Progress tracking
    const GuideProgress& getProgress() const { return m_progress; }
    float getOverallProgress() const { return m_progress.overallProgress; }
    bool isComplete() const { return m_progress.isComplete; }
    bool isActive() const { return m_isActive; }
    const GuideStep* getCurrentStep() const;
    int getCurrentStepIndex() const { return m_progress.currentStepIndex; }

    // Callbacks
    void setStepCompletionCallback(StepCompletionCallback callback) { m_stepCompletionCallback = callback; }
    void setGuideCompletionCallback(GuideCompletionCallback callback) { m_guideCompletionCallback = callback; }
    void setUserActionCallback(UserActionCallback callback) { m_userActionCallback = callback; }

    // Visual customization
    void setHighlightColor(const vec3& color);
    void setPointerStyle(GuidePointer::PointerStyle style);
    void setCharacterType(GuideCharacter::CharacterType type);
    void setUITheme(const std::string& theme);

    // Adaptive features
    void enableAdaptivePacing(bool enable);
    void setUserSkillLevel(float level); // 0.0 = beginner, 1.0 = expert
    void enableContextualHelp(bool enable);
    void setHelpThreshold(float threshold); // How long before offering help

    // Analytics and feedback
    void recordUserAction(const std::string& action, bool successful);
    void trackStepDuration(const std::string& stepId, float duration);
    float getAverageStepTime() const;
    float getUserSuccessRate() const;
    // Analytics and feedback
    void recordUserAction(const std::string& action, bool successful);
    void trackStepDuration(const std::string& stepId, float duration);
    float getAverageStepTime() const;
    float getUserSuccessRate() const;
    std::vector<std::string> getUserDifficulties() const;
    void generateProgressReport() const;

protected:
    // Step management
    void initializeStep(const GuideStep& step);
    void updateCurrentStep(float deltaTime);
    void completeCurrentStep(bool successful = true);
    void transitionToNextStep();
    bool checkStepCompletion();
    void handleStepTimeout();

    // Visual effects
    void showStepHighlight();
    void hideStepHighlight();
    void updateHighlightAnimation(float deltaTime);
    void createStepVisuals();
    void destroyStepVisuals();

    // Audio management
    void playStepAudio();
    void stopStepAudio();
    void playSuccessSound();
    void playErrorSound();
    void playTransitionSound();

    // User guidance
    void provideHint();
    void showContextualHelp();
    void adjustDifficultyBasedOnPerformance();
    void offerSkipOption();

    // Event dispatching
    void dispatchStepCompletion(const std::string& stepId, bool completed);
    void dispatchGuideCompletion();
    void dispatchUserAction(const std::string& action, const vec3& position);

private:
    // Guide configuration
    std::string m_guideTitle;
    std::string m_guideDescription;
    std::vector<GuideStep> m_steps;
    std::map<std::string, int> m_stepIndices;

    // Guide state
    bool m_isActive;
    bool m_isPaused;
    GuideProgress m_progress;
    float m_currentStepTimer;
    float m_guideSpeed;

    // Components
    std::shared_ptr<GuideCharacter> m_guideCharacter;
    std::shared_ptr<GuidePointer> m_guidePointer;
    std::shared_ptr<HolographicDisplay> m_infoPanel;
    std::shared_ptr<AudioEngine> m_audioEngine;

    // Visual effects
    std::shared_ptr<ParticleEmitter> m_highlightParticles;
    std::shared_ptr<SceneNode> m_highlightGeometry;
    vec3 m_highlightColor;
    float m_highlightIntensity;
    float m_highlightPhase;

    // Settings
    bool m_audioNarrationEnabled;
    bool m_visualHighlightsEnabled;
    bool m_userInputEnabled;
    bool m_adaptivePacingEnabled;
    bool m_contextualHelpEnabled;
    float m_userSkillLevel;
    float m_helpThreshold;
    std::string m_uiTheme;

    // Callbacks
    StepCompletionCallback m_stepCompletionCallback;
    GuideCompletionCallback m_guideCompletionCallback;
    UserActionCallback m_userActionCallback;

    // Analytics
    struct StepAnalytics {
        std::string stepId;
        uint64_t startTime;
        uint64_t endTime;
        int attempts;
        bool completed;
        std::vector<std::string> actions;
        std::vector<std::string> mistakes;
    };
    std::vector<StepAnalytics> m_stepAnalytics;
    std::map<std::string, float> m_actionSuccessRates;
    float m_totalGuideTime;

    // Adaptive features
    float m_currentDifficultyLevel;
    std::queue<float> m_recentPerformance;
    float m_helpTimer;
    bool m_hintOffered;
    bool m_skipOffered;

    // Helper methods
    void updateProgress();
    void resetStepTimer();
    GuideStep* findStepById(const std::string& stepId);
    int findStepIndexById(const std::string& stepId);
    bool arePrerequisitesMet(const GuideStep& step);
    void unlockDependentSteps(const std::string& completedStepId);
    float calculateStepScore(const StepAnalytics& analytics);
};

// ============================================================================
// GuideSequenceBuilder - Builder pattern for creating guide sequences
// ============================================================================

class GuideSequenceBuilder {
public:
    GuideSequenceBuilder(const std::string& sequenceName);

    // Basic step creation
    GuideSequenceBuilder& addIntroduction(const std::string& title, const std::string& description);
    GuideSequenceBuilder& addPointAndClick(const std::string& targetObject, const std::string& description);
    GuideSequenceBuilder& addGestureDemo(const std::string& gesture, const std::string& description);
    GuideSequenceBuilder& addInteractiveTask(const std::string& task, const std::string& description);
    GuideSequenceBuilder& addInformation(const std::string& info);
    GuideSequenceBuilder& addCelebration(const std::string& message);

    // Advanced step configuration
    GuideSequenceBuilder& setStepPosition(const vec3& position);
    GuideSequenceBuilder& setStepRadius(float radius);
    GuideSequenceBuilder& setStepDuration(float duration);
    GuideSequenceBuilder& setStepSkippable(bool skippable);
    GuideSequenceBuilder& setStepRepeatable(bool repeatable);
    GuideSequenceBuilder& setStepHighlight(const vec3& color, float intensity);
    GuideSequenceBuilder& setStepAudio(const std::string& audioFile);
    GuideSequenceBuilder& setStepCompletion(std::function<bool()> completionCheck);
    GuideSequenceBuilder& setStepCallback(std::function<void()> callback);

    // Step dependencies
    GuideSequenceBuilder& requireStep(const std::string& stepId);
    GuideSequenceBuilder& unlockStep(const std::string& stepId);

    // Conditional steps
    GuideSequenceBuilder& addConditionalStep(std::function<bool()> condition, const GuideStep& step);
    GuideSequenceBuilder& addBranchingPath(const std::string& condition, 
                                          const std::vector<GuideStep>& pathA, 
                                          const std::vector<GuideStep>& pathB);

    // Finalization
    std::vector<GuideStep> build();
    void applyToGuide(std::shared_ptr<InteractiveGuide> guide);

private:
    std::string m_sequenceName;
    std::vector<GuideStep> m_steps;
    GuideStep m_currentStep;
    int m_stepCounter;

    void finalizeCurrentStep();
    std::string generateStepId();
};

// ============================================================================
// Predefined Guide Sequences for Finalverse
// ============================================================================

class FinalverseGuideSequences {
public:
    // Basic tutorial sequences
    static std::vector<GuideStep> createWelcomeSequence();
    static std::vector<GuideStep> createNavigationTutorial();
    static std::vector<GuideStep> createServiceDiscoveryTutorial();
    static std::vector<GuideStep> createConnectionTutorial();
    static std::vector<GuideStep> createInteractionTutorial();

    // Advanced tutorial sequences
    static std::vector<GuideStep> createNexusControlTutorial();
    static std::vector<GuideStep> createServiceManagementTutorial();
    static std::vector<GuideStep> createCustomizationTutorial();
    static std::vector<GuideStep> createTroubleshootingTutorial();

    // Finalverse-specific tutorials
    static std::vector<GuideStep> createSongWeavingTutorial();
    static std::vector<GuideStep> createHarmonyTutorial();
    static std::vector<GuideStep> createEchoInteractionTutorial();
    static std::vector<GuideStep> createSilenceDetectionTutorial();

    // Contextual help sequences
    static std::vector<GuideStep> createQuickHelpSequence(const std::string& context);
    static std::vector<GuideStep> createErrorRecoverySequence(const std::string& errorType);
    static std::vector<GuideStep> createFeatureIntroduction(const std::string& featureName);

    // Adaptive sequences based on user skill level
    static std::vector<GuideStep> createAdaptiveSequence(float userSkillLevel, const std::string& topic);
    static std::vector<GuideStep> createPersonalizedSequence(const std::vector<std::string>& userInterests);

private:
    static GuideStep createBasicStep(GuideStepType type, const std::string& title, const std::string& description);
    static GuideStep createInteractionStep(const std::string& targetObject, const std::string& action, const std::string& description);
    static GuideStep createInformationStep(const std::string& title, const std::string& content);
    static void addFinalverseTheme(GuideStep& step);
};

// ============================================================================
// GuideManager - Global guide system management
// ============================================================================

class GuideManager {
public:
    static GuideManager& getInstance();

    // Guide registration
    void registerGuideSequence(const std::string& name, const std::vector<GuideStep>& steps);
    void unregisterGuideSequence(const std::string& name);
    std::vector<std::string> getAvailableSequences() const;

    // Guide execution
    std::shared_ptr<InteractiveGuide> createGuide(const std::string& sequenceName);
    void startGuide(const std::string& sequenceName, std::shared_ptr<SceneNode> parentNode);
    void stopAllGuides();
    void pauseAllGuides();
    void resumeAllGuides();

    // Global settings
    void setGlobalGuideSpeed(float speed);
    void setGlobalAudioEnabled(bool enabled);
    void setGlobalVisualsEnabled(bool enabled);
    void setDefaultCharacterType(GuideCharacter::CharacterType type);
    void setDefaultPointerStyle(GuidePointer::PointerStyle style);

    // User preferences
    void setUserSkillLevel(float level);
    void setUserPreferences(const std::map<std::string, std::string>& preferences);
    void enableAdaptiveGuidance(bool enable);
    void setAccessibilityOptions(const std::map<std::string, bool>& options);

    // Analytics
    void recordGuideCompletion(const std::string& sequenceName, const GuideProgress& progress);
    float getAverageCompletionRate(const std::string& sequenceName) const;
    std::vector<std::string> getMostDifficultSteps() const;
    void generateAnalyticsReport() const;

private:
    GuideManager() = default;
    ~GuideManager() = default;

    std::map<std::string, std::vector<GuideStep>> m_registeredSequences;
    std::vector<std::shared_ptr<InteractiveGuide>> m_activeGuides;
    
    // Global settings
    float m_globalGuideSpeed = 1.0f;
    bool m_globalAudioEnabled = true;
    bool m_globalVisualsEnabled = true;
    GuideCharacter::CharacterType m_defaultCharacterType = GuideCharacter::CharacterType::LUMI_ECHO;
    GuidePointer::PointerStyle m_defaultPointerStyle = GuidePointer::PointerStyle::BEAM;
    
    // User settings
    float m_userSkillLevel = 0.5f;
    std::map<std::string, std::string> m_userPreferences;
    bool m_adaptiveGuidanceEnabled = true;
    std::map<std::string, bool> m_accessibilityOptions;
    
    // Analytics
    struct GuideAnalytics {
        std::string sequenceName;
        uint64_t timestamp;
        GuideProgress progress;
        float userSkillLevel;
    };
    std::vector<GuideAnalytics> m_completionHistory;
    
    void cleanupFinishedGuides();
};

} // namespace FinalStorm