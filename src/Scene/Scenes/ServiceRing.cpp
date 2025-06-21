// ============================================================================
// File: FinalStorm/src/Scene/Scenes/ServiceRing.cpp
// ServiceRing Implementation - Orbital Service Arrangement System
// ============================================================================

#include "Scene/Scenes/FirstScene.h"
#include "Services/ServiceEntity.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Math.h"
#include "Services/Components/ParticleEmitter.h"
#include "Services/Components/ConnectionBeam.h"
#include "Rendering/RenderContext.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace FinalStorm {

// ============================================================================
// ServiceRing Implementation - Intelligent orbital service positioning
// ============================================================================

ServiceRing::ServiceRing() : SceneNode("Service Ring") {
    // Initialize ring parameters
    m_radius = 8.0f;
    m_targetRadius = 8.0f;
    m_verticalSpread = 2.5f;
    m_rotationSpeed = 0.08f;
    m_rotationOffset = 0.0f;
    m_expansionTime = 0.0f;
    m_expansionDuration = 0.0f;
    m_highlightedService = nullptr;
    m_focusedService = nullptr;
    
    // Animation state
    m_isExpanding = false;
    m_isRotatingToFocus = false;
    m_focusRotationTime = 0.0f;
    m_focusRotationDuration = 1.5f;
    m_targetFocusAngle = 0.0f;
    
    // Visual parameters
    m_ringGlowIntensity = 0.5f;
    m_serviceSpacing = 1.2f; // Minimum spacing factor
    m_heightVariation = 0.8f;
    
    // Create ring visualization
    createRingVisualization();
    createOrbitTrails();
    
    std::cout << "ServiceRing initialized with orbital positioning system." << std::endl;
}

ServiceRing::~ServiceRing() = default;

void ServiceRing::addService(std::shared_ptr<ServiceEntity> service) {
    if (!service) return;
    
    // Check if service already exists
    auto it = std::find(m_services.begin(), m_services.end(), service);
    if (it != m_services.end()) {
        std::cout << "Service already in ring: " << service->getName() << std::endl;
        return;
    }
    
    m_services.push_back(service);
    addChild(service);
    
    // Calculate initial position
    updateServicePositions(0.0f);
    
    // Animate service entry
    animateServiceEntry(service.get());
    
    // Update orbit trails
    updateOrbitTrails();
    
    std::cout << "Added service to ring: " << service->getName() 
              << " (Total: " << m_services.size() << ")" << std::endl;
}

void ServiceRing::removeService(ServiceEntity* service) {
    if (!service) return;
    
    auto it = std::find_if(m_services.begin(), m_services.end(),
        [service](const std::shared_ptr<ServiceEntity>& s) {
            return s.get() == service;
        });
    
    if (it != m_services.end()) {
        // Animate service exit
        animateServiceExit(service);
        
        removeChild(*it);
        m_services.erase(it);
        
        // Reposition remaining services
        updateServicePositions(0.0f);
        updateOrbitTrails();
        
        // Clear focus if this was the focused service
        if (m_focusedService == service) {
            m_focusedService = nullptr;
        }
        if (m_highlightedService == service) {
            m_highlightedService = nullptr;
        }
        
        std::cout << "Removed service from ring: " << service->getName() 
                  << " (Remaining: " << m_services.size() << ")" << std::endl;
    }
}

void ServiceRing::clear() {
    // Animate all services exiting
    for (auto& service : m_services) {
        animateServiceExit(service.get());
        removeChild(service);
    }
    
    m_services.clear();
    m_highlightedService = nullptr;
    m_focusedService = nullptr;
    
    updateOrbitTrails();
    
    std::cout << "Cleared all services from ring." << std::endl;
}

void ServiceRing::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateRingExpansion(deltaTime);
    updateRingRotation(deltaTime);
    updateServicePositions(deltaTime);
    updateServiceAnimations(deltaTime);
    updateFocusTransition(deltaTime);
    updateVisualEffects(deltaTime);
}

