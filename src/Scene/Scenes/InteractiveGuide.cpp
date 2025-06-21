// ============================================================================
// File: FinalStorm/src/Scene/Scenes/InteractiveGuide.cpp
// InteractiveGuide & ServiceConnectionVisualizer Implementation
// ============================================================================

#include "Scene/Scenes/FirstScene.h"
#include "UI/UI3DPanel.h"
#include "Services/ServiceEntity.h"
#include "Services/Components/ParticleEmitter.h"
#include "Services/Components/ConnectionBeam.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Math.h"
#include <iostream>
#include <algorithm>

namespace FinalStorm {

// ============================================================================
// InteractiveGuide Implementation - Helpful floating companion
// ============================================================================

InteractiveGuide::InteractiveGuide() : SceneNode("Interactive Guide") {
    // Initialize state
    m_isVisible = true;
    m_bobPhase = 0.0f;
    m_textDisplayTime = 0.0f;
    m_textDuration = 0.0f;
    m_moveTime = 0.0f;
    m_moveDuration = 0.0f;
    m_currentState = GuideState::IDLE;
    m_attentionLevel = 0.5f;
    
    // Movement parameters
    m_targetPosition = vec3_zero();
    m_homePosition = make_vec3(5, 5, 5);
    m_orbitCenter = vec3_zero();
    m_orbitRadius = 0.0f;
    m_orbitSpeed = 0.0f;
    m_orbitPhase = 0.0f;
    
    createGuideVisual();
    createTextPanel();
    createAuraEffects();
    
    std::cout << "InteractiveGuide initialized." << std::endl;
}

InteractiveGuide::~InteractiveGuide() = default;

void InteractiveGuide::showTip(const std::string& text, float duration) {
    if (m_textPanel) {
        m_textPanel->setText(text);
        m_textPanel->setVisible(true);
        m_textDisplayTime = 0.0f;
        m_textDuration = duration;
        
        // Animate panel entrance
        animateTextPanelIn();
        
        // Get guide's attention
        setAttentionLevel(0.9f);
        
        std::cout << "Guide showing tip: " << text << std::endl;
    }
}

void InteractiveGuide::showServiceInfo(ServiceEntity* service) {
    if (!service) return;
    
    std::string info = service->getName() + "\n";
    info += "Type: " + serviceTypeToString(service->getServiceType()) + "\n";
    info += "Status: " + std::string(service->isConnected() ? "Online" : "Offline") + "\n";
    
    if (service->isConnected()) {
        auto metrics = service->getMetrics();
        if (!metrics.empty()) {
            info += "CPU: " + std::to_string(static_cast<int>(metrics["cpu_usage"])) + "%\n";
            info += "Memory: " + std::to_string(static_cast<int>(metrics["memory_usage"])) + "%";
        }
    }
    
    showTip(info, 8.0f);
    
    // Move guide closer to service
    if (service) {
        vec3 servicePos = service->getWorldPosition();
        vec3 guidePos = servicePos + make_vec3(2, 1, 1);
        moveToPosition(guidePos, 1.2f);
    }
}

void InteractiveGuide::hide() {
    m_isVisible = false;
    setVisible(false);
    
    if (m_textPanel) {
        m_textPanel->setVisible(false);
    }
    
    // Fade out aura
    if (m_aura) {
        m_aura->setEmitRate(0.0f);
    }
    
    std::cout << "Guide hidden." << std::endl;
}

void InteractiveGuide::show() {
    m_isVisible = true;
    setVisible(true);
    
    // Restore aura
    if (m_aura) {
        m_aura->setEmitRate(15.0f);
    }
    
    std::cout << "Guide shown." << std::endl;
}

void InteractiveGuide::update(float deltaTime) {
    if (!m_isVisible) return;
    
    SceneNode::update(deltaTime);
    
    updateState(deltaTime);
    updateMovement(deltaTime);
    updateBobbing(deltaTime);
    updateTextDisplay(deltaTime);
    updateVisualEffects(deltaTime);
    updateLookDirection(deltaTime);
}

void InteractiveGuide::render(RenderContext& context) {
    if (!m_isVisible) return;
    
    // Apply attention-based glow
    context.pushMaterial();
    
    float glowIntensity = 0.5f + m_attentionLevel * 0.5f;
    context.setEmissionIntensity(glowIntensity);
    
    SceneNode::render(context);
    
    context.popMaterial();
}

void InteractiveGuide::moveToPosition(const vec3& target, float duration) {
    m_targetPosition = target;
    m_moveTime = 0.0f;
    m_moveDuration = duration;
    m_currentState = GuideState::MOVING;
    
    // Create movement trail effect
    createMovementTrail();
    
    std::cout << "Guide moving to new position over " << duration << " seconds." << std::endl;
}

void InteractiveGuide::orbitAround(const vec3& center, float radius, float speed) {
    m_orbitCenter = center;
    m_orbitRadius = radius;
    m_orbitSpeed = speed;
    m_orbitPhase = 0.0f;
    m_currentState = GuideState::ORBITING;
    
    std::cout << "Guide beginning orbit around center with radius: " << radius << std::endl;
}

void InteractiveGuide::returnHome(float duration) {
    moveToPosition(m_homePosition, duration);
    m_currentState = GuideState::RETURNING_HOME;
}

void InteractiveGuide::setAttentionLevel(float level) {
    m_attentionLevel = clamp(level, 0.0f, 1.0f);
    
    // Update visual intensity based on attention
    updateAttentionEffects();
}

void InteractiveGuide::pointToLocation(const vec3& location, float duration) {
    m_pointingTarget = location;
    m_pointingDuration = duration;
    m_pointingTime = 0.0f;
    m_currentState = GuideState::POINTING;
    
    // Create pointing effect
    createPointingEffect(location);
    
    std::cout << "Guide pointing to location." << std::endl;
}

void InteractiveGuide::celebrateAchievement() {
    m_currentState = GuideState::CELEBRATING;
    m_celebrationTime = 0.0f;
    
    // Burst celebration particles
    if (m_aura) {
        m_aura->burst(30);
    }
    
    // Create celebration effects
    createCelebrationEffects();
    
    std::cout << "Guide celebrating achievement!" << std::endl;
}

// Private implementation methods

void InteractiveGuide::createGuideVisual() {
    // Central core - icosahedral crystal
    m_core = std::make_shared<MeshNode>("Guide Core");
    m_core->setMesh("icosahedron");
    m_core->setScale(make_vec3(0.25f));
    
    // Create core material
    auto coreMaterial = std::make_shared<Material>("guide_core");
    coreMaterial->setAlbedo(make_vec3(0.2f, 0.6f, 0.9f));
    coreMaterial->setMetallic(0.1f);
    coreMaterial->setRoughness(0.2f);
    coreMaterial->setEmission(make_vec3(0.3f, 0.7f, 1.0f));
    coreMaterial->setEmissionStrength(1.5f);
    m_core->setMaterial(coreMaterial);
    
    addChild(m_core);
    
    // Orbiting elements - smaller crystals
    createOrbitingElements();
    
    // Energy connectors
    createEnergyConnectors();
    
    std::cout << "Guide visual elements created." << std::endl;
}

void InteractiveGuide::createOrbitingElements() {
    // Create 3 small orbiting crystals
    for (int i = 0; i < 3; ++i) {
        auto element = std::make_shared<OrbitingElement>();
        
        element->setMesh("octahedron");
        element->setScale(make_vec3(0.08f));
        
        // Set orbit parameters
        float orbitRadius = 0.6f + i * 0.1f;
        float orbitSpeed = 2.0f + i * 0.5f;
        float orbitOffset = (i / 3.0f) * 2.0f * M_PI;
        
        element->setOrbitRadius(orbitRadius);
        element->setOrbitSpeed(orbitSpeed);
        element->setOrbitOffset(orbitOffset);
        element->setOrbitAxis(make_vec3(0, 1, 0));
        
        // Material
        auto elementMaterial = std::make_shared<Material>("guide_element");
        elementMaterial->setAlbedo(make_vec3(0.4f, 0.8f, 1.0f));
        elementMaterial->setEmission(make_vec3(0.2f, 0.6f, 0.9f));
        elementMaterial->setEmissionStrength(0.8f);
        element->setMaterial(elementMaterial);
        
        m_orbitingElements.push_back(element);
        addChild(element);
    }
}

void InteractiveGuide::createEnergyConnectors() {
    // Create energy connections between core and orbiting elements
    for (auto& element : m_orbitingElements) {
        auto connector = std::make_shared<EnergyConnector>();
        connector->setStartNode(m_core.get());
        connector->setEndNode(element.get());
        connector->setColor(make_vec3(0.4f, 0.8f, 1.0f));
        connector->setIntensity(0.3f);
        connector->setFlowSpeed(3.0f);
        
        m_energyConnectors.push_back(connector);
        addChild(connector);
    }
}

void InteractiveGuide::createTextPanel() {
    // Create floating text panel
    m_textPanel = std::make_shared<UI3DPanel>(4.0f, 1.5f);
    m_textPanel->setText("");
    m_textPanel->setTextSize(0.6f);
    m_textPanel->setTextColor(make_vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_textPanel->setBackgroundColor(make_vec4(0.1f, 0.2f, 0.4f, 0.8f));
    m_textPanel->setBorderColor(make_vec4(0.4f, 0.8f, 1.0f, 1.0f));
    m_textPanel->setBorderWidth(0.02f);
    m_textPanel->setPosition(make_vec3(0, 0.8f, 0));
    m_textPanel->setVisible(false);
    
    // Add subtle glow effect
    m_textPanel->setGlowColor(make_vec3(0.4f, 0.8f, 1.0f));
    m_textPanel->setGlowIntensity(0.3f);
    
    addChild(m_textPanel);
}

void InteractiveGuide::createAuraEffects() {
    // Main aura particle system
    ParticleEmitter::Config auraConfig;
    auraConfig.emitShape = ParticleEmitter::Shape::SPHERE;
    auraConfig.emitRadius = 0.4f;
    auraConfig.emitRate = 15.0f;
    auraConfig.particleLifetime = 2.0f;
    auraConfig.startSize = 0.03f;
    auraConfig.endSize = 0.08f;
    auraConfig.startColor = make_vec4(0.5f, 0.8f, 1.0f, 0.8f);
    auraConfig.endColor = make_vec4(0.2f, 0.6f, 0.9f, 0.0f);
    auraConfig.velocity = 0.5f;
    auraConfig.velocityVariation = 0.3f;
    auraConfig.gravity = make_vec3(0, 0.1f, 0);
    
    m_aura = std::make_shared<ParticleEmitter>(auraConfig);
    m_aura->setName("Guide Aura");
    addChild(m_aura);
    
    // Attention sparkles
    createAttentionSparkles();
    
    std::cout << "Guide aura effects created." << std::endl;
}

void InteractiveGuide::createAttentionSparkles() {
    // Sparkles that appear when guide has high attention
    ParticleEmitter::Config sparkleConfig;
    sparkleConfig.emitShape = ParticleEmitter::Shape::POINT;
    sparkleConfig.emitRate = 0.0f; // Manual control
    sparkleConfig.particleLifetime = 1.5f;
    sparkleConfig.startSize = 0.05f;
    sparkleConfig.endSize = 0.01f;
    sparkleConfig.startColor = make_vec4(1.0f, 1.0f, 1.0f, 1.0f);
    sparkleConfig.endColor = make_vec4(0.8f, 0.9f, 1.0f, 0.0f);
    sparkleConfig.velocity = 1.2f;
    sparkleConfig.velocityVariation = 0.8f;
    
    m_attentionSparkles = std::make_shared<ParticleEmitter>(sparkleConfig);
    m_attentionSparkles->setName("Attention Sparkles");
    addChild(m_attentionSparkles);
}

void InteractiveGuide::updateState(float deltaTime) {
    switch (m_currentState) {
        case GuideState::IDLE:
            updateIdleState(deltaTime);
            break;
            
        case GuideState::MOVING:
            updateMovingState(deltaTime);
            break;
            
        case GuideState::ORBITING:
            updateOrbitingState(deltaTime);
            break;
            
        case GuideState::POINTING:
            updatePointingState(deltaTime);
            break;
            
        case GuideState::CELEBRATING:
            updateCelebratingState(deltaTime);
            break;
            
        case GuideState::RETURNING_HOME:
            updateReturningHomeState(deltaTime);
            break;
    }
}

void InteractiveGuide::updateIdleState(float deltaTime) {
    // Gentle floating motion when idle
    static float idleTime = 0.0f;
    idleTime += deltaTime;
    
    // Occasional attention-seeking behavior
    if (idleTime > 10.0f && rand() % 1000 < 2) {
        setAttentionLevel(0.8f);
        if (m_attentionSparkles) {
            m_attentionSparkles->burst(5);
        }
        idleTime = 0.0f;
    }
    
    // Gradual attention decay
    m_attentionLevel = lerp(m_attentionLevel, 0.3f, deltaTime * 0.5f);
}

void InteractiveGuide::updateMovingState(float deltaTime) {
    // Check if movement is complete
    if (m_moveTime >= m_moveDuration) {
        m_currentState = GuideState::IDLE;
        setAttentionLevel(0.4f);
    }
}

void InteractiveGuide::updateOrbitingState(float deltaTime) {
    m_orbitPhase += deltaTime * m_orbitSpeed;
    
    // Calculate orbital position
    vec3 orbitPos = m_orbitCenter + make_vec3(
        cos(m_orbitPhase) * m_orbitRadius,
        sin(m_orbitPhase * 0.5f) * (m_orbitRadius * 0.3f),
        sin(m_orbitPhase) * m_orbitRadius
    );
    
    setPosition(orbitPos);
    
    // Maintain higher attention during orbit
    setAttentionLevel(0.7f);
}

void InteractiveGuide::updatePointingState(float deltaTime) {
    m_pointingTime += deltaTime;
    
    if (m_pointingTime >= m_pointingDuration) {
        m_currentState = GuideState::IDLE;
        setAttentionLevel(0.4f);
    } else {
        // Maintain high attention while pointing
        setAttentionLevel(0.9f);
        
        // Create periodic pointing effects
        if (static_cast<int>(m_pointingTime * 3.0f) % 2 == 0) {
            createPointingPulse();
        }
    }
}

void InteractiveGuide::updateCelebratingState(float deltaTime) {
    m_celebrationTime += deltaTime;
    
    if (m_celebrationTime >= 3.0f) {
        m_currentState = GuideState::IDLE;
        setAttentionLevel(0.6f);
    } else {
        // High energy celebration
        setAttentionLevel(1.0f);
        
        // Celebration effects
        if (m_attentionSparkles && rand() % 100 < 5) {
            m_attentionSparkles->burst(3);
        }
        
        // Bouncy movement
        float bouncePhase = m_celebrationTime * 8.0f;
        vec3 currentPos = getPosition();
        currentPos.y += sin(bouncePhase) * 0.2f * deltaTime;
        setPosition(currentPos);
    }
}

void InteractiveGuide::updateReturningHomeState(float deltaTime) {
    // Same as moving, but with different final state
    if (m_moveTime >= m_moveDuration) {
        m_currentState = GuideState::IDLE;
        setAttentionLevel(0.3f);
        std::cout << "Guide returned home." << std::endl;
    }
}

void InteractiveGuide::updateMovement(float deltaTime) {
    if (m_currentState == GuideState::MOVING || m_currentState == GuideState::RETURNING_HOME) {
        if (m_moveTime < m_moveDuration) {
            m_moveTime += deltaTime;
            
            // Smooth interpolation with easeInOutCubic
            float t = m_moveTime / m_moveDuration;
            t = t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            
            vec3 startPos = getPosition();
            vec3 newPos = lerp(startPos, m_targetPosition, deltaTime * 5.0f); // Smooth following
            setPosition(newPos);
        }
    }
}

void InteractiveGuide::updateBobbing(float deltaTime) {
    m_bobPhase += deltaTime * 1.5f;
    
    // Subtle bobbing motion
    float bobIntensity = 0.08f + m_attentionLevel * 0.05f;
    vec3 currentPos = getPosition();
    
    // Use a combination of sine waves for natural motion
    float bobOffset = sin(m_bobPhase) * bobIntensity + 
                     sin(m_bobPhase * 1.3f + 0.5f) * bobIntensity * 0.3f;
    
    // Apply bobbing only if not in specific movement states
    if (m_currentState != GuideState::ORBITING) {
        currentPos.y += bobOffset * deltaTime * 2.0f;
        setPosition(currentPos);
    }
}

void InteractiveGuide::updateTextDisplay(float deltaTime) {
    if (m_textPanel && m_textPanel->isVisible()) {
        m_textDisplayTime += deltaTime;
        
        // Fade out text panel near end
        if (m_textDisplayTime > m_textDuration - 1.0f) {
            float fadeTime = m_textDuration - m_textDisplayTime;
            float alpha = clamp(fadeTime, 0.0f, 1.0f);
            m_textPanel->setAlpha(alpha);
        }
        
        // Hide text panel when duration expires
        if (m_textDisplayTime >= m_textDuration) {
            m_textPanel->setVisible(false);
            setAttentionLevel(0.4f);
        }
    }
}

void InteractiveGuide::updateVisualEffects(float deltaTime) {
    // Update core rotation
    if (m_core) {
        quat rotation = simd_quaternion(deltaTime * 0.8f, make_vec3(0, 1, 0));
        m_core->rotate(rotation);
    }
    
    // Update orbiting elements
    for (auto& element : m_orbitingElements) {
        element->updateOrbit(deltaTime);
    }
    
    // Update energy connectors
    for (auto& connector : m_energyConnectors) {
        connector->updateFlow(deltaTime);
    }
    
    // Update aura intensity based on attention
    if (m_aura) {
        float auraRate = 10.0f + m_attentionLevel * 15.0f;
        m_aura->setEmitRate(auraRate);
        
        // Color intensity
        vec4 auraColor = make_vec4(0.5f, 0.8f, 1.0f, 0.6f + m_attentionLevel * 0.4f);
        m_aura->setParticleColor(auraColor);
    }
}

void InteractiveGuide::updateLookDirection(float deltaTime) {
    // Make guide face interesting directions based on state
    vec3 lookTarget = getPosition() + make_vec3(0, 0, 1); // Default forward
    
    switch (m_currentState) {
        case GuideState::POINTING:
            lookTarget = m_pointingTarget;
            break;
            
        case GuideState::MOVING:
        case GuideState::RETURNING_HOME:
            lookTarget = m_targetPosition;
            break;
            
        case GuideState::ORBITING:
            lookTarget = m_orbitCenter;
            break;
            
        default:
            // Look around occasionally when idle
            static float lookTime = 0.0f;
            lookTime += deltaTime;
            if (lookTime > 5.0f) {
                lookTarget = getPosition() + make_vec3(
                    sin(lookTime * 0.3f) * 3.0f,
                    cos(lookTime * 0.2f) * 1.0f,
                    cos(lookTime * 0.4f) * 3.0f
                );
                if (lookTime > 8.0f) lookTime = 0.0f;
            }
            break;
    }
    
    // Smooth rotation toward look target
    vec3 lookDir = normalize(lookTarget - getPosition());
    quat targetRotation = quatLookAt(lookDir, make_vec3(0, 1, 0));
    quat currentRotation = getRotation();
    quat newRotation = slerp(currentRotation, targetRotation, deltaTime * 2.0f);
    setRotation(newRotation);
}

void InteractiveGuide::updateAttentionEffects() {
    // Update visual effects based on attention level
    if (m_attentionLevel > 0.7f && m_attentionSparkles) {
        // High attention - create sparkles
        if (rand() % 200 < static_cast<int>(m_attentionLevel * 100)) {
            m_attentionSparkles->burst(2);
        }
    }
    
    // Update core glow
    if (m_core && m_core->getMaterial()) {
        float glowIntensity = 1.0f + m_attentionLevel * 0.8f;
        auto material = m_core->getMaterial();
        material->setEmissionStrength(glowIntensity);
    }
}

// Effect creation methods

void InteractiveGuide::createMovementTrail() {
    // Create trail effect during movement
    auto trail = std::make_shared<MovementTrail>();
    trail->setStartPosition(getPosition());
    trail->setColor(make_vec3(0.4f, 0.8f, 1.0f));
    trail->setLifetime(1.5f);
    trail->setWidth(0.1f);
    
    // The trail will automatically follow the guide's movement
    addChild(trail);
}

void InteractiveGuide::createPointingEffect(const vec3& location) {
    // Create pointing beam or indicator
    auto pointing = std::make_shared<PointingEffect>();
    pointing->setStartPosition(getPosition());
    pointing->setTargetPosition(location);
    pointing->setColor(make_vec3(1.0f, 0.8f, 0.2f));
    pointing->setIntensity(0.8f);
    pointing->setDuration(m_pointingDuration);
    
    addChild(pointing);
}

void InteractiveGuide::createPointingPulse() {
    // Create pulse effect toward pointing target
    auto pulse = std::make_shared<PointingPulse>();
    pulse->setPosition(getPosition());
    pulse->setDirection(normalize(m_pointingTarget - getPosition()));
    pulse->setColor(make_vec3(1.0f, 0.8f, 0.2f));
    pulse->setSpeed(8.0f);
    pulse->setLifetime(0.8f);
    
    addChild(pulse);
}

void InteractiveGuide::createCelebrationEffects() {
    // Create firework-like celebration effects
    for (int i = 0; i < 5; ++i) {
        auto firework = std::make_shared<CelebrationFirework>();
        
        vec3 offset = make_vec3(
            (rand() / float(RAND_MAX) - 0.5f) * 2.0f,
            (rand() / float(RAND_MAX)) * 1.5f + 0.5f,
            (rand() / float(RAND_MAX) - 0.5f) * 2.0f
        );
        
        firework->setPosition(getPosition() + offset);
        firework->setColor(make_vec3(
            0.5f + (rand() / float(RAND_MAX)) * 0.5f,
            0.5f + (rand() / float(RAND_MAX)) * 0.5f,
            0.8f + (rand() / float(RAND_MAX)) * 0.2f
        ));
        firework->setIntensity(0.8f);
        firework->setDelay(i * 0.3f);
        
        addChild(firework);
    }
}

void InteractiveGuide::animateTextPanelIn() {
    if (!m_textPanel) return;
    
    // Start small and grow
    m_textPanel->setScale(make_vec3(0.5f));
    m_textPanel->setAlpha(0.0f);
    
    // Create animation
    auto animation = std::make_shared<TextPanelAnimation>();
    animation->setTarget(m_textPanel.get());
    animation->setTargetScale(make_vec3(1.0f));
    animation->setTargetAlpha(1.0f);
    animation->setDuration(0.5f);
    animation->setEasing(AnimationEasing::EASE_OUT_BACK);
    
    addChild(animation);
}

std::string InteractiveGuide::serviceTypeToString(ServiceEntity::ServiceType type) {
    switch (type) {
        case ServiceEntity::ServiceType::API_GATEWAY: return "API Gateway";
        case ServiceEntity::ServiceType::AI_ORCHESTRA: return "AI Orchestra";
        case ServiceEntity::ServiceType::SONG_ENGINE: return "Song Engine";
        case ServiceEntity::ServiceType::ECHO_ENGINE: return "Echo Engine";
        case ServiceEntity::ServiceType::WORLD_ENGINE: return "World Engine";
        case ServiceEntity::ServiceType::HARMONY_SERVICE: return "Harmony Service";
        case ServiceEntity::ServiceType::ASSET_SERVICE: return "Asset Service";
        case ServiceEntity::ServiceType::COMMUNITY: return "Community";
        case ServiceEntity::ServiceType::SILENCE_SERVICE: return "Silence Service";
        case ServiceEntity::ServiceType::PROCEDURAL_GEN: return "Procedural Generator";
        case ServiceEntity::ServiceType::BEHAVIOR_AI: return "Behavior AI";
        default: return "Unknown Service";
    }
}

// ============================================================================
// ServiceConnectionVisualizer Implementation - Connection visualization system
// ============================================================================

ServiceConnectionVisualizer::ServiceConnectionVisualizer() 
    : SceneNode("Service Connections") {
    
    m_maxConnections = 50;
    m_connectionIntensity = 0.6f;
    m_showDataFlow = true;
    m_pulseSpeed = 2.0f;
    
    createDataFlowSystem();
    createConnectionPool();
    
    std::cout << "ServiceConnectionVisualizer initialized." << std::endl;
}

ServiceConnectionVisualizer::~ServiceConnectionVisualizer() = default;

void ServiceConnectionVisualizer::addConnection(ServiceEntity* from, ServiceEntity* to, float strength) {
    if (!from || !to || from == to) return;
    
    // Check if connection already exists
    auto existing = findConnection(from, to);
    if (existing != m_connections.end()) {
        existing->strength = strength;
        existing->lastActivity = 0.0f;
        std::cout << "Updated existing connection strength: " << strength << std::endl;
        return;
    }
    
    // Check connection limit
    if (m_connections.size() >= m_maxConnections) {
        removeOldestConnection();
    }
    
    Connection conn;
    conn.from = from;
    conn.to = to;
    conn.strength = clamp(strength, 0.1f, 1.0f);
    conn.pulsePhase = 0.0f;
    conn.lastActivity = 0.0f;
    conn.isActive = true;
    conn.connectionId = generateConnectionId();
    
    createConnectionVisual(conn);
    m_connections.push_back(conn);
    
    std::cout << "Added connection: " << from->getName() << " -> " << to->getName() 
              << " (strength: " << strength << ")" << std::endl;
}

void ServiceConnectionVisualizer::removeConnection(ServiceEntity* from, ServiceEntity* to) {
    auto it = findConnection(from, to);
    if (it != m_connections.end()) {
        // Animate connection removal
        animateConnectionRemoval(*it);
        
        // Remove visual
        if (it->visual) {
            removeChild(it->visual);
        }
        
        std::cout << "Removed connection: " << from->getName() << " -> " << to->getName() << std::endl;
        m_connections.erase(it);
    }
}

void ServiceConnectionVisualizer::clearConnections() {
    for (auto& conn : m_connections) {
        if (conn.visual) {
            removeChild(conn.visual);
        }
    }
    m_connections.clear();
    
    std::cout << "Cleared all connections." << std::endl;
}

void ServiceConnectionVisualizer::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateConnectionVisuals(deltaTime);
    updateDataFlow(deltaTime);
    updateConnectionActivity(deltaTime);
    cleanupInactiveConnections(deltaTime);
}

void ServiceConnectionVisualizer::render(RenderContext& context) {
    if (m_connections.empty()) return;
    
    // Render with additive blending for energy effects
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    SceneNode::render(context);
    
    context.popBlendMode();
}

void ServiceConnectionVisualizer::pulseConnection(ServiceEntity* from, ServiceEntity* to) {
    auto it = findConnection(from, to);
    if (it != m_connections.end()) {
        it->pulsePhase = 0.0f;
        it->lastActivity = 0.0f;
        
        // Create pulse effect
        createConnectionPulse(*it);
        
        // Emit data flow particles
        if (m_showDataFlow) {
            emitDataFlowParticles(*it);
        }
        
        std::cout << "Pulsed connection: " << from->getName() << " -> " << to->getName() << std::endl;
    }
}

void ServiceConnectionVisualizer::pulseAllConnections(float intensity) {
    for (auto& conn : m_connections) {
        conn.pulsePhase = 0.0f;
        conn.lastActivity = 0.0f;
        
        if (intensity > 0.5f) {
            createConnectionPulse(conn);
        }
    }
    
    std::cout << "Pulsed all connections with intensity: " << intensity << std::endl;
}

void ServiceConnectionVisualizer::setConnectionIntensity(float intensity) {
    m_connectionIntensity = clamp(intensity, 0.0f, 1.0f);
    
    // Update all connection visuals
    for (auto& conn : m_connections) {
        updateConnectionVisualIntensity(conn);
    }
}

void ServiceConnectionVisualizer::setShowDataFlow(bool show) {
    m_showDataFlow = show;
    
    if (m_dataFlow) {
        m_dataFlow->setEmitRate(show ? 10.0f : 0.0f);
    }
}

void ServiceConnectionVisualizer::highlightConnection(ServiceEntity* from, ServiceEntity* to, float duration) {
    auto it = findConnection(from, to);
    if (it != m_connections.end()) {
        // Create highlight effect
        auto highlight = std::make_shared<ConnectionHighlight>();
        highlight->setConnection(&(*it));
        highlight->setDuration(duration);
        highlight->setIntensity(1.5f);
        highlight->setColor(make_vec3(1.0f, 0.8f, 0.2f));
        
        addChild(highlight);
        
        std::cout << "Highlighted connection for " << duration << " seconds." << std::endl;
    }
}

void ServiceConnectionVisualizer::showConnectionInfo(ServiceEntity* from, ServiceEntity* to) {
    auto it = findConnection(from, to);
    if (it != m_connections.end()) {
        // Create info display
        auto infoDisplay = std::make_shared<ConnectionInfoDisplay>();
        infoDisplay->setConnection(&(*it));
        infoDisplay->setDuration(5.0f);
        
        // Position between the two services
        vec3 midPoint = (from->getWorldPosition() + to->getWorldPosition()) * 0.5f;
        infoDisplay->setPosition(midPoint + make_vec3(0, 1, 0));
        
        addChild(infoDisplay);
    }
}

// Private implementation methods

std::vector<ServiceConnectionVisualizer::Connection>::iterator 
ServiceConnectionVisualizer::findConnection(ServiceEntity* from, ServiceEntity* to) {
    return std::find_if(m_connections.begin(), m_connections.end(),
        [from, to](const Connection& conn) {
            return (conn.from == from && conn.to == to) ||
                   (conn.from == to && conn.to == from); // Bidirectional
        });
}

void ServiceConnectionVisualizer::createConnectionVisual(Connection& conn) {
    // Create beam visual
    auto beam = std::make_shared<ConnectionBeam>();
    beam->setStartPosition(conn.from->getWorldPosition());
    beam->setEndPosition(conn.to->getWorldPosition());
    beam->setColor(calculateConnectionColor(conn));
    beam->setIntensity(conn.strength * m_connectionIntensity);
    beam->setThickness(0.02f + conn.strength * 0.03f);
    beam->setFlowSpeed(2.0f);
    beam->setConnectionId(conn.connectionId);
    
    conn.visual = beam;
    addChild(beam);
}

vec3 ServiceConnectionVisualizer::calculateConnectionColor(const Connection& conn) {
    // Blend colors based on service types and connection strength
    vec3 fromColor = conn.from->getBaseColor();
    vec3 toColor = conn.to->getBaseColor();
    
    // Blend colors
    vec3 baseColor = lerp(fromColor, toColor, 0.5f);
    
    // Adjust based on connection strength
    float intensity = 0.6f + conn.strength * 0.4f;
    
    return baseColor * intensity;
}

void ServiceConnectionVisualizer::updateConnectionVisuals(float deltaTime) {
    for (auto& conn : m_connections) {
        if (!conn.visual || !conn.from || !conn.to) continue;
        
        // Update connection beam position
        vec3 startPos = conn.from->getWorldPosition();
        vec3 endPos = conn.to->getWorldPosition();
        
        conn.visual->setStartPosition(startPos);
        conn.visual->setEndPosition(endPos);
        
        // Update pulse animation
        conn.pulsePhase += deltaTime * m_pulseSpeed;
        
        // Calculate activity-based intensity
        float activityDecay = exp(-conn.lastActivity * 0.5f); // Exponential decay
        float pulseIntensity = 0.5f + 0.5f * sin(conn.pulsePhase) * activityDecay;
        float finalIntensity = conn.strength * m_connectionIntensity * pulseIntensity;
        
        conn.visual->setIntensity(finalIntensity);
        
        // Update color based on activity
        vec3 connectionColor = calculateConnectionColor(conn);
        if (conn.lastActivity < 2.0f) {
            // Recent activity - brighter color
            connectionColor = connectionColor * (1.0f + activityDecay * 0.5f);
        }
        conn.visual->setColor(connectionColor);
        
        // Update flow direction based on last data direction
        updateConnectionFlow(conn, deltaTime);
    }
}

void ServiceConnectionVisualizer::updateConnectionFlow(Connection& conn, float deltaTime) {
    if (!conn.visual) return;
    
    // Simulate data flow direction based on service types
    bool flowForward = shouldFlowForward(conn.from, conn.to);
    conn.visual->setFlowDirection(flowForward ? 1.0f : -1.0f);
    
    // Update flow speed based on activity
    float baseSpeed = 2.0f;
    float activitySpeed = (3.0f - conn.lastActivity) * 0.5f;
    float flowSpeed = baseSpeed + clamp(activitySpeed, 0.0f, 3.0f);
    conn.visual->setFlowSpeed(flowSpeed);
}

bool ServiceConnectionVisualizer::shouldFlowForward(ServiceEntity* from, ServiceEntity* to) {
    // Determine data flow direction based on service types
    auto fromType = from->getServiceType();
    auto toType = to->getServiceType();
    
    // API Gateway typically sends to other services
    if (fromType == ServiceEntity::ServiceType::API_GATEWAY) return true;
    if (toType == ServiceEntity::ServiceType::API_GATEWAY) return false;
    
    // Data flows toward storage and processing services
    if (toType == ServiceEntity::ServiceType::ASSET_SERVICE) return true;
    if (fromType == ServiceEntity::ServiceType::ASSET_SERVICE) return false;
    
    // AI services typically process data from other services
    if (toType == ServiceEntity::ServiceType::AI_ORCHESTRA ||
        toType == ServiceEntity::ServiceType::BEHAVIOR_AI) return true;
    
    // Default: flow from lower to higher service type enum value
    return static_cast<int>(fromType) < static_cast<int>(toType);
}

void ServiceConnectionVisualizer::updateDataFlow(float deltaTime) {
    if (!m_showDataFlow || !m_dataFlow) return;
    
    // Update data flow particle system
    static float dataFlowTimer = 0.0f;
    dataFlowTimer += deltaTime;
    
    // Emit data particles along active connections
    if (dataFlowTimer > 0.5f) {
        for (const auto& conn : m_connections) {
            if (conn.isActive && conn.lastActivity < 3.0f) {
                emitDataFlowParticles(conn);
            }
        }
        dataFlowTimer = 0.0f;
    }
    
    // Update data flow intensity based on overall activity
    float overallActivity = calculateOverallActivity();
    m_dataFlow->setEmitRate(10.0f * overallActivity);
}

void ServiceConnectionVisualizer::updateConnectionActivity(float deltaTime) {
    for (auto& conn : m_connections) {
        conn.lastActivity += deltaTime;
        
        // Mark connection as inactive if no activity for too long
        if (conn.lastActivity > 15.0f) {
            conn.isActive = false;
        }
        
        // Reduce visual intensity for inactive connections
        if (!conn.isActive && conn.visual) {
            float inactiveIntensity = conn.strength * m_connectionIntensity * 0.2f;
            conn.visual->setIntensity(inactiveIntensity);
        }
    }
}

void ServiceConnectionVisualizer::cleanupInactiveConnections(float deltaTime) {
    static float cleanupTimer = 0.0f;
    cleanupTimer += deltaTime;
    
    if (cleanupTimer > 30.0f) { // Cleanup every 30 seconds
        auto it = std::remove_if(m_connections.begin(), m_connections.end(),
            [](const Connection& conn) {
                return !conn.isActive && conn.lastActivity > 60.0f; // Remove after 1 minute of inactivity
            });
        
        if (it != m_connections.end()) {
            // Remove visuals for cleaned up connections
            for (auto connIt = it; connIt != m_connections.end(); ++connIt) {
                if (connIt->visual) {
                    removeChild(connIt->visual);
                }
            }
            
            size_t removedCount = std::distance(it, m_connections.end());
            m_connections.erase(it, m_connections.end());
            
            std::cout << "Cleaned up " << removedCount << " inactive connections." << std::endl;
        }
        
        cleanupTimer = 0.0f;
    }
}

void ServiceConnectionVisualizer::createDataFlowSystem() {
    // Main data flow particle system
    ParticleEmitter::Config dataConfig;
    dataConfig.emitShape = ParticleEmitter::Shape::LINE;
    dataConfig.emitRate = 10.0f;
    dataConfig.particleLifetime = 2.0f;
    dataConfig.startSize = 0.02f;
    dataConfig.endSize = 0.005f;
    dataConfig.startColor = make_vec4(0.4f, 0.8f, 1.0f, 0.8f);
    dataConfig.endColor = make_vec4(0.2f, 0.6f, 0.9f, 0.0f);
    dataConfig.velocity = 5.0f;
    dataConfig.velocityVariation = 0.2f;
    dataConfig.inheritVelocity = 0.9f;
    
    m_dataFlow = std::make_shared<ParticleEmitter>(dataConfig);
    m_dataFlow->setName("Data Flow");
    addChild(m_dataFlow);
    
    std::cout << "Data flow system created." << std::endl;
}

void ServiceConnectionVisualizer::createConnectionPool() {
    // Pre-create connection visuals for better performance
    m_connectionPool.reserve(m_maxConnections);
    
    for (size_t i = 0; i < m_maxConnections / 2; ++i) {
        auto beam = std::make_shared<ConnectionBeam>();
        beam->setVisible(false);
        m_connectionPool.push_back(beam);
    }
    
    std::cout << "Connection pool created with " << m_connectionPool.size() << " pre-allocated beams." << std::endl;
}

void ServiceConnectionVisualizer::removeOldestConnection() {
    if (m_connections.empty()) return;
    
    // Find connection with oldest last activity
    auto oldest = std::max_element(m_connections.begin(), m_connections.end(),
        [](const Connection& a, const Connection& b) {
            return a.lastActivity < b.lastActivity;
        });
    
    if (oldest != m_connections.end()) {
        if (oldest->visual) {
            removeChild(oldest->visual);
        }
        m_connections.erase(oldest);
        std::cout << "Removed oldest connection to make room for new one." << std::endl;
    }
}

uint32_t ServiceConnectionVisualizer::generateConnectionId() {
    static uint32_t nextId = 1;
    return nextId++;
}

void ServiceConnectionVisualizer::updateConnectionVisualIntensity(Connection& conn) {
    if (conn.visual) {
        float intensity = conn.strength * m_connectionIntensity;
        conn.visual->setIntensity(intensity);
    }
}

float ServiceConnectionVisualizer::calculateOverallActivity() {
    if (m_connections.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    int activeCount = 0;
    
    for (const auto& conn : m_connections) {
        if (conn.isActive) {
            float activity = exp(-conn.lastActivity * 0.2f); // Exponential decay
            totalActivity += activity;
            activeCount++;
        }
    }
    
    return activeCount > 0 ? totalActivity / activeCount : 0.0f;
}

void ServiceConnectionVisualizer::createConnectionPulse(const Connection& conn) {
    // Create expanding pulse at connection midpoint
    vec3 startPos = conn.from->getWorldPosition();
    vec3 endPos = conn.to->getWorldPosition();
    vec3 midPoint = (startPos + endPos) * 0.5f;
    
    auto pulse = std::make_shared<ConnectionPulse>();
    pulse->setPosition(midPoint);
    pulse->setColor(calculateConnectionColor(conn));
    pulse->setMaxRadius(1.5f + conn.strength);
    pulse->setDuration(1.0f);
    pulse->setIntensity(conn.strength);
    
    addChild(pulse);
}

void ServiceConnectionVisualizer::emitDataFlowParticles(const Connection& conn) {
    if (!m_dataFlow) return;
    
    vec3 startPos = conn.from->getWorldPosition();
    vec3 endPos = conn.to->getWorldPosition();
    
    // Set particle emission along connection line
    m_dataFlow->setEmitLine(startPos, endPos);
    
    // Emit particles based on connection strength
    int particleCount = static_cast<int>(3 + conn.strength * 7);
    m_dataFlow->burst(particleCount);
    
    // Set particle color based on connection
    vec4 particleColor = make_vec4(
        calculateConnectionColor(conn).x,
        calculateConnectionColor(conn).y, 
        calculateConnectionColor(conn).z,
        0.8f
    );
    m_dataFlow->setParticleColor(particleColor);
}

void ServiceConnectionVisualizer::animateConnectionRemoval(const Connection& conn) {
    if (!conn.visual) return;
    
    // Create fade-out animation
    auto fadeOut = std::make_shared<ConnectionFadeOut>();
    fadeOut->setTarget(conn.visual.get());
    fadeOut->setDuration(0.8f);
    fadeOut->setFadeType(ConnectionFadeOut::Type::DISSOLVE);
    
    addChild(fadeOut);
}

// Connection analysis methods

std::vector<ServiceEntity*> ServiceConnectionVisualizer::getConnectedServices(ServiceEntity* service) {
    std::vector<ServiceEntity*> connected;
    
    for (const auto& conn : m_connections) {
        if (conn.from == service && conn.isActive) {
            connected.push_back(conn.to);
        } else if (conn.to == service && conn.isActive) {
            connected.push_back(conn.from);
        }
    }
    
    return connected;
}

float ServiceConnectionVisualizer::getConnectionStrength(ServiceEntity* from, ServiceEntity* to) {
    auto it = findConnection(from, to);
    return (it != m_connections.end()) ? it->strength : 0.0f;
}

size_t ServiceConnectionVisualizer::getActiveConnectionCount() {
    return std::count_if(m_connections.begin(), m_connections.end(),
        [](const Connection& conn) { return conn.isActive; });
}

ServiceEntity* ServiceConnectionVisualizer::getMostConnectedService() {
    std::map<ServiceEntity*, int> connectionCounts;
    
    for (const auto& conn : m_connections) {
        if (conn.isActive) {
            connectionCounts[conn.from]++;
            connectionCounts[conn.to]++;
        }
    }
    
    if (connectionCounts.empty()) return nullptr;
    
    auto most = std::max_element(connectionCounts.begin(), connectionCounts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return most->first;
}

void ServiceConnectionVisualizer::createNetworkVisualization() {
    // Create overall network health visualization
    auto networkViz = std::make_shared<NetworkVisualization>();
    networkViz->setConnections(m_connections);
    networkViz->setUpdateInterval(2.0f);
    networkViz->setPosition(make_vec3(0, 8, 0));
    
    addChild(networkViz);
    
    std::cout << "Network visualization created." << std::endl;
}

void ServiceConnectionVisualizer::showNetworkStatistics() {
    size_t totalConnections = m_connections.size();
    size_t activeConnections = getActiveConnectionCount();
    float averageStrength = 0.0f;
    
    if (!m_connections.empty()) {
        float totalStrength = 0.0f;
        for (const auto& conn : m_connections) {
            totalStrength += conn.strength;
        }
        averageStrength = totalStrength / m_connections.size();
    }
    
    std::cout << "=== Network Statistics ===" << std::endl;
    std::cout << "Total Connections: " << totalConnections << std::endl;
    std::cout << "Active Connections: " << activeConnections << std::endl;
    std::cout << "Average Strength: " << averageStrength << std::endl;
    std::cout << "Overall Activity: " << calculateOverallActivity() << std::endl;
    
    ServiceEntity* mostConnected = getMostConnectedService();
    if (mostConnected) {
        std::cout << "Most Connected Service: " << mostConnected->getName() << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

} // namespace FinalStorm