// ============================================================================
// File: FinalStorm/src/Scene/Scenes/WelcomeSequence.cpp
// WelcomeSequence & NebulaEnvironment Implementation
// ============================================================================

#include "Scene/Scenes/FirstScene.h"
#include "UI/UI3DPanel.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Math.h"
#include <iostream>

namespace FinalStorm {

// ============================================================================
// WelcomeSequence Implementation - First-time user experience
// ============================================================================

WelcomeSequence::WelcomeSequence() : SceneNode("Welcome Sequence") {
    m_currentPhase = Phase::FADE_IN;
    m_phaseTime = 0.0f;
    m_isComplete = false;
    m_fadeAlpha = 1.0f;
    
    createUIElements();
    createVisualEffects();
    
    std::cout << "WelcomeSequence initialized." << std::endl;
}

WelcomeSequence::~WelcomeSequence() = default;

void WelcomeSequence::start() {
    m_currentPhase = Phase::FADE_IN;
    m_phaseTime = 0.0f;
    m_isComplete = false;
    m_fadeAlpha = 1.0f;
    
    setVisible(true);
    
    // Start with all UI elements hidden
    if (m_titlePanel) m_titlePanel->setAlpha(0.0f);
    if (m_subtitlePanel) m_subtitlePanel->setAlpha(0.0f);
    if (m_instructionPanel) m_instructionPanel->setAlpha(0.0f);
    
    // Start background music fade-in
    startBackgroundMusic();
    
    std::cout << "Welcome sequence started." << std::endl;
}

void WelcomeSequence::skip() {
    m_isComplete = true;
    setVisible(false);
    
    // Stop all effects
    stopAllEffects();
    
    if (onComplete) {
        onComplete();
    }
    
    std::cout << "Welcome sequence skipped." << std::endl;
}

void WelcomeSequence::update(float deltaTime) {
    if (m_isComplete) return;
    
    SceneNode::update(deltaTime);
    updatePhase(deltaTime);
    updateVisualEffects(deltaTime);
}

void WelcomeSequence::render(RenderContext& context) {
    if (!isVisible()) return;
    
    // Render fade overlay
    renderFadeOverlay(context);
    
    // Render phase-specific effects
    renderPhaseEffects(context);
    
    SceneNode::render(context);
}

bool WelcomeSequence::isComplete() const {
    return m_isComplete;
}

// Private implementation methods

void WelcomeSequence::updatePhase(float deltaTime) {
    m_phaseTime += deltaTime;
    
    switch (m_currentPhase) {
        case Phase::FADE_IN:
            updateFadeInPhase();
            break;
            
        case Phase::TITLE_REVEAL:
            updateTitleRevealPhase();
            break;
            
        case Phase::NEXUS_FORMATION:
            updateNexusFormationPhase();
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            updateServiceIntroductionPhase();
            break;
            
        case Phase::READY:
            updateReadyPhase();
            break;
    }
}

void WelcomeSequence::updateFadeInPhase() {
    // Fade from black over 2 seconds
    m_fadeAlpha = 1.0f - clamp(m_phaseTime / 2.0f, 0.0f, 1.0f);
    
    if (m_phaseTime > 2.0f) {
        transitionToPhase(Phase::TITLE_REVEAL);
    }
}

void WelcomeSequence::updateTitleRevealPhase() {
    // Reveal title with elegant animation
    if (m_titlePanel) {
        float alpha = clamp(m_phaseTime / 1.5f, 0.0f, 1.0f);
        
        // Smooth step for elegant ease-in-out
        alpha = alpha * alpha * (3.0f - 2.0f * alpha);
        m_titlePanel->setAlpha(alpha);
        
        // Add gentle scale animation
        float scale = 0.8f + alpha * 0.2f;
        m_titlePanel->setScale(make_vec3(scale));
    }
    
    // Create title sparkle effects
    if (m_phaseTime > 1.0f && m_sparkles) {
        if (rand() % 100 < 3) { // 3% chance per frame
            m_sparkles->burst(5);
        }
    }
    
    if (m_phaseTime > 3.5f) {
        transitionToPhase(Phase::NEXUS_FORMATION);
    }
}

void WelcomeSequence::updateNexusFormationPhase() {
    // Reveal subtitle and begin nexus effects
    if (m_subtitlePanel) {
        float alpha = clamp((m_phaseTime - 0.5f) / 1.5f, 0.0f, 1.0f);
        alpha = alpha * alpha * (3.0f - 2.0f * alpha);
        m_subtitlePanel->setAlpha(alpha);
        
        // Gentle floating animation
        float floatOffset = sin(m_phaseTime * 2.0f) * 0.1f;
        vec3 basePos = make_vec3(0, 3, -10);
        m_subtitlePanel->setPosition(basePos + make_vec3(0, floatOffset, 0));
    }
    
    // Trigger nexus formation effects
    if (m_phaseTime > 1.0f && !m_nexusEffectTriggered) {
        triggerNexusFormationEffect();
        m_nexusEffectTriggered = true;
    }
    
    if (m_phaseTime > 4.0f) {
        transitionToPhase(Phase::SERVICE_INTRODUCTION);
    }
}

void WelcomeSequence::updateServiceIntroductionPhase() {
    // Show instruction panel and service introduction
    if (m_instructionPanel) {
        float alpha = clamp((m_phaseTime - 0.8f) / 1.2f, 0.0f, 1.0f);
        alpha = alpha * alpha * (3.0f - 2.0f * alpha);
        m_instructionPanel->setAlpha(alpha);
        
        // Pulsing glow effect
        float glow = 0.8f + 0.2f * sin(m_phaseTime * 3.0f);
        m_instructionPanel->setBorderIntensity(glow);
    }
    
    // Create service introduction effects
    createServiceIntroductionEffects();
    
    if (m_phaseTime > 5.0f) {
        transitionToPhase(Phase::READY);
    }
}

void WelcomeSequence::updateReadyPhase() {
    // Fade out all UI elements
    float fadeOut = 1.0f - clamp(m_phaseTime / 2.5f, 0.0f, 1.0f);
    
    if (m_titlePanel) m_titlePanel->setAlpha(fadeOut);
    if (m_subtitlePanel) m_subtitlePanel->setAlpha(fadeOut);
    if (m_instructionPanel) m_instructionPanel->setAlpha(fadeOut);
    
    // Final sparkle burst
    if (m_phaseTime > 1.0f && m_phaseTime < 1.5f && m_sparkles) {
        m_sparkles->burst(50);
    }
    
    if (m_phaseTime > 2.5f) {
        skip(); // Complete the sequence
    }
}

void WelcomeSequence::transitionToPhase(Phase newPhase) {
    std::cout << "Welcome sequence transitioning to phase: " << static_cast<int>(newPhase) << std::endl;
    
    m_currentPhase = newPhase;
    m_phaseTime = 0.0f;
    
    // Trigger phase-specific initialization
    switch (newPhase) {
        case Phase::TITLE_REVEAL:
            startTitleRevealEffects();
            break;
            
        case Phase::NEXUS_FORMATION:
            startNexusFormationEffects();
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            startServiceIntroductionEffects();
            break;
            
        case Phase::READY:
            startReadyPhaseEffects();
            break;
            
        default:
            break;
    }
}

void WelcomeSequence::createUIElements() {
    // Main title panel
    m_titlePanel = std::make_shared<UI3DPanel>(10.0f, 2.5f);
    m_titlePanel->setText("WELCOME TO THE NEXUS");
    m_titlePanel->setTextSize(2.0f);
    m_titlePanel->setTextColor(make_vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_titlePanel->setBackgroundColor(make_vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_titlePanel->setBorderColor(make_vec4(0.0f, 0.8f, 1.0f, 1.0f));
    m_titlePanel->setBorderWidth(0.05f);
    m_titlePanel->setPosition(make_vec3(0, 6, -12));
    m_titlePanel->setAlpha(0.0f);
    addChild(m_titlePanel);
    
    // Subtitle panel
    m_subtitlePanel = std::make_shared<UI3DPanel>(8.0f, 1.5f);
    m_subtitlePanel->setText("Your Gateway to the Finalverse");
    m_subtitlePanel->setTextSize(1.2f);
    m_subtitlePanel->setTextColor(make_vec4(0.8f, 0.9f, 1.0f, 1.0f));
    m_subtitlePanel->setBackgroundColor(make_vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m_subtitlePanel->setBorderColor(make_vec4(0.5f, 0.8f, 1.0f, 0.8f));
    m_subtitlePanel->setBorderWidth(0.03f);
    m_subtitlePanel->setPosition(make_vec3(0, 3, -10));
    m_subtitlePanel->setAlpha(0.0f);
    addChild(m_subtitlePanel);
    
    // Instruction panel
    m_instructionPanel = std::make_shared<UI3DPanel>(6.0f, 1.2f);
    m_instructionPanel->setText("Click any service to explore");
    m_instructionPanel->setTextSize(0.8f);
    m_instructionPanel->setTextColor(make_vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_instructionPanel->setBackgroundColor(make_vec4(0.0f, 0.0f, 0.0f, 0.6f));
    m_instructionPanel->setBorderColor(make_vec4(0.3f, 0.8f, 1.0f, 0.8f));
    m_instructionPanel->setBorderWidth(0.02f);
    m_instructionPanel->setPosition(make_vec3(0, -3, -8));
    m_instructionPanel->setAlpha(0.0f);
    addChild(m_instructionPanel);
    
    std::cout << "Welcome sequence UI elements created." << std::endl;
}

void WelcomeSequence::createVisualEffects() {
    // Sparkle particle system
    ParticleEmitter::Config sparkleConfig;
    sparkleConfig.emitShape = ParticleEmitter::Shape::BOX;
    sparkleConfig.emitSize = make_vec3(12, 8, 2);
    sparkleConfig.emitRate = 0.0f; // Manual control via bursts
    sparkleConfig.particleLifetime = 4.0f;
    sparkleConfig.startSize = 0.08f;
    sparkleConfig.endSize = 0.02f;
    sparkleConfig.startColor = make_vec4(1.0f, 1.0f, 1.0f, 1.0f);
    sparkleConfig.endColor = make_vec4(0.6f, 0.8f, 1.0f, 0.0f);
    sparkleConfig.velocity = 1.0f;
    sparkleConfig.velocityVariation = 0.5f;
    sparkleConfig.gravity = make_vec3(0, -0.2f, 0);
    sparkleConfig.turbulence = 0.3f;
    
    m_sparkles = std::make_shared<ParticleEmitter>(sparkleConfig);
    m_sparkles->setPosition(make_vec3(0, 2, -10));
    addChild(m_sparkles);
    
    // Energy flow effects
    createEnergyFlowEffects();
    
    // Ambient glow effects
    createAmbientGlowEffects();
    
    std::cout << "Welcome sequence visual effects created." << std::endl;
}

void WelcomeSequence::createEnergyFlowEffects() {
    // Energy streams flowing toward the center
    for (int i = 0; i < 4; ++i) {
        ParticleEmitter::Config energyConfig;
        energyConfig.emitShape = ParticleEmitter::Shape::LINE;
        energyConfig.emitRate = 15.0f;
        energyConfig.particleLifetime = 3.0f;
        energyConfig.startSize = 0.05f;
        energyConfig.endSize = 0.01f;
        energyConfig.startColor = make_vec4(0.2f, 0.8f, 1.0f, 0.8f);
        energyConfig.endColor = make_vec4(0.1f, 0.4f, 0.8f, 0.0f);
        energyConfig.velocity = 2.0f;
        energyConfig.inheritVelocity = 0.8f;
        
        auto energyStream = std::make_shared<ParticleEmitter>(energyConfig);
        
        // Position streams around the periphery
        float angle = (i / 4.0f) * 2.0f * M_PI;
        vec3 startPos = make_vec3(
            cos(angle) * 15.0f,
            sin(i * 0.8f) * 3.0f,
            sin(angle) * 15.0f - 10.0f
        );
        vec3 endPos = make_vec3(0, 0, -5);
        
        energyStream->setEmitLine(startPos, endPos);
        m_energyStreams.push_back(energyStream);
        addChild(energyStream);
    }
}

void WelcomeSequence::createAmbientGlowEffects() {
    // Ambient particle field
    ParticleEmitter::Config ambientConfig;
    ambientConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    ambientConfig.emitRadius = 20.0f;
    ambientConfig.emitRate = 5.0f;
    ambientConfig.particleLifetime = 15.0f;
    ambientConfig.startSize = 0.3f;
    ambientConfig.endSize = 0.5f;
    ambientConfig.startColor = make_vec4(0.1f, 0.3f, 0.6f, 0.2f);
    ambientConfig.endColor = make_vec4(0.2f, 0.5f, 0.8f, 0.0f);
    ambientConfig.velocity = 0.3f;
    ambientConfig.velocityVariation = 0.8f;
    ambientConfig.gravity = make_vec3(0, 0.05f, 0);
    
    m_ambientGlow = std::make_shared<ParticleEmitter>(ambientConfig);
    m_ambientGlow->setPosition(make_vec3(0, 0, -8));
    addChild(m_ambientGlow);
}

void WelcomeSequence::updateVisualEffects(float deltaTime) {
    // Update energy stream flow
    updateEnergyStreamFlow(deltaTime);
    
    // Update sparkle emission based on phase
    updateSparkleEmission();
    
    // Update ambient effects
    updateAmbientEffects(deltaTime);
}

void WelcomeSequence::updateEnergyStreamFlow(float deltaTime) {
    static float flowTime = 0.0f;
    flowTime += deltaTime;
    
    for (size_t i = 0; i < m_energyStreams.size(); ++i) {
        // Update stream intensity based on phase
        float intensity = 0.5f;
        if (m_currentPhase == Phase::NEXUS_FORMATION) {
            intensity = 1.0f + sin(flowTime * 3.0f + i) * 0.3f;
        }
        
        m_energyStreams[i]->setEmitRate(15.0f * intensity);
        
        // Update stream direction with slight variation
        float angle = (i / 4.0f) * 2.0f * M_PI + flowTime * 0.2f;
        vec3 startPos = make_vec3(
            cos(angle) * 15.0f,
            sin(flowTime + i * 0.8f) * 3.0f,
            sin(angle) * 15.0f - 10.0f
        );
        vec3 endPos = make_vec3(0, 0, -5);
        
        m_energyStreams[i]->setEmitLine(startPos, endPos);
    }
}

void WelcomeSequence::updateSparkleEmission() {
    if (!m_sparkles) return;
    
    switch (m_currentPhase) {
        case Phase::TITLE_REVEAL:
            // Gentle sparkles during title reveal
            if (rand() % 200 < 3) {
                m_sparkles->burst(3);
            }
            break;
            
        case Phase::NEXUS_FORMATION:
            // More intense sparkles during nexus formation
            if (rand() % 100 < 5) {
                m_sparkles->burst(8);
            }
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            // Rhythmic sparkles during service introduction
            if (static_cast<int>(m_phaseTime * 2.0f) % 2 == 0 && rand() % 150 < 4) {
                m_sparkles->burst(5);
            }
            break;
            
        default:
            break;
    }
}

void WelcomeSequence::updateAmbientEffects(float deltaTime) {
    if (!m_ambientGlow) return;
    
    // Modulate ambient intensity based on phase
    float intensity = 0.3f;
    switch (m_currentPhase) {
        case Phase::NEXUS_FORMATION:
            intensity = 0.8f + sin(m_phaseTime * 2.0f) * 0.2f;
            break;
        case Phase::SERVICE_INTRODUCTION:
            intensity = 0.6f;
            break;
        case Phase::READY:
            intensity = 0.4f;
            break;
        default:
            break;
    }
    
    m_ambientGlow->setEmitRate(5.0f * intensity);
}

void WelcomeSequence::renderFadeOverlay(RenderContext& context) {
    if (m_fadeAlpha > 0.01f) {
        context.pushTransform(mat4_identity());
        context.setColor(make_vec4(0, 0, 0, m_fadeAlpha));
        context.renderFullscreenQuad();
        context.popTransform();
    }
}

void WelcomeSequence::renderPhaseEffects(RenderContext& context) {
    switch (m_currentPhase) {
        case Phase::TITLE_REVEAL:
            renderTitleEffects(context);
            break;
            
        case Phase::NEXUS_FORMATION:
            renderNexusEffects(context);
            break;
            
        case Phase::SERVICE_INTRODUCTION:
            renderServiceEffects(context);
            break;
            
        case Phase::READY:
            renderReadyEffects(context);
            break;
            
        default:
            break;
    }
}

void WelcomeSequence::renderTitleEffects(RenderContext& context) {
    // Render title glow effects
    if (m_titlePanel && m_titlePanel->getAlpha() > 0.1f) {
        context.pushTransform(m_titlePanel->getWorldMatrix());
        
        // Soft glow behind title
        context.setColor(make_vec4(0.0f, 0.8f, 1.0f, 0.3f * m_titlePanel->getAlpha()));
        context.drawGlowQuad(12.0f, 3.5f, 2.0f);
        
        context.popTransform();
    }
}

void WelcomeSequence::renderNexusEffects(RenderContext& context) {
    // Render nexus formation visualization
    float progress = clamp(m_phaseTime / 3.0f, 0.0f, 1.0f);
    
    context.pushTransform(getWorldMatrix());
    context.translate(make_vec3(0, 0, -5));
    
    // Central energy core
    float coreIntensity = progress * (0.8f + 0.2f * sin(m_phaseTime * 4.0f));
    context.setColor(make_vec4(0.2f, 0.9f, 1.0f, coreIntensity));
    context.drawGlowSphere(0.5f + progress * 1.0f);
    
    // Energy rings
    for (int i = 0; i < 3; ++i) {
        float ringProgress = clamp((progress - i * 0.2f) / 0.8f, 0.0f, 1.0f);
        if (ringProgress > 0.0f) {
            float radius = (1.0f + i * 0.5f) * ringProgress;
            context.setColor(make_vec4(0.1f, 0.7f, 1.0f, 0.6f * ringProgress));
            context.drawWireframeCircle(radius, 32);
        }
    }
    
    context.popTransform();
}

void WelcomeSequence::renderServiceEffects(RenderContext& context) {
    // Render service introduction hints
    float progress = clamp(m_phaseTime / 4.0f, 0.0f, 1.0f);
    
    // Service position indicators
    for (int i = 0; i < 6; ++i) {
        float serviceProgress = clamp((progress - i * 0.1f) / 0.6f, 0.0f, 1.0f);
        if (serviceProgress > 0.0f) {
            float angle = (i / 6.0f) * 2.0f * M_PI;
            vec3 pos = make_vec3(
                cos(angle) * 8.0f,
                sin(i * 0.7f) * 2.0f,
                sin(angle) * 8.0f - 5.0f
            );
            
            context.pushTransform(getWorldMatrix());
            context.translate(pos);
            
            float alpha = serviceProgress * (0.6f + 0.4f * sin(m_phaseTime * 2.0f + i));
            context.setColor(make_vec4(0.4f, 0.8f, 1.0f, alpha));
            context.drawGlowSphere(0.3f);
            
            context.popTransform();
        }
    }
}

void WelcomeSequence::renderReadyEffects(RenderContext& context) {
    // Render final transition effects
    float progress = clamp(m_phaseTime / 2.0f, 0.0f, 1.0f);
    float intensity = (1.0f - progress) * sin(m_phaseTime * 8.0f);
    
    if (intensity > 0.0f) {
        context.pushTransform(getWorldMatrix());
        context.setColor(make_vec4(1.0f, 1.0f, 1.0f, intensity * 0.3f));
        context.renderFullscreenFlash();
        context.popTransform();
    }
}

// Phase-specific effect triggers

void WelcomeSequence::startTitleRevealEffects() {
    // Begin gentle sparkle emission
    if (m_sparkles) {
        m_sparkles->burst(10);
    }
}

void WelcomeSequence::startNexusFormationEffects() {
    // Increase energy stream intensity
    for (auto& stream : m_energyStreams) {
        stream->setEmitRate(25.0f);
    }
    
    // Trigger formation sparkles
    if (m_sparkles) {
        m_sparkles->burst(20);
    }
    
    m_nexusEffectTriggered = false; // Reset for this phase
}

void WelcomeSequence::startServiceIntroductionEffects() {
    // Create service preview effects
    createServicePreviewEffects();
}

void WelcomeSequence::startReadyPhaseEffects() {
    // Final burst of effects
    if (m_sparkles) {
        m_sparkles->burst(30);
    }
    
    // Reduce energy stream intensity
    for (auto& stream : m_energyStreams) {
        stream->setEmitRate(5.0f);
    }
}

void WelcomeSequence::triggerNexusFormationEffect() {
    // Create expanding formation wave
    auto formation = std::make_shared<NexusFormationEffect>();
    formation->setPosition(make_vec3(0, 0, -5));
    formation->setIntensity(0.8f);
    formation->setDuration(2.0f);
    addChild(formation);
}

void WelcomeSequence::createServiceIntroductionEffects() {
    // Create preview of service positions
    static bool effectsCreated = false;
    if (!effectsCreated && m_phaseTime > 1.0f) {
        createServicePreviewEffects();
        effectsCreated = true;
    }
}

void WelcomeSequence::createServicePreviewEffects() {
    // Create subtle indicators where services will appear
    for (int i = 0; i < 8; ++i) {
        auto preview = std::make_shared<ServicePreviewEffect>();
        
        float angle = (i / 8.0f) * 2.0f * M_PI;
        vec3 pos = make_vec3(
            cos(angle) * 10.0f,
            sin(i * 0.6f) * 2.0f,
            sin(angle) * 10.0f - 3.0f
        );
        
        preview->setPosition(pos);
        preview->setColor(make_vec3(0.4f, 0.7f, 1.0f));
        preview->setDelay(i * 0.2f);
        preview->setDuration(1.5f);
        
        addChild(preview);
    }
}

void WelcomeSequence::startBackgroundMusic() {
    // TODO: Integrate with audio system
    std::cout << "Starting welcome sequence background music." << std::endl;
}

void WelcomeSequence::stopAllEffects() {
    // Stop all particle emissions
    if (m_sparkles) {
        m_sparkles->setEmitRate(0.0f);
    }
    
    for (auto& stream : m_energyStreams) {
        stream->setEmitRate(0.0f);
    }
    
    if (m_ambientGlow) {
        m_ambientGlow->setEmitRate(0.0f);
    }
    
    std::cout << "All welcome sequence effects stopped." << std::endl;
}

// ============================================================================
// NebulaEnvironment Implementation - Dynamic background environment
// ============================================================================

NebulaEnvironment::NebulaEnvironment() : SceneNode("Nebula Environment") {
    // Initialize visual parameters
    m_primaryColor = make_vec3(0.1f, 0.2f, 0.4f);
    m_secondaryColor = make_vec3(0.4f, 0.1f, 0.3f);
    m_density = 0.7f;
    m_flowPhase = 0.0f;
    m_isActive = true;
    
    // Initialize layer speeds for different rotation rates
    m_layerSpeeds = {0.05f, -0.03f, 0.08f, -0.06f, 0.04f};
    
    createNebulaLayers();
    createStarField();
    createDustClouds();
    createAmbientParticles();
    
    std::cout << "NebulaEnvironment initialized with dynamic layers." << std::endl;
}

NebulaEnvironment::~NebulaEnvironment() = default;

void NebulaEnvironment::update(float deltaTime) {
    if (!m_isActive) return;
    
    SceneNode::update(deltaTime);
    updateFlow(deltaTime);
    updateNebulaLayers(deltaTime);
    updateStarField(deltaTime);
    updateAmbientEffects(deltaTime);
}

void NebulaEnvironment::render(RenderContext& context) {
    if (!m_isActive) return;
    
    // Render with additive blending for nebula effect
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    // Render background layers first
    renderNebulaLayers(context);
    
    SceneNode::render(context);
    
    context.popBlendMode();
}

void NebulaEnvironment::setColorScheme(const vec3& primary, const vec3& secondary) {
    m_primaryColor = primary;
    m_secondaryColor = secondary;
    
    updateLayerColors();
    updateParticleColors();
    
    std::cout << "Nebula color scheme updated." << std::endl;
}

void NebulaEnvironment::setDensity(float density) {
    m_density = clamp(density, 0.0f, 1.0f);
    
    updateLayerDensity();
    updateParticleEmissionRates();
    
    std::cout << "Nebula density set to: " << m_density << std::endl;
}

void NebulaEnvironment::addStarburst(const vec3& position, float intensity) {
    // Create a temporary starburst effect
    auto starburst = std::make_shared<StarburstEffect>();
    starburst->setPosition(position);
    starburst->setIntensity(intensity);
    starburst->setColor(lerp(m_primaryColor, m_secondaryColor, 0.5f));
    starburst->setDuration(3.0f);
    starburst->setMaxRadius(5.0f + intensity * 10.0f);
    
    addChild(starburst);
    
    std::cout << "Starburst created at position with intensity: " << intensity << std::endl;
}

void NebulaEnvironment::setActive(bool active) {
    m_isActive = active;
    
    // Update particle emission states
    updateParticleEmissionRates();
    
    std::cout << "Nebula environment " << (active ? "activated" : "deactivated") << std::endl;
}

void NebulaEnvironment::triggerDisturbance(const vec3& center, float radius, float intensity) {
    // Create expanding disturbance wave
    auto disturbance = std::make_shared<NebulaDisturbance>();
    disturbance->setCenter(center);
    disturbance->setRadius(radius);
    disturbance->setIntensity(intensity);
    disturbance->setDuration(4.0f);
    disturbance->setColor(m_secondaryColor);
    
    addChild(disturbance);
    
    // Affect nearby particle systems
    affectNearbyParticles(center, radius, intensity);
}

// Private implementation methods

void NebulaEnvironment::createNebulaLayers() {
    // Create multiple nebula cloud layers at different scales
    for (int i = 0; i < 5; ++i) {
        auto layer = std::make_shared<NebulaLayer>();
        
        float scale = 80.0f + i * 30.0f;
        layer->setScale(make_vec3(scale));
        layer->setAlpha(0.08f * m_density * (1.0f - i * 0.15f));
        
        // Position layers for depth variation
        vec3 offset = make_vec3(
            sin(i * 1.3f) * 15.0f,
            cos(i * 0.9f) * 8.0f,
            i * -8.0f
        );
        layer->setPosition(offset);
        
        // Set unique rotation axis for each layer
        vec3 rotationAxis = normalize(make_vec3(
            sin(i * 0.7f),
            cos(i * 1.1f),
            sin(i * 1.5f)
        ));
        layer->setRotationAxis(rotationAxis);
        layer->setRotationSpeed(m_layerSpeeds[i]);
        
        // Create layer material
        auto layerMaterial = std::make_shared<NebulaMaterial>();
        layerMaterial->setPrimaryColor(m_primaryColor);
        layerMaterial->setSecondaryColor(m_secondaryColor);
        layerMaterial->setDensity(m_density);
        layerMaterial->setLayerIndex(i);
        layer->setMaterial(layerMaterial);
        
        m_nebulaLayers.push_back(layer);
        addChild(layer);
    }
    
    std::cout << "Created " << m_nebulaLayers.size() << " nebula layers." << std::endl;
}

void NebulaEnvironment::createStarField() {
    // Create background star field
    ParticleEmitter::Config starConfig;
    starConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    starConfig.emitRadius = 150.0f;
    starConfig.emitRate = 0.0f; // Static stars, emit once
    starConfig.particleLifetime = 1000.0f; // Very long lifetime
    starConfig.startSize = 0.02f;
    starConfig.endSize = 0.02f;
    starConfig.startColor = make_vec4(1.0f, 0.95f, 0.9f, 1.0f);
    starConfig.endColor = make_vec4(1.0f, 0.95f, 0.9f, 1.0f);
    starConfig.velocity = 0.0f; // Static stars
    starConfig.sizeVariation = 0.5f;
    starConfig.colorVariation = 0.2f;
    
    m_stars = std::make_shared<ParticleEmitter>(starConfig);
    m_stars->setName("Star Field");
    
    // Emit initial star batch
    m_stars->burst(800); // Create 800 stars
    
    addChild(m_stars);
    
    // Create special bright stars
    createBrightStars();
    
    std::cout << "Star field created with bright star accents." << std::endl;
}

void NebulaEnvironment::createBrightStars() {
    // Create a few bright, twinkling stars
    for (int i = 0; i < 20; ++i) {
        auto brightStar = std::make_shared<BrightStar>();
        
        // Random position on sphere
        float theta = (rand() / float(RAND_MAX)) * 2.0f * M_PI;
        float phi = acos(1.0f - 2.0f * (rand() / float(RAND_MAX)));
        float radius = 120.0f + (rand() / float(RAND_MAX)) * 60.0f;
        
        vec3 position = make_vec3(
            sin(phi) * cos(theta) * radius,
            cos(phi) * radius,
            sin(phi) * sin(theta) * radius
        );
        
        brightStar->setPosition(position);
        brightStar->setIntensity(0.6f + (rand() / float(RAND_MAX)) * 0.4f);
        brightStar->setTwinkleSpeed(1.0f + (rand() / float(RAND_MAX)) * 2.0f);
        brightStar->setColor(generateStarColor());
        
        m_brightStars.push_back(brightStar);
        addChild(brightStar);
    }
}

vec3 NebulaEnvironment::generateStarColor() {
    // Generate realistic star colors based on temperature
    float temp = rand() / float(RAND_MAX);
    
    if (temp < 0.3f) {
        // Cool red stars
        return make_vec3(1.0f, 0.7f, 0.5f);
    } else if (temp < 0.6f) {
        // Sun-like yellow stars
        return make_vec3(1.0f, 0.95f, 0.8f);
    } else if (temp < 0.8f) {
        // White stars
        return make_vec3(1.0f, 1.0f, 1.0f);
    } else {
        // Hot blue stars
        return make_vec3(0.8f, 0.9f, 1.0f);
    }
}

void NebulaEnvironment::createDustClouds() {
    // Create floating dust particle systems
    ParticleEmitter::Config dustConfig;
    dustConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    dustConfig.emitRadius = 60.0f;
    dustConfig.emitRate = 15.0f;
    dustConfig.particleLifetime = 25.0f;
    dustConfig.startSize = 0.8f;
    dustConfig.endSize = 1.2f;
    dustConfig.startColor = make_vec4(m_primaryColor.x, m_primaryColor.y, m_primaryColor.z, 0.08f);
    dustConfig.endColor = make_vec4(m_secondaryColor.x, m_secondaryColor.y, m_secondaryColor.z, 0.02f);
    dustConfig.velocity = 0.3f;
    dustConfig.velocityVariation = 0.8f;
    dustConfig.gravity = make_vec3(0, 0.02f, 0);
    dustConfig.turbulence = 0.1f;
    
    m_dust = std::make_shared<ParticleEmitter>(dustConfig);
    m_dust->setName("Dust Clouds");
    addChild(m_dust);
    
    // Create additional dust layers
    createDustLayers();
    
    std::cout << "Dust cloud systems created." << std::endl;
}

void NebulaEnvironment::createDustLayers() {
    // Create multiple dust layers with different properties
    for (int i = 0; i < 3; ++i) {
        ParticleEmitter::Config layerConfig;
        layerConfig.emitShape = ParticleEmitter::Shape::BOX;
        layerConfig.emitSize = make_vec3(100.0f + i * 20.0f, 40.0f, 100.0f + i * 20.0f);
        layerConfig.emitRate = 8.0f - i * 2.0f;
        layerConfig.particleLifetime = 30.0f + i * 10.0f;
        layerConfig.startSize = 1.0f + i * 0.3f;
        layerConfig.endSize = 1.5f + i * 0.4f;
        
        float t = i / 3.0f;
        vec3 layerColor = lerp(m_primaryColor, m_secondaryColor, t);
        layerConfig.startColor = make_vec4(layerColor.x, layerColor.y, layerColor.z, 0.06f);
        layerConfig.endColor = make_vec4(layerColor.x * 0.5f, layerColor.y * 0.5f, layerColor.z * 0.5f, 0.01f);
        
        layerConfig.velocity = 0.2f + i * 0.1f;
        layerConfig.velocityVariation = 0.6f;
        layerConfig.gravity = make_vec3(0, 0.01f + i * 0.005f, 0);
        
        auto dustLayer = std::make_shared<ParticleEmitter>(layerConfig);
        dustLayer->setName("Dust Layer " + std::to_string(i));
        dustLayer->setPosition(make_vec3(0, -5.0f * i, 0));
        
        m_dustLayers.push_back(dustLayer);
        addChild(dustLayer);
    }
}

void NebulaEnvironment::createAmbientParticles() {
    // Create ambient energy motes
    ParticleEmitter::Config moteConfig;
    moteConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    moteConfig.emitRadius = 40.0f;
    moteConfig.emitRate = 5.0f;
    moteConfig.particleLifetime = 20.0f;
    moteConfig.startSize = 0.15f;
    moteConfig.endSize = 0.05f;
    moteConfig.startColor = make_vec4(0.6f, 0.8f, 1.0f, 0.6f);
    moteConfig.endColor = make_vec4(0.3f, 0.5f, 0.8f, 0.0f);
    moteConfig.velocity = 0.8f;
    moteConfig.velocityVariation = 0.4f;
    moteConfig.gravity = make_vec3(0, 0.1f, 0);
    moteConfig.turbulence = 0.3f;
    
    m_ambientMotes = std::make_shared<ParticleEmitter>(moteConfig);
    m_ambientMotes->setName("Ambient Energy Motes");
    addChild(m_ambientMotes);
    
    std::cout << "Ambient particle systems created." << std::endl;
}

void NebulaEnvironment::updateFlow(float deltaTime) {
    m_flowPhase += deltaTime * 0.1f;
    
    // Update nebula layer flow
    updateNebulaFlow(deltaTime);
    
    // Update particle system flow
    updateParticleFlow(deltaTime);
}

void NebulaEnvironment::updateNebulaFlow(float deltaTime) {
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        auto& layer = m_nebulaLayers[i];
        
        // Rotate each layer at its specific speed
        quat rotation = simd_quaternion(
            m_layerSpeeds[i] * deltaTime,
            layer->getRotationAxis()
        );
        layer->rotate(rotation);
        
        // Subtle scale pulsing for organic feel
        float scalePulse = 1.0f + sin(m_flowPhase * 0.8f + i * 0.6f) * 0.02f;
        vec3 baseScale = make_vec3(80.0f + i * 30.0f);
        layer->setScale(baseScale * scalePulse);
        
        // Update layer opacity based on flow
        float opacityMod = 1.0f + sin(m_flowPhase * 1.2f + i * 0.8f) * 0.3f;
        float baseOpacity = 0.08f * m_density * (1.0f - i * 0.15f);
        layer->setAlpha(baseOpacity * opacityMod);
    }
}

void NebulaEnvironment::updateParticleFlow(float deltaTime) {
    // Create gentle currents in particle systems
    vec3 currentDirection = make_vec3(
        sin(m_flowPhase * 0.7f),
        cos(m_flowPhase * 0.5f) * 0.3f,
        cos(m_flowPhase * 0.9f)
    );
    
    // Apply flow to dust systems
    if (m_dust) {
        vec3 flowForce = currentDirection * 0.1f;
        m_dust->setWindForce(flowForce);
    }
    
    for (auto& dustLayer : m_dustLayers) {
        vec3 layerFlow = currentDirection * (0.05f + dustLayer->getLayerIndex() * 0.02f);
        dustLayer->setWindForce(layerFlow);
    }
    
    // Apply flow to ambient motes
    if (m_ambientMotes) {
        vec3 moteFlow = currentDirection * 0.15f;
        m_ambientMotes->setWindForce(moteFlow);
    }
}

void NebulaEnvironment::updateNebulaLayers(float deltaTime) {
    // Update layer-specific effects
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        auto& layer = m_nebulaLayers[i];
        
        // Update material animation
        if (auto material = layer->getMaterial()) {
            material->updateAnimation(deltaTime, m_flowPhase);
        }
        
        // Update layer distortions
        updateLayerDistortion(layer.get(), i, deltaTime);
    }
}

void NebulaEnvironment::updateLayerDistortion(NebulaLayer* layer, int index, float deltaTime) {
    if (!layer) return;
    
    // Create flowing distortion effects
    float distortionPhase = m_flowPhase + index * 0.5f;
    
    // UV offset for flowing motion
    vec2 uvOffset = make_vec2(
        sin(distortionPhase * 0.3f) * 0.1f,
        cos(distortionPhase * 0.2f) * 0.05f
    );
    layer->setUVOffset(uvOffset);
    
    // Distortion strength variation
    float distortionStrength = 0.5f + sin(distortionPhase * 0.8f) * 0.3f;
    layer->setDistortionStrength(distortionStrength);
}

void NebulaEnvironment::updateStarField(float deltaTime) {
    // Update bright star twinkling
    for (auto& star : m_brightStars) {
        star->updateTwinkling(deltaTime);
    }
    
    // Occasionally create shooting stars
    static float shootingStarTimer = 0.0f;
    shootingStarTimer += deltaTime;
    
    if (shootingStarTimer > 15.0f && rand() % 100 < 2) { // 2% chance every 15 seconds
        createShootingStar();
        shootingStarTimer = 0.0f;
    }
}

void NebulaEnvironment::createShootingStar() {
    auto shootingStar = std::make_shared<ShootingStar>();
    
    // Random start position on sphere edge
    float theta = (rand() / float(RAND_MAX)) * 2.0f * M_PI;
    float phi = (rand() / float(RAND_MAX)) * M_PI;
    
    vec3 startPos = make_vec3(
        sin(phi) * cos(theta) * 100.0f,
        cos(phi) * 100.0f,
        sin(phi) * sin(theta) * 100.0f
    );
    
    // Random direction across sky
    vec3 direction = normalize(make_vec3(
        (rand() / float(RAND_MAX) - 0.5f) * 2.0f,
        (rand() / float(RAND_MAX) - 0.5f) * 0.5f,
        (rand() / float(RAND_MAX) - 0.5f) * 2.0f
    ));
    
    shootingStar->setStartPosition(startPos);
    shootingStar->setDirection(direction);
    shootingStar->setSpeed(50.0f + (rand() / float(RAND_MAX)) * 30.0f);
    shootingStar->setLifetime(2.0f + (rand() / float(RAND_MAX)) * 1.0f);
    shootingStar->setColor(generateStarColor());
    
    addChild(shootingStar);
    
    std::cout << "Shooting star created." << std::endl;
}

void NebulaEnvironment::updateAmbientEffects(float deltaTime) {
    // Update ambient particle emission based on activity level
    float baseEmission = m_isActive ? 1.0f : 0.1f;
    float activityMod = 1.0f + sin(m_flowPhase * 1.5f) * 0.3f;
    
    if (m_ambientMotes) {
        m_ambientMotes->setEmitRate(5.0f * baseEmission * activityMod);
    }
    
    // Update ambient lighting
    updateAmbientLighting(deltaTime);
}

void NebulaEnvironment::updateAmbientLighting(float deltaTime) {
    // Create subtle ambient light variation
    static float lightPhase = 0.0f;
    lightPhase += deltaTime * 0.2f;
    
    float lightIntensity = 0.3f + sin(lightPhase) * 0.1f;
    vec3 lightColor = lerp(m_primaryColor, m_secondaryColor, 0.5f + sin(lightPhase * 0.7f) * 0.3f);
    
    // Apply to environment lighting
    setAmbientLight(lightColor * lightIntensity);
}

// Update helper methods

void NebulaEnvironment::updateLayerColors() {
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        auto& layer = m_nebulaLayers[i];
        if (auto material = layer->getMaterial()) {
            material->setPrimaryColor(m_primaryColor);
            material->setSecondaryColor(m_secondaryColor);
        }
    }
}

void NebulaEnvironment::updateParticleColors() {
    // Update dust cloud colors
    if (m_dust) {
        vec4 dustColor = make_vec4(m_primaryColor.x, m_primaryColor.y, m_primaryColor.z, 0.08f);
        m_dust->setParticleColor(dustColor);
    }
    
    for (auto& dustLayer : m_dustLayers) {
        float t = dustLayer->getLayerIndex() / 3.0f;
        vec3 layerColor = lerp(m_primaryColor, m_secondaryColor, t);
        vec4 dustColor = make_vec4(layerColor.x, layerColor.y, layerColor.z, 0.06f);
        dustLayer->setParticleColor(dustColor);
    }
}

void NebulaEnvironment::updateLayerDensity() {
    for (size_t i = 0; i < m_nebulaLayers.size(); ++i) {
        auto& layer = m_nebulaLayers[i];
        float baseAlpha = 0.08f * (1.0f - i * 0.15f);
        layer->setAlpha(baseAlpha * m_density);
        
        if (auto material = layer->getMaterial()) {
            material->setDensity(m_density);
        }
    }
}

void NebulaEnvironment::updateParticleEmissionRates() {
    float emissionMod = m_isActive ? m_density : 0.1f;
    
    if (m_dust) {
        m_dust->setEmitRate(15.0f * emissionMod);
    }
    
    for (size_t i = 0; i < m_dustLayers.size(); ++i) {
        float layerRate = (8.0f - i * 2.0f) * emissionMod;
        m_dustLayers[i]->setEmitRate(layerRate);
    }
    
    if (m_ambientMotes) {
        m_ambientMotes->setEmitRate(5.0f * emissionMod);
    }
}

void NebulaEnvironment::affectNearbyParticles(const vec3& center, float radius, float intensity) {
    // Apply disturbance force to nearby particle systems
    vec3 repulsionForce = make_vec3(0, intensity * 2.0f, 0);
    
    if (m_dust) {
        float distance = length(m_dust->getPosition() - center);
        if (distance < radius) {
            float strength = (1.0f - distance / radius) * intensity;
            m_dust->addForce(repulsionForce * strength);
        }
    }
    
    // Affect dust layers
    for (auto& dustLayer : m_dustLayers) {
        float distance = length(dustLayer->getPosition() - center);
        if (distance < radius) {
            float strength = (1.0f - distance / radius) * intensity;
            dustLayer->addForce(repulsionForce * strength);
        }
    }
}

void NebulaEnvironment::renderNebulaLayers(RenderContext& context) {
    // Render background nebula layers with special blending
    for (auto& layer : m_nebulaLayers) {
        context.pushBlendMode(BlendMode::SOFT_ADDITIVE);
        layer->render(context);
        context.popBlendMode();
    }
}

void NebulaEnvironment::setAmbientLight(const vec3& color) {
    // Set ambient lighting for the environment
    // This would integrate with the lighting system
    std::cout << "Ambient light updated." << std::endl;
}

} // namespace FinalStorm