void ServiceRing::render(RenderContext& context) {
    // Render ring guides and effects
    renderRingGuides(context);
    renderOrbitTrails(context);
    
    // Render services with special effects
    SceneNode::render(context);
    
    // Render focus indicators
    renderFocusIndicators(context);
}

void ServiceRing::expandRing(float targetRadius, float duration) {
    if (targetRadius < 2.0f) targetRadius = 2.0f; // Minimum radius
    if (targetRadius > 50.0f) targetRadius = 50.0f; // Maximum radius
    
    m_targetRadius = targetRadius;
    m_expansionTime = 0.0f;
    m_expansionDuration = duration;
    m_isExpanding = true;
    
    // Create expansion wave effect
    createExpansionWave();
    
    std::cout << "Expanding ring to radius: " << targetRadius 
              << " over " << duration << " seconds" << std::endl;
}

void ServiceRing::contractRing(float targetRadius, float duration) {
    expandRing(targetRadius, duration); // Use same mechanism
}

void ServiceRing::highlightService(ServiceEntity* service) {
    // Clear previous highlight
    if (m_highlightedService && m_highlightedService != service) {
        m_highlightedService->setHighlight(false);
        m_highlightedService->setGlowIntensity(0.5f);
    }
    
    m_highlightedService = service;
    
    if (service) {
        service->setHighlight(true);
        service->setGlowIntensity(0.9f);
        
        // Create highlight effects
        createHighlightEffects(service);
        
        // Dim other services slightly
        for (auto& s : m_services) {
            if (s.get() != service) {
                s->setAlpha(0.7f);
            } else {
                s->setAlpha(1.0f);
            }
        }
        
        std::cout << "Highlighted service: " << service->getName() << std::endl;
    } else {
        // Restore all services to normal
        for (auto& s : m_services) {
            s->setAlpha(1.0f);
        }
    }
}

void ServiceRing::rotateToService(ServiceEntity* service, float duration) {
    if (!service) return;
    
    // Find service index
    auto it = std::find_if(m_services.begin(), m_services.end(),
        [service](const std::shared_ptr<ServiceEntity>& s) {
            return s.get() == service;
        });
    
    if (it == m_services.end()) {
        std::cout << "Service not found in ring: " << service->getName() << std::endl;
        return;
    }
    
    int index = std::distance(m_services.begin(), it);
    float targetAngle = -(index / float(m_services.size())) * 2.0f * M_PI;
    
    // Calculate shortest rotation path
    float currentAngle = m_rotationOffset;
    float angleDiff = targetAngle - currentAngle;
    
    // Normalize angle difference to [-π, π]
    while (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2.0f * M_PI;
    
    m_targetFocusAngle = currentAngle + angleDiff;
    m_focusRotationTime = 0.0f;
    m_focusRotationDuration = duration;
    m_isRotatingToFocus = true;
    m_focusedService = service;
    
    // Create rotation indicators
    createRotationIndicators(service);
    
    std::cout << "Rotating ring to focus on: " << service->getName() 
              << " (angle: " << degrees(targetAngle) << "°)" << std::endl;
}

void ServiceRing::setServiceSpacing(float spacing) {
    m_serviceSpacing = clamp(spacing, 0.8f, 3.0f);
    updateServicePositions(0.0f);
}

void ServiceRing::setVerticalSpread(float spread) {
    m_verticalSpread = clamp(spread, 0.0f, 5.0f);
    updateServicePositions(0.0f);
}

void ServiceRing::setRotationSpeed(float speed) {
    m_rotationSpeed = clamp(speed, -1.0f, 1.0f);
}

void ServiceRing::pauseRotation(bool pause) {
    m_rotationPaused = pause;
}

// Private implementation methods

void ServiceRing::updateRingExpansion(float deltaTime) {
    if (!m_isExpanding) return;
    
    m_expansionTime += deltaTime;
    
    if (m_expansionTime >= m_expansionDuration) {
        // Expansion complete
        m_radius = m_targetRadius;
        m_isExpanding = false;
        std::cout << "Ring expansion complete. New radius: " << m_radius << std::endl;
    } else {
        // Smooth interpolation using easeInOutCubic
        float t = m_expansionTime / m_expansionDuration;
        t = t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
        
        float startRadius = m_radius;
        m_radius = lerp(startRadius, m_targetRadius, t);
        
        // Update ring visualization scale
        updateRingVisualizationScale();
    }
}

void ServiceRing::updateRingRotation(float deltaTime) {
    if (m_rotationPaused) return;
    
    if (m_isRotatingToFocus) {
        updateFocusRotation(deltaTime);
    } else {
        // Normal continuous rotation
        m_rotationOffset += deltaTime * m_rotationSpeed;
        
        // Normalize rotation offset
        while (m_rotationOffset > 2.0f * M_PI) m_rotationOffset -= 2.0f * M_PI;
        while (m_rotationOffset < 0.0f) m_rotationOffset += 2.0f * M_PI;
    }
}

void ServiceRing::updateFocusRotation(float deltaTime) {
    m_focusRotationTime += deltaTime;
    
    if (m_focusRotationTime >= m_focusRotationDuration) {
        // Focus rotation complete
        m_rotationOffset = m_targetFocusAngle;
        m_isRotatingToFocus = false;
        
        // Normalize final angle
        while (m_rotationOffset > 2.0f * M_PI) m_rotationOffset -= 2.0f * M_PI;
        while (m_rotationOffset < 0.0f) m_rotationOffset += 2.0f * M_PI;
        
        std::cout << "Focus rotation complete." << std::endl;
    } else {
        // Smooth interpolation with easeInOutQuart
        float t = m_focusRotationTime / m_focusRotationDuration;
        t = t < 0.5f ? 8.0f * t * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
        
        float startAngle = m_rotationOffset;
        m_rotationOffset = lerp(startAngle, m_targetFocusAngle, t);
    }
}

void ServiceRing::updateServicePositions(float deltaTime) {
    int count = m_services.size();
    if (count == 0) return;
    
    for (int i = 0; i < count; ++i) {
        vec3 targetPos = calculateServicePosition(i, m_radius);
        
        // Smooth position interpolation
        auto& service = m_services[i];
        vec3 currentPos = service->getPosition();
        
        float lerpSpeed = (deltaTime > 0.0f) ? deltaTime * 8.0f : 1.0f;
        vec3 newPos = lerp(currentPos, targetPos, lerpSpeed);
        service->setPosition(newPos);
        
        // Update service orientation to face outward
        updateServiceOrientation(service.get(), newPos);
        
        // Apply floating animation
        applyFloatingAnimation(service.get(), i, deltaTime);
        
        // Update service-specific effects
        updateServiceEffects(service.get(), i, deltaTime);
    }
}

vec3 ServiceRing::calculateServicePosition(int index, float radius) {
    if (m_services.empty()) return vec3_zero();
    
    int count = m_services.size();
    
    // Calculate base angle with proper spacing
    float angleStep = (2.0f * M_PI) / count;
    float angle = index * angleStep + m_rotationOffset;
    
    // Apply service spacing adjustment
    if (m_serviceSpacing > 1.0f) {
        float spacingRadius = radius * m_serviceSpacing;
        radius = lerp(radius, spacingRadius, 0.5f);
    }
    
    // Calculate height variation using Fibonacci spiral for natural distribution
    float heightPhase = index * 2.39996f; // Golden angle in radians
    float height = sin(heightPhase) * m_verticalSpread * m_heightVariation;
    
    // Add secondary height variation for visual interest
    float secondaryHeight = sin(angle * 2.0f + heightPhase) * m_verticalSpread * 0.3f;
    height += secondaryHeight;
    
    // Apply radius variation for organic feel
    float radiusVariation = 1.0f + sin(angle * 3.0f + index * 0.5f) * 0.1f;
    float finalRadius = radius * radiusVariation;
    
    return make_vec3(
        cos(angle) * finalRadius,
        height,
        sin(angle) * finalRadius
    );
}

void ServiceRing::updateServiceOrientation(ServiceEntity* service, const vec3& position) {
    if (!service) return;
    
    // Calculate outward-facing direction
    vec3 outwardDir = normalize(make_vec3(position.x, 0, position.z));
    vec3 upDir = make_vec3(0, 1, 0);
    
    // Create rotation to face outward with slight upward tilt
    vec3 finalDir = normalize(outwardDir + upDir * 0.2f);
    quat orientation = quatLookAt(finalDir, upDir);
    
    // Apply smooth rotation interpolation
    quat currentRot = service->getRotation();
    quat newRot = slerp(currentRot, orientation, 0.1f);
    service->setRotation(newRot);
}

void ServiceRing::updateServiceAnimations(float deltaTime) {
    // Update global animation timers
    static float globalAnimTime = 0.0f;
    globalAnimTime += deltaTime;
    
    for (size_t i = 0; i < m_services.size(); ++i) {
        auto& service = m_services[i];
        
        // Breathing scale animation
        float breathPhase = globalAnimTime * 1.5f + i * 0.8f;
        float breathScale = 1.0f + sin(breathPhase) * 0.02f;
        
        vec3 baseScale = service->getBaseScale();
        service->setScale(baseScale * breathScale);
        
        // Glow pulse animation
        float glowPhase = globalAnimTime * 2.0f + i * 1.2f;
        float glowIntensity = 0.6f + sin(glowPhase) * 0.2f;
        
        // Modify glow based on service state
        if (service.get() == m_highlightedService) {
            glowIntensity = 0.9f + sin(glowPhase * 2.0f) * 0.1f;
        } else if (service.get() == m_focusedService) {
            glowIntensity = 0.8f + sin(glowPhase * 1.5f) * 0.15f;
        }
        
        service->setGlowIntensity(glowIntensity);
    }
}

void ServiceRing::applyFloatingAnimation(ServiceEntity* service, int index, float deltaTime) {
    if (!service) return;
    
    static float floatTime = 0.0f;
    floatTime += deltaTime;
    
    // Individual floating motion
    float floatPhase = floatTime * 0.8f + index * 0.6f;
    float floatOffset = sin(floatPhase) * 0.15f;
    
    // Secondary motion for complexity
    float secondaryPhase = floatTime * 1.2f + index * 0.9f;
    float secondaryOffset = cos(secondaryPhase) * 0.08f;
    
    vec3 currentPos = service->getPosition();
    currentPos.y += (floatOffset + secondaryOffset) * deltaTime * 2.0f;
    service->setPosition(currentPos);
}

void ServiceRing::updateServiceEffects(ServiceEntity* service, int index, float deltaTime) {
    if (!service) return;
    
    // Update connection indicators
    if (service->isConnected()) {
        // Create occasional data flow particles
        static float dataFlowTimer = 0.0f;
        dataFlowTimer += deltaTime;
        
        if (dataFlowTimer > (2.0f + index * 0.3f)) {
            createDataFlowEffect(service);
            dataFlowTimer = 0.0f;
        }
    }
    
    // Update service-specific visual effects based on type
    updateServiceTypeEffects(service, deltaTime);
}

void ServiceRing::updateServiceTypeEffects(ServiceEntity* service, float deltaTime) {
    ServiceEntity::ServiceType type = service->getServiceType();
    
    switch (type) {
        case ServiceEntity::ServiceType::API_GATEWAY:
            // Create occasional connection pulses
            if (rand() % 1000 < 5) { // 0.5% chance per frame
                createConnectionPulse(service);
            }
            break;
            
        case ServiceEntity::ServiceType::AI_ORCHESTRA:
            // Create neural network effects
            createNeuralNetworkEffect(service, deltaTime);
            break;
            
        case ServiceEntity::ServiceType::WORLD_ENGINE:
            // Create terrain generation effects
            createTerrainEffect(service, deltaTime);
            break;
            
        case ServiceEntity::ServiceType::SONG_ENGINE:
            // Create musical note effects
            createMusicalEffect(service, deltaTime);
            break;
            
        default:
            // Default subtle pulse effect
            createDefaultEffect(service, deltaTime);
            break;
    }
}

void ServiceRing::updateFocusTransition(float deltaTime) {
    if (!m_focusedService) return;
    
    // Update focus visual effects
    static float focusTime = 0.0f;
    focusTime += deltaTime;
    
    // Create focus beam to center
    if (static_cast<int>(focusTime * 2.0f) % 3 == 0) {
        createFocusBeam(m_focusedService);
    }
}

void ServiceRing::updateVisualEffects(float deltaTime) {
    // Update ring glow intensity based on activity
    float activityLevel = calculateRingActivity();
    m_ringGlowIntensity = lerp(m_ringGlowIntensity, activityLevel, deltaTime * 2.0f);
    
    // Update orbit trail intensity
    updateOrbitTrailIntensity(activityLevel);
    
    // Update particle systems
    updateRingParticles(deltaTime);
}

float ServiceRing::calculateRingActivity() {
    if (m_services.empty()) return 0.1f;
    
    float totalActivity = 0.0f;
    for (auto& service : m_services) {
        if (service->isConnected()) {
            totalActivity += service->getActivityLevel();
        }
    }
    
    return clamp(totalActivity / m_services.size(), 0.1f, 1.0f);
}

// Visual creation methods

void ServiceRing::createRingVisualization() {
    // Create subtle ring guides
    m_ringGuide = std::make_shared<RingGuide>();
    m_ringGuide->setRadius(m_radius);
    m_ringGuide->setThickness(0.05f);
    m_ringGuide->setColor(make_vec3(0.3f, 0.6f, 0.8f));
    m_ringGuide->setAlpha(0.3f);
    m_ringGuide->setSegments(64);
    addChild(m_ringGuide);
    
    std::cout << "Ring visualization created." << std::endl;
}

void ServiceRing::createOrbitTrails() {
    // Create orbital trail effects
    for (int i = 0; i < 3; ++i) {
        auto trail = std::make_shared<OrbitTrail>();
        trail->setRadius(m_radius + i * 0.2f);
        trail->setSpeed(0.5f + i * 0.1f);
        trail->setColor(make_vec3(0.2f, 0.5f, 0.8f));
        trail->setAlpha(0.2f - i * 0.05f);
        trail->setDirection(i % 2 == 0 ? 1.0f : -1.0f);
        
        m_orbitTrails.push_back(trail);
        addChild(trail);
    }
    
    std::cout << "Orbit trails created." << std::endl;
}

void ServiceRing::animateServiceEntry(ServiceEntity* service) {
    if (!service) return;
    
    // Start from center and expand outward
    service->setPosition(vec3_zero());
    service->setScale(make_vec3(0.1f));
    service->setAlpha(0.0f);
    
    // Create entry animation
    auto entry = std::make_shared<ServiceEntryAnimation>();
    entry->setService(service);
    entry->setDuration(1.5f);
    entry->setEntryType(ServiceEntryAnimation::Type::SPIRAL_OUT);
    addChild(entry);
    
    std::cout << "Service entry animation started: " << service->getName() << std::endl;
}

void ServiceRing::animateServiceExit(ServiceEntity* service) {
    if (!service) return;
    
    // Create exit animation
    auto exit = std::make_shared<ServiceExitAnimation>();
    exit->setService(service);
    exit->setDuration(1.0f);
    exit->setExitType(ServiceExitAnimation::Type::FADE_TO_CENTER);
    addChild(exit);
    
    std::cout << "Service exit animation started: " << service->getName() << std::endl;
}

void ServiceRing::createExpansionWave() {
    // Create expanding ring wave effect
    auto wave = std::make_shared<ExpansionWave>();
    wave->setStartRadius(m_radius);
    wave->setEndRadius(m_targetRadius);
    wave->setDuration(m_expansionDuration);
    wave->setColor(make_vec3(0.4f, 0.8f, 1.0f));
    wave->setIntensity(0.8f);
    addChild(wave);
    
    std::cout << "Expansion wave created." << std::endl;
}

void ServiceRing::createHighlightEffects(ServiceEntity* service) {
    if (!service) return;
    
    // Create highlight ring around service
    auto highlight = std::make_shared<ServiceHighlight>();
    highlight->setTarget(service);
    highlight->setRadius(1.5f);
    highlight->setColor(service->getBaseColor());
    highlight->setIntensity(0.8f);
    highlight->setPulseSpeed(2.0f);
    addChild(highlight);
    
    // Store for cleanup
    m_highlightEffects[service] = highlight;
}

void ServiceRing::createRotationIndicators(ServiceEntity* service) {
    if (!service) return;
    
    // Create rotation arrow indicators
    auto indicator = std::make_shared<RotationIndicator>();
    indicator->setTarget(service);
    indicator->setDirection(m_targetFocusAngle > m_rotationOffset ? 1.0f : -1.0f);
    indicator->setDuration(m_focusRotationDuration);
    addChild(indicator);
}

void ServiceRing::createDataFlowEffect(ServiceEntity* service) {
    if (!service) return;
    
    // Create data flow particles from service to center
    auto dataFlow = std::make_shared<DataFlowEffect>();
    dataFlow->setStartPosition(service->getWorldPosition());
    dataFlow->setEndPosition(vec3_zero());
    dataFlow->setColor(service->getBaseColor());
    dataFlow->setParticleCount(5);
    dataFlow->setDuration(2.0f);
    addChild(dataFlow);
}

void ServiceRing::createConnectionPulse(ServiceEntity* service) {
    // Create expanding pulse from service
    auto pulse = std::make_shared<ConnectionPulse>();
    pulse->setPosition(service->getWorldPosition());
    pulse->setColor(service->getBaseColor());
    pulse->setMaxRadius(2.0f);
    pulse->setDuration(1.0f);
    addChild(pulse);
}

void ServiceRing::createNeuralNetworkEffect(ServiceEntity* service, float deltaTime) {
    // Create neural connection effects
    static float neuralTimer = 0.0f;
    neuralTimer += deltaTime;
    
    if (neuralTimer > 3.0f) {
        auto neural = std::make_shared<NeuralNetworkEffect>();
        neural->setPosition(service->getWorldPosition());
        neural->setColor(service->getBaseColor());
        neural->setComplexity(0.7f);
        addChild(neural);
        neuralTimer = 0.0f;
    }
}

void ServiceRing::createTerrainEffect(ServiceEntity* service, float deltaTime) {
    // Create terrain generation visualization
    static float terrainTimer = 0.0f;
    terrainTimer += deltaTime;
    
    if (terrainTimer > 4.0f) {
        auto terrain = std::make_shared<TerrainGenerationEffect>();
        terrain->setPosition(service->getWorldPosition());
        terrain->setScale(make_vec3(0.5f));
        addChild(terrain);
        terrainTimer = 0.0f;
    }
}

void ServiceRing::createMusicalEffect(ServiceEntity* service, float deltaTime) {
    // Create musical note visualization
    static float musicTimer = 0.0f;
    musicTimer += deltaTime;
    
    if (musicTimer > 2.5f) {
        auto music = std::make_shared<MusicalNoteEffect>();
        music->setPosition(service->getWorldPosition());
        music->setColor(service->getBaseColor());
        music->setNoteType(rand() % 7); // Random musical note
        addChild(music);
        musicTimer = 0.0f;
    }
}

void ServiceRing::createDefaultEffect(ServiceEntity* service, float deltaTime) {
    // Create subtle default effect
    static float defaultTimer = 0.0f;
    defaultTimer += deltaTime;
    
    if (defaultTimer > 5.0f) {
        auto effect = std::make_shared<SubtlePulseEffect>();
        effect->setPosition(service->getWorldPosition());
        effect->setColor(service->getBaseColor());
        effect->setIntensity(0.3f);
        addChild(effect);
        defaultTimer = 0.0f;
    }
}

void ServiceRing::createFocusBeam(ServiceEntity* service) {
    if (!service) return;
    
    // Create beam from service to center
    auto beam = std::make_shared<FocusBeam>();
    beam->setStartPosition(service->getWorldPosition());
    beam->setEndPosition(vec3_zero());
    beam->setColor(service->getBaseColor());
    beam->setIntensity(0.6f);
    beam->setDuration(0.5f);
    addChild(beam);
}

// Render methods

void ServiceRing::renderRingGuides(RenderContext& context) {
    if (!m_ringGuide) return;
    
    context.pushTransform(getWorldMatrix());
    context.setColor(make_vec4(0.3f, 0.6f, 0.8f, 0.3f * m_ringGlowIntensity));
    m_ringGuide->render(context);
    context.popTransform();
}

void ServiceRing::renderOrbitTrails(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    for (auto& trail : m_orbitTrails) {
        trail->render(context);
    }
    
    context.popTransform();
}

void ServiceRing::renderFocusIndicators(RenderContext& context) {
    if (!m_focusedService) return;
    
    // Render focus effects around the focused service
    context.pushTransform(getWorldMatrix());
    
    // Focus ring
    vec3 focusPos = m_focusedService->getWorldPosition();
    context.translate(focusPos);
    context.setColor(make_vec4(1.0f, 1.0f, 1.0f, 0.8f));
    context.drawWireframeCircle(2.0f, 32);
    
    context.popTransform();
}

// Update methods

void ServiceRing::updateRingVisualizationScale() {
    if (m_ringGuide) {
        m_ringGuide->setRadius(m_radius);
    }
    
    for (size_t i = 0; i < m_orbitTrails.size(); ++i) {
        m_orbitTrails[i]->setRadius(m_radius + i * 0.2f);
    }
}

void ServiceRing::updateOrbitTrails() {
    // Update orbit trail count based on service count
    int desiredTrails = std::min(3, static_cast<int>(m_services.size()) / 2 + 1);
    
    while (m_orbitTrails.size() < desiredTrails) {
        auto trail = std::make_shared<OrbitTrail>();
        trail->setRadius(m_radius + m_orbitTrails.size() * 0.2f);
        trail->setSpeed(0.5f + m_orbitTrails.size() * 0.1f);
        trail->setColor(make_vec3(0.2f, 0.5f, 0.8f));
        trail->setAlpha(0.2f - m_orbitTrails.size() * 0.05f);
        trail->setDirection(m_orbitTrails.size() % 2 == 0 ? 1.0f : -1.0f);
        
        m_orbitTrails.push_back(trail);
        addChild(trail);
    }
    
    // Update existing trails
    for (size_t i = 0; i < m_orbitTrails.size(); ++i) {
        m_orbitTrails[i]->setRadius(m_radius + i * 0.2f);
    }
}

void ServiceRing::updateOrbitTrailIntensity(float activity) {
    for (auto& trail : m_orbitTrails) {
        trail->setIntensity(0.3f + activity * 0.5f);
    }
}

void ServiceRing::updateRingParticles(float deltaTime) {
    // Update any ring-level particle effects
    static float particleTimer = 0.0f;
    particleTimer += deltaTime;
    
    if (particleTimer > 1.0f && !m_services.empty()) {
        // Create ambient ring particles
        auto particles = std::make_shared<RingParticleEffect>();
        particles->setRadius(m_radius);
        particles->setCount(m_services.size());
        particles->setColor(make_vec3(0.4f, 0.7f, 1.0f));
        particles->setLifetime(3.0f);
        addChild(particles);
        
        particleTimer = 0.0f;
    }
}

} // namespace FinalStorm