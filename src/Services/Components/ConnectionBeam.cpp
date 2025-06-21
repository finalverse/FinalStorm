// ============================================================================
// File: FinalStorm/src/Services/Components/ConnectionBeam.cpp
// Advanced Connection Visualization Implementation for Finalverse Networks
// ============================================================================

#include "Services/Components/ConnectionBeam.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include "Core/Math/Transform.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace FinalStorm {

// ============================================================================
// ConnectionBeam Implementation
// ============================================================================

ConnectionBeam::ConnectionBeam() 
    : SceneNode("Connection Beam")
    , m_startPosition(vec3_zero())
    , m_endPosition(make_vec3(0, 0, 1))
    , m_color(make_vec3(0.4f, 0.8f, 1.0f))
    , m_intensity(1.0f)
    , m_thickness(0.02f)
    , m_flowSpeed(2.0f)
    , m_flowDirection(1.0f)
    , m_connectionType(ConnectionType::DATA_FLOW)
    , m_connectionState(ConnectionState::ACTIVE)
    , m_connectionId(0)
    , m_latency(0.0f)
    , m_bandwidth(1.0f)
    , m_duration(0.0f)
    , m_maxDuration(0.0f)
    , m_isExpired(false)
    , m_flowPhase(0.0f)
    , m_pulsePhase(0.0f)
    , m_pulseIntensity(0.0f)
    , m_turbulence(0.0f)
    , m_glowFalloff(1.0f)
    , m_quantumFlicker(false)
    , m_holographicNoise(0.0f)
    , m_quantumPhase(0.0f)
    , m_hologramPhase(0.0f)
    , m_geometryDirty(true)
    , m_segments(32) {
    
    createBeamGeometry();
    createFlowEffects();
    
    std::cout << "ConnectionBeam created." << std::endl;
}

ConnectionBeam::~ConnectionBeam() = default;

void ConnectionBeam::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateFlow(deltaTime);
    updatePulse(deltaTime);
    updateDuration(deltaTime);
    updateDataPackets(deltaTime);
    updateQuantumEffects(deltaTime);
    updateHolographicNoise(deltaTime);
    updateGeometry();
}

void ConnectionBeam::render(RenderContext& context) {
    if (m_isExpired) return;
    
    context.pushTransform(getWorldMatrix());
    context.pushBlendMode(BlendMode::ADDITIVE);
    
    renderBeam(context);
    renderFlowEffects(context);
    renderDataPackets(context);
    renderGlowEffect(context);
    
    context.popBlendMode();
    context.popTransform();
}

void ConnectionBeam::setStartPosition(const vec3& pos) {
    m_startPosition = pos;
    m_geometryDirty = true;
}

void ConnectionBeam::setEndPosition(const vec3& pos) {
    m_endPosition = pos;
    m_geometryDirty = true;
}

void ConnectionBeam::setConnectionType(ConnectionType type) {
    m_connectionType = type;
    updateMaterialProperties();
}

void ConnectionBeam::setConnectionState(ConnectionState state) {
    m_connectionState = state;
    updateMaterialProperties();
}

void ConnectionBeam::setColor(const vec3& color) {
    m_color = color;
    updateMaterialProperties();
}

void ConnectionBeam::setIntensity(float intensity) {
    m_intensity = clamp(intensity, 0.0f, 2.0f);
}

void ConnectionBeam::setThickness(float thickness) {
    m_thickness = std::max(0.001f, thickness);
    m_geometryDirty = true;
}

void ConnectionBeam::setFlowSpeed(float speed) {
    m_flowSpeed = speed;
}

void ConnectionBeam::setFlowDirection(float direction) {
    m_flowDirection = direction;
}

void ConnectionBeam::setDuration(float duration) {
    m_maxDuration = duration;
    m_duration = 0.0f;
    m_isExpired = false;
}

void ConnectionBeam::setConnectionId(uint32_t id) {
    m_connectionId = id;
}

void ConnectionBeam::pulse(float intensity) {
    m_pulsePhase = 0.0f;
    m_pulseIntensity = intensity;
    
    std::cout << "ConnectionBeam: Pulse triggered with intensity " << intensity << std::endl;
}

void ConnectionBeam::showDataPacket(const vec3& color, float size) {
    DataPacket packet;
    packet.position = m_startPosition;
    packet.color = color;
    packet.size = size;
    packet.life = 2.0f;
    packet.maxLife = 2.0f;
    packet.speed = 5.0f;
    
    m_dataPackets.push_back(packet);
    
    std::cout << "ConnectionBeam: Data packet created" << std::endl;
}

void ConnectionBeam::setLatency(float milliseconds) {
    m_latency = milliseconds;
}

void ConnectionBeam::setBandwidth(float mbps) {
    m_bandwidth = mbps;
    
    // Adjust visual properties based on bandwidth
    float normalizedBandwidth = clamp(mbps / 100.0f, 0.1f, 2.0f);
    setThickness(0.02f * normalizedBandwidth);
    
    // Update flow particle density
    if (m_flowParticles) {
        float emissionRate = 15.0f * normalizedBandwidth;
        m_flowParticles->setEmitRate(emissionRate);
    }
}

bool ConnectionBeam::isExpired() const {
    return m_isExpired;
}

float ConnectionBeam::getLength() const {
    return magnitude(m_endPosition - m_startPosition);
}

void ConnectionBeam::setTurbulence(float amount) {
    m_turbulence = clamp(amount, 0.0f, 1.0f);
}

void ConnectionBeam::setGlowFalloff(float falloff) {
    m_glowFalloff = clamp(falloff, 0.1f, 5.0f);
}

void ConnectionBeam::enableQuantumFlicker(bool enable) {
    m_quantumFlicker = enable;
}

void ConnectionBeam::setHolographicNoise(float amount) {
    m_holographicNoise = clamp(amount, 0.0f, 1.0f);
}

// ============================================================================
// Private Implementation Methods
// ============================================================================

void ConnectionBeam::createBeamGeometry() {
    // Create beam mesh with segments for smooth curves
    m_beamMesh = std::make_shared<BeamMesh>();
    m_beamMesh->setSegmentCount(m_segments);
    updateBeamMesh();
    
    // Create beam material
    m_beamMaterial = std::make_shared<Material>("connection_beam");
    updateMaterialProperties();
}

void ConnectionBeam::createFlowEffects() {
    // Create flow particle system
    ParticleEmitter::Config flowConfig;
    flowConfig.emitShape = ParticleEmitter::Shape::LINE;
    flowConfig.emitRate = 20.0f;
    flowConfig.particleLifetime = 1.0f;
    flowConfig.startSize = 0.015f;
    flowConfig.endSize = 0.005f;
    flowConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 1.0f);
    flowConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    flowConfig.velocity = 3.0f;
    flowConfig.inheritVelocity = 0.8f;
    
    m_flowParticles = std::make_shared<ParticleEmitter>(flowConfig);
    m_flowParticles->setName("Beam Flow");
    addChild(m_flowParticles);
    
    // Create glow particles for enhanced effect
    ParticleEmitter::Config glowConfig;
    glowConfig.emitShape = ParticleEmitter::Shape::LINE;
    glowConfig.emitRate = 8.0f;
    glowConfig.particleLifetime = 2.0f;
    glowConfig.startSize = 0.08f;
    glowConfig.endSize = 0.15f;
    glowConfig.startColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.3f);
    glowConfig.endColor = make_vec4(m_color.x, m_color.y, m_color.z, 0.0f);
    glowConfig.velocity = 1.0f;
    
    m_glowParticles = std::make_shared<ParticleEmitter>(glowConfig);
    m_glowParticles->setName("Beam Glow");
    addChild(m_glowParticles);
    
    updateFlowLine();
}

void ConnectionBeam::updateFlow(float deltaTime) {
    m_flowPhase += deltaTime * m_flowSpeed * m_flowDirection;
    
    // Update flow particle emission
    if (m_flowParticles) {
        updateFlowLine();
        
        // Modulate emission based on intensity and state
        float stateMultiplier = calculateStateIntensityMultiplier();
        float emissionRate = 15.0f * m_intensity * stateMultiplier;
        m_flowParticles->setEmitRate(emissionRate);
        
        // Update particle color based on connection type and state
        vec4 flowColor = getConnectionTypeColor();
        vec4 stateColor = getConnectionStateColor();
        vec4 finalColor = lerpVec4(flowColor, stateColor, 0.5f);
        finalColor.w *= m_intensity;
        
        m_flowParticles->setParticleColor(finalColor);
    }
    
    // Update glow particles
    if (m_glowParticles) {
        float glowIntensity = m_intensity * 0.3f;
        if (m_pulseIntensity > 0.0f) {
            glowIntensity += m_pulseIntensity * sin(m_pulsePhase * 6.0f) * 0.2f;
        }
        
        vec4 glowColor = make_vec4(m_color.x, m_color.y, m_color.z, glowIntensity);
        m_glowParticles->setParticleColor(glowColor);
    }
}

void ConnectionBeam::updatePulse(float deltaTime) {
    if (m_pulseIntensity > 0.0f) {
        m_pulsePhase += deltaTime * 8.0f; // Fast pulse animation
        
        // Exponential decay of pulse intensity
        float decayRate = 2.0f; // Decay over 2 seconds
        m_pulseIntensity = std::max(0.0f, m_pulseIntensity - deltaTime * decayRate);
    }
}

void ConnectionBeam::updateDuration(float deltaTime) {
    if (m_maxDuration > 0.0f) {
        m_duration += deltaTime;
        if (m_duration >= m_maxDuration) {
            m_isExpired = true;
            std::cout << "ConnectionBeam: Expired after " << m_duration << " seconds" << std::endl;
        }
    }
}

void ConnectionBeam::updateDataPackets(float deltaTime) {
    for (auto& packet : m_dataPackets) {
        // Update packet lifetime
        packet.life -= deltaTime;
        
        if (packet.life > 0.0f) {
            // Calculate progress along the beam
            float progress = 1.0f - (packet.life / packet.maxLife);
            
        void ConnectionBeam::renderBeam(RenderContext& context) {
    if (!m_beamMesh) return;
    
    // Calculate current intensity with all modifiers
    float currentIntensity = m_intensity;
    
    // Apply pulse effect
    if (m_pulseIntensity > 0.0f) {
        currentIntensity += m_pulseIntensity * sin(m_pulsePhase);
    }
    
    // Apply quantum flicker
    if (m_quantumFlicker) {
        float flicker = 0.8f + 0.2f * sin(m_quantumPhase * 3.0f) * cos(m_quantumPhase * 1.7f);
        currentIntensity *= flicker;
    }
    
    // Apply fade-out if expiring
    if (m_maxDuration > 0.0f && m_duration > 0.0f) {
        float fadeRatio = 1.0f - clamp((m_duration - m_maxDuration + 2.0f) / 2.0f, 0.0f, 1.0f);
        currentIntensity *= fadeRatio;
    }
    
    // Apply state intensity multiplier
    currentIntensity *= calculateStateIntensityMultiplier();
    
    // Set beam color and intensity
    vec4 beamColor = getConnectionTypeColor();
    beamColor = lerpVec4(beamColor, getConnectionStateColor(), 0.4f);
    beamColor.w *= currentIntensity;
    
    context.setColor(beamColor);
    context.setEmission(make_vec3(beamColor.x, beamColor.y, beamColor.z) * currentIntensity);
    
    // Render beam mesh
    m_beamMesh->render(context);
}

void ConnectionBeam::renderFlowEffects(RenderContext& context) {
    // Flow effects are rendered by the particle systems automatically
    // Additional flow visualization could be added here
    
    // Render flow indicators along the beam
    if (m_intensity > 0.3f) {
        renderFlowIndicators(context);
    }
}

void ConnectionBeam::renderFlowIndicators(RenderContext& context) {
    // Render small indicators moving along the beam
    int indicatorCount = static_cast<int>(3 + m_bandwidth);
    vec3 direction = m_endPosition - m_startPosition;
    float length = magnitude(direction);
    
    for (int i = 0; i < indicatorCount; ++i) {
        float offset = (i / float(indicatorCount)) + m_flowPhase * 0.1f;
        offset = fmod(offset, 1.0f);
        
        if (m_flowDirection < 0.0f) {
            offset = 1.0f - offset;
        }
        
        vec3 indicatorPos = calculateBeamPosition(offset);
        
        context.pushTransform(mat4_identity());
        context.translate(indicatorPos);
        
        float indicatorIntensity = m_intensity * 0.8f;
        vec4 indicatorColor = getConnectionTypeColor();
        indicatorColor.w *= indicatorIntensity;
        context.setColor(indicatorColor);
        
        float indicatorSize = 0.02f + (m_thickness * 2.0f);
        context.drawGlowSphere(indicatorSize);
        
        context.popTransform();
    }
}

void ConnectionBeam::renderDataPackets(RenderContext& context) {
    for (const auto& packet : m_dataPackets) {
        if (packet.life > 0.0f) {
            context.pushTransform(mat4_identity());
            context.translate(packet.position);
            
            // Calculate packet alpha based on remaining life
            float alpha = clamp(packet.life / packet.maxLife, 0.0f, 1.0f);
            vec4 packetColor = make_vec4(packet.color.x, packet.color.y, packet.color.z, alpha);
            context.setColor(packetColor);
            
            // Draw packet with trailing effect
            context.drawGlowSphere(packet.size);
            
            // Add trailing particles
            context.setColor(make_vec4(packetColor.x, packetColor.y, packetColor.z, alpha * 0.3f));
            context.drawGlowSphere(packet.size * 1.5f);
            
            context.popTransform();
        }
    }
}

void ConnectionBeam::renderGlowEffect(RenderContext& context) {
    // Render outer glow effect
    float glowIntensity = m_intensity * 0.3f;
    
    if (glowIntensity > 0.1f) {
        vec4 glowColor = getConnectionTypeColor();
        glowColor.w = glowIntensity / m_glowFalloff;
        
        context.setColor(glowColor);
        context.setBlendMode(BlendMode::ADDITIVE);
        
        // Render glow as expanded beam
        if (m_beamMesh) {
            // Create temporary expanded mesh for glow
            std::vector<vec3> centerLine;
            vec3 direction = normalize(m_endPosition - m_startPosition);
            
            for (int i = 0; i <= m_segments; ++i) {
                float t = i / float(m_segments);
                vec3 point = lerp(m_startPosition, m_endPosition, t);
                centerLine.push_back(point);
            }
            
            // Use a temporary beam mesh with larger thickness for glow
            auto glowMesh = std::make_shared<BeamMesh>();
            glowMesh->setSegmentCount(m_segments);
            glowMesh->updateCenterLine(centerLine, m_thickness * 3.0f);
            glowMesh->render(context);
        }
    }
}

vec3 ConnectionBeam::calculateBeamPosition(float t) const {
    // Basic linear interpolation
    vec3 basePos = lerp(m_startPosition, m_endPosition, t);
    
    // Add curve for organic feel
    vec3 direction = normalize(m_endPosition - m_startPosition);
    float length = magnitude(m_endPosition - m_startPosition);
    
    if (m_connectionType == ConnectionType::DATA_FLOW || 
        m_connectionType == ConnectionType::STREAM) {
        float curvature = sin(t * M_PI) * length * 0.02f;
        vec3 perpendicular = cross(direction, make_vec3(0, 1, 0));
        if (magnitude(perpendicular) < 0.1f) {
            perpendicular = cross(direction, make_vec3(1, 0, 0));
        }
        perpendicular = normalize(perpendicular);
        basePos = basePos + perpendicular * curvature;
    }
    
    return basePos;
}

vec3 ConnectionBeam::calculateFlowTurbulence(const vec3& position, float time) const {
    // Simple Perlin-like noise for turbulence
    float x = sin(position.x * 0.3f + time * 2.0f) * cos(position.y * 0.25f + time * 1.5f);
    float y = cos(position.y * 0.3f + time * 1.8f) * sin(position.z * 0.22f + time * 2.2f);
    float z = sin(position.z * 0.3f + time * 1.9f) * cos(position.x * 0.28f + time * 1.7f);
    
    return make_vec3(x, y, z) * 0.1f;
}

vec4 ConnectionBeam::getConnectionTypeColor() const {
    switch (m_connectionType) {
        case ConnectionType::DATA_FLOW:
            return make_vec4(0.4f, 0.8f, 1.0f, 0.8f); // Cyan
        case ConnectionType::QUERY:
            return make_vec4(0.8f, 0.4f, 1.0f, 0.8f); // Purple
        case ConnectionType::API_CALL:
            return make_vec4(0.2f, 1.0f, 0.4f, 0.8f); // Green
        case ConnectionType::STREAM:
            return make_vec4(1.0f, 0.6f, 0.2f, 0.8f); // Orange
        case ConnectionType::BLOCKCHAIN:
            return make_vec4(1.0f, 0.8f, 0.2f, 0.8f); // Gold
        case ConnectionType::AI_INFERENCE:
            return make_vec4(1.0f, 0.2f, 0.8f, 0.8f); // Magenta
        case ConnectionType::AUDIO_SIGNAL:
            return make_vec4(0.6f, 1.0f, 0.8f, 0.8f); // Cyan-green
        default:
            return make_vec4(0.6f, 0.6f, 0.6f, 0.8f); // Gray
    }
}

vec4 ConnectionBeam::getConnectionStateColor() const {
    switch (m_connectionState) {
        case ConnectionState::ESTABLISHING:
            return make_vec4(0.8f, 0.8f, 0.2f, 0.6f); // Yellow
        case ConnectionState::ACTIVE:
            return make_vec4(0.2f, 1.0f, 0.4f, 0.8f); // Green
        case ConnectionState::HIGH_LOAD:
            return make_vec4(1.0f, 0.6f, 0.2f, 0.9f); // Orange
        case ConnectionState::ERROR:
            return make_vec4(1.0f, 0.2f, 0.2f, 1.0f); // Red
        case ConnectionState::TIMEOUT:
            return make_vec4(0.8f, 0.4f, 0.2f, 0.7f); // Dark orange
        case ConnectionState::CLOSING:
            return make_vec4(0.6f, 0.6f, 0.6f, 0.4f); // Fading gray
        default:
            return make_vec4(0.5f, 0.5f, 0.5f, 0.6f); // Neutral gray
    }
}

float ConnectionBeam::calculateStateIntensityMultiplier() const {
    switch (m_connectionState) {
        case ConnectionState::ESTABLISHING:
            return 0.6f + 0.4f * sin(m_flowPhase * 3.0f); // Pulsing
        case ConnectionState::ACTIVE:
            return 1.0f;
        case ConnectionState::HIGH_LOAD:
            return 1.2f + 0.3f * sin(m_flowPhase * 5.0f); // Intense pulsing
        case ConnectionState::ERROR:
            return 0.8f + 0.7f * sin(m_flowPhase * 8.0f); // Rapid flashing
        case ConnectionState::TIMEOUT:
            return 0.4f;
        case ConnectionState::CLOSING:
            return std::max(0.1f, 1.0f - (m_duration / std::max(m_maxDuration, 1.0f))); // Fading
        default:
            return 0.5f;
    }
}

vec4 ConnectionBeam::lerpVec4(const vec4& a, const vec4& b, float t) const {
    return make_vec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
}

// ============================================================================
// BeamMesh Implementation
// ============================================================================

BeamMesh::BeamMesh() 
    : m_segmentCount(32)
    , m_isDirty(true)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr) {
}

BeamMesh::~BeamMesh() = default;

void BeamMesh::setSegmentCount(int segments) {
    m_segmentCount = std::max(4, segments);
    m_isDirty = true;
}

void BeamMesh::updateCenterLine(const std::vector<vec3>& centerLine, float thickness) {
    if (centerLine.size() < 2) return;
    
    generateTube(centerLine, thickness);
    m_isDirty = true;
}

void BeamMesh::generateTube(const std::vector<vec3>& centerLine, float thickness) {
    m_vertices.clear();
    m_normals.clear();
    m_uvs.clear();
    m_indices.clear();
    
    int radialSegments = 8; // Number of vertices around the tube
    float radius = thickness * 0.5f;
    
    // Generate vertices
    for (size_t i = 0; i < centerLine.size(); ++i) {
        vec3 center = centerLine[i];
        vec3 tangent = calculateTangent(centerLine, static_cast<int>(i));
        vec3 binormal = calculateBinormal(tangent);
        vec3 normal = cross(tangent, binormal);
        
        float v = static_cast<float>(i) / (centerLine.size() - 1);
        
        for (int j = 0; j < radialSegments; ++j) {
            float angle = (j / float(radialSegments)) * 2.0f * M_PI;
            float cosAngle = cos(angle);
            float sinAngle = sin(angle);
            
            vec3 offset = (binormal * cosAngle + normal * sinAngle) * radius;
            vec3 vertex = center + offset;
            vec3 vertexNormal = normalize(offset);
            
            m_vertices.push_back(vertex);
            m_normals.push_back(vertexNormal);
            m_uvs.push_back(make_vec2(j / float(radialSegments), v));
        }
    }
    
    generateIndices();
}

void BeamMesh::generateRibbon(const std::vector<vec3>& centerLine, float width) {
    m_vertices.clear();
    m_normals.clear();
    m_uvs.clear();
    m_indices.clear();
    
    float halfWidth = width * 0.5f;
    
    // Generate ribbon vertices
    for (size_t i = 0; i < centerLine.size(); ++i) {
        vec3 center = centerLine[i];
        vec3 tangent = calculateTangent(centerLine, static_cast<int>(i));
        vec3 binormal = calculateBinormal(tangent, make_vec3(0, 1, 0));
        
        float v = static_cast<float>(i) / (centerLine.size() - 1);
        
        // Left vertex
        vec3 leftVertex = center - binormal * halfWidth;
        m_vertices.push_back(leftVertex);
        m_normals.push_back(make_vec3(0, 0, 1)); // Simple upward normal
        m_uvs.push_back(make_vec2(0.0f, v));
        
        // Right vertex
        vec3 rightVertex = center + binormal * halfWidth;
        m_vertices.push_back(rightVertex);
        m_normals.push_back(make_vec3(0, 0, 1));
        m_uvs.push_back(make_vec2(1.0f, v));
    }
    
    // Generate indices for ribbon
    for (size_t i = 0; i < centerLine.size() - 1; ++i) {
        uint32_t base = static_cast<uint32_t>(i * 2);
        
        // First triangle
        m_indices.push_back(base);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);
        
        // Second triangle
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 3);
        m_indices.push_back(base + 2);
    }
}

void BeamMesh::render(RenderContext& context) {
    if (m_isDirty) {
        updateBuffers();
        m_isDirty = false;
    }
    
    if (!m_vertices.empty() && !m_indices.empty()) {
        // Render the mesh using the vertex and index buffers
        context.drawIndexedMesh(m_vertexBuffer, m_indexBuffer, static_cast<int>(m_indices.size()));
    }
}

void BeamMesh::generateIndices() {
    int radialSegments = 8;
    int longitudinalSegments = static_cast<int>(m_vertices.size() / radialSegments) - 1;
    
    for (int i = 0; i < longitudinalSegments; ++i) {
        for (int j = 0; j < radialSegments; ++j) {
            int current = i * radialSegments + j;
            int next = current + radialSegments;
            int currentNext = i * radialSegments + ((j + 1) % radialSegments);
            int nextNext = currentNext + radialSegments;
            
            // First triangle
            m_indices.push_back(current);
            m_indices.push_back(next);
            m_indices.push_back(currentNext);
            
            // Second triangle
            m_indices.push_back(currentNext);
            m_indices.push_back(next);
            m_indices.push_back(nextNext);
        }
    }
}

void BeamMesh::updateBuffers() {
    // In a real implementation, this would update the Metal/Vulkan buffers
    // For now, we'll just mark that the buffers are updated
    
    // Create or update vertex buffer
    if (m_vertexBuffer) {
        // Update existing buffer
    } else {
        // Create new buffer
        // m_vertexBuffer = createVertexBuffer(m_vertices, m_normals, m_uvs);
    }
    
    // Create or update index buffer
    if (m_indexBuffer) {
        // Update existing buffer
    } else {
        // Create new buffer
        // m_indexBuffer = createIndexBuffer(m_indices);
    }
}

vec3 BeamMesh::calculateTangent(const std::vector<vec3>& centerLine, int index) const {
    if (centerLine.size() < 2) return make_vec3(1, 0, 0);
    
    if (index == 0) {
        return normalize(centerLine[1] - centerLine[0]);
    } else if (index == static_cast<int>(centerLine.size()) - 1) {
        return normalize(centerLine[index] - centerLine[index - 1]);
    } else {
        vec3 tangent1 = centerLine[index] - centerLine[index - 1];
        vec3 tangent2 = centerLine[index + 1] - centerLine[index];
        return normalize(tangent1 + tangent2);
    }
}

vec3 BeamMesh::calculateBinormal(const vec3& tangent, const vec3& up) const {
    vec3 binormal = cross(tangent, up);
    if (magnitude(binormal) < 0.1f) {
        // Fallback if tangent is parallel to up
        binormal = cross(tangent, make_vec3(1, 0, 0));
        if (magnitude(binormal) < 0.1f) {
            binormal = cross(tangent, make_vec3(0, 0, 1));
        }
    }
    return normalize(binormal);
}

// ============================================================================
// ConnectionManager Implementation
// ============================================================================

ConnectionManager::ConnectionManager() 
    : SceneNode("Connection Manager")
    , m_nextConnectionId(1)
    , m_globalIntensity(1.0f)
    , m_globalFlowSpeed(1.0f) {
    
    std::cout << "ConnectionManager created." << std::endl;
}

ConnectionManager::~ConnectionManager() = default;

std::shared_ptr<ConnectionBeam> ConnectionManager::createConnection(
    const vec3& startPos,
    const vec3& endPos,
    ConnectionBeam::ConnectionType type) {
    
    auto connection = std::make_shared<ConnectionBeam>();
    connection->setStartPosition(startPos);
    connection->setEndPosition(endPos);
    connection->setConnectionType(type);
    connection->setConnectionId(generateConnectionId());
    
    m_connections.push_back(connection);
    addChild(connection);
    
    std::cout << "ConnectionManager: Created connection " << connection->getConnectionId() << std::endl;
    
    return connection;
}

void ConnectionManager::removeConnection(uint32_t connectionId) {
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [connectionId](const std::shared_ptr<ConnectionBeam>& conn) {
            return conn->getConnectionId() == connectionId;
        });
    
    if (it != m_connections.end()) {
        removeChild(*it);
        m_connections.erase(it);
        std::cout << "ConnectionManager: Removed connection " << connectionId << std::endl;
    }
}

void ConnectionManager::removeConnection(std::shared_ptr<ConnectionBeam> beam) {
    if (beam) {
        removeConnection(beam->getConnectionId());
    }
}

void ConnectionManager::removeAllConnections() {
    for (auto& connection : m_connections) {
        removeChild(connection);
    }
    m_connections.clear();
    std::cout << "ConnectionManager: Removed all connections." << std::endl;
}

void ConnectionManager::update(float deltaTime) {
    SceneNode::update(deltaTime);
    
    updateAllConnections(deltaTime);
    cleanupExpiredConnections();
}

void ConnectionManager::render(RenderContext& context) {
    // Connections render themselves as child nodes
    SceneNode::render(context);
}

void ConnectionManager::updateAllConnections(float deltaTime) {
    for (auto& connection : m_connections) {
        if (connection && !connection->isExpired()) {
            // Apply global modifiers
            connection->setIntensity(connection->getIntensity() * m_globalIntensity);
            connection->setFlowSpeed(connection->getFlowSpeed() * m_globalFlowSpeed);
        }
    }
}

void ConnectionManager::setGlobalIntensity(float intensity) {
    m_globalIntensity = clamp(intensity, 0.0f, 2.0f);
}

void ConnectionManager::setGlobalFlowSpeed(float speed) {
    m_globalFlowSpeed = clamp(speed, 0.1f, 5.0f);
}

void ConnectionManager::pulseAllConnections(float intensity) {
    for (auto& connection : m_connections) {
        if (connection && !connection->isExpired()) {
            connection->pulse(intensity);
        }
    }
    std::cout << "ConnectionManager: Pulsed all connections with intensity " << intensity << std::endl;
}

std::shared_ptr<ConnectionBeam> ConnectionManager::findConnection(uint32_t connectionId) {
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [connectionId](const std::shared_ptr<ConnectionBeam>& conn) {
            return conn->getConnectionId() == connectionId;
        });
    
    return (it != m_connections.end()) ? *it : nullptr;
}

std::vector<std::shared_ptr<ConnectionBeam>> ConnectionManager::getConnectionsByType(ConnectionBeam::ConnectionType type) {
    std::vector<std::shared_ptr<ConnectionBeam>> result;
    
    for (auto& connection : m_connections) {
        if (connection && connection->getType() == type) {
            result.push_back(connection);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<ConnectionBeam>> ConnectionManager::getActiveConnections() {
    std::vector<std::shared_ptr<ConnectionBeam>> result;
    
    for (auto& connection : m_connections) {
        if (connection && !connection->isExpired()) {
            result.push_back(connection);
        }
    }
    
    return result;
}

void ConnectionManager::connectServices(const std::string& serviceA, const std::string& serviceB, 
                                      ConnectionBeam::ConnectionType type) {
    vec3 posA = getServicePosition(serviceA);
    vec3 posB = getServicePosition(serviceB);
    
    auto connection = createConnection(posA, posB, type);
    
    auto key = std::make_pair(serviceA, serviceB);
    m_serviceConnections[key] = connection;
    
    registerServicePosition(serviceA, posA);
    registerServicePosition(serviceB, posB);
}

void ConnectionManager::disconnectServices(const std::string& serviceA, const std::string& serviceB) {
    auto key = std::make_pair(serviceA, serviceB);
    auto it = m_serviceConnections.find(key);
    
    if (it != m_serviceConnections.end()) {
        removeConnection(it->second);
        m_serviceConnections.erase(it);
    }
}

void ConnectionManager::onServiceActivity(const std::string& serviceName, float intensity) {
    // Find connections involving this service and pulse them
    for (auto& pair : m_serviceConnections) {
        if (pair.first.first == serviceName || pair.first.second == serviceName) {
            if (pair.second && !pair.second->isExpired()) {
                pair.second->pulse(intensity);
            }
        }
    }
}

void ConnectionManager::onDataTransfer(const std::string& from, const std::string& to, float amount) {
    auto key = std::make_pair(from, to);
    auto it = m_serviceConnections.find(key);
    
    if (it != m_serviceConnections.end() && it->second && !it->second->isExpired()) {
        vec3 packetColor = make_vec3(0.2f, 1.0f, 0.6f);
        float packetSize = 0.05f + amount * 0.1f;
        it->second->showDataPacket(packetColor, packetSize);
    }
}

void ConnectionManager::onConnectionError(const std::string& from, const std::string& to) {
    auto key = std::make_pair(from, to);
    auto it = m_serviceConnections.find(key);
    
    if (it != m_serviceConnections.end() && it->second && !it->second->isExpired()) {
        it->second->setConnectionState(ConnectionBeam::ConnectionState::ERROR);
        it->second->pulse(1.5f);
    }
}

void ConnectionManager::cleanupExpiredConnections() {
    auto it = std::remove_if(m_connections.begin(), m_connections.end(),
        [this](const std::shared_ptr<ConnectionBeam>& conn) {
            if (conn && conn->isExpired()) {
                removeChild(conn);
                return true;
            }
            return false;
        });
    
    if (it != m_connections.end()) {
        m_connections.erase(it, m_connections.end());
    }
    
    // Also cleanup service connections
    for (auto it = m_serviceConnections.begin(); it != m_serviceConnections.end();) {
        if (!it->second || it->second->isExpired()) {
            it = m_serviceConnections.erase(it);
        } else {
            ++it;
        }
    }
}

uint32_t ConnectionManager::generateConnectionId() {
    return m_nextConnectionId++;
}

vec3 ConnectionManager::getServicePosition(const std::string& serviceName) {
    auto it = m_servicePositions.find(serviceName);
    if (it != m_servicePositions.end()) {
        return it->second;
    }
    
    // Return default position if service not found
    return vec3_zero();
}

void ConnectionManager::registerServicePosition(const std::string& serviceName, const vec3& position) {
    m_servicePositions[serviceName] = position;
}

} // namespace FinalStorm flow direction
            if (m_flowDirection < 0.0f) {
                progress = 1.0f - progress;
            }
            
            // Calculate position along beam path
            packet.position = calculateBeamPosition(progress);
            
            // Add turbulence if enabled
            if (m_turbulence > 0.0f) {
                vec3 turbulenceOffset = calculateFlowTurbulence(packet.position, m_flowPhase);
                packet.position = packet.position + turbulenceOffset * m_turbulence * 0.1f;
            }
        }
    }
    
    // Remove expired packets
    m_dataPackets.erase(
        std::remove_if(m_dataPackets.begin(), m_dataPackets.end(),
            [](const DataPacket& packet) { return packet.life <= 0.0f; }),
        m_dataPackets.end()
    );
}

void ConnectionBeam::updateQuantumEffects(float deltaTime) {
    if (m_quantumFlicker) {
        m_quantumPhase += deltaTime * 15.0f; // Fast quantum fluctuation
    }
}

void ConnectionBeam::updateHolographicNoise(float deltaTime) {
    if (m_holographicNoise > 0.0f) {
        m_hologramPhase += deltaTime * 10.0f; // Moderate hologram interference
    }
}

void ConnectionBeam::updateGeometry() {
    if (m_geometryDirty) {
        updateBeamMesh();
        updateFlowLine();
        m_geometryDirty = false;
    }
}

void ConnectionBeam::updateBeamMesh() {
    if (!m_beamMesh) return;
    
    // Calculate beam parameters
    vec3 direction = m_endPosition - m_startPosition;
    float length = magnitude(direction);
    vec3 normalizedDir = normalize(direction);
    
    // Generate beam vertices with slight curve for organic feel
    std::vector<vec3> centerLine;
    for (int i = 0; i <= m_segments; ++i) {
        float t = i / float(m_segments);
        vec3 point = lerp(m_startPosition, m_endPosition, t);
        
        // Add subtle curve based on connection type
        if (m_connectionType == ConnectionType::DATA_FLOW || 
            m_connectionType == ConnectionType::STREAM) {
            float curvature = sin(t * M_PI) * length * 0.02f;
            vec3 perpendicular = cross(normalizedDir, make_vec3(0, 1, 0));
            if (magnitude(perpendicular) < 0.1f) {
                perpendicular = cross(normalizedDir, make_vec3(1, 0, 0));
            }
            perpendicular = normalize(perpendicular);
            point = point + perpendicular * curvature;
        }
        
        // Add noise for holographic interference
        if (m_holographicNoise > 0.0f) {
            vec3 noise = make_vec3(
                sin(t * 20.0f + m_hologramPhase) * 0.02f,
                cos(t * 15.0f + m_hologramPhase * 1.3f) * 0.015f,
                sin(t * 25.0f + m_hologramPhase * 0.8f) * 0.018f
            );
            point = point + noise * m_holographicNoise;
        }
        
        centerLine.push_back(point);
    }
    
    m_beamMesh->updateCenterLine(centerLine, m_thickness);
}

void ConnectionBeam::updateFlowLine() {
    if (m_flowParticles) {
        m_flowParticles->setEmitLine(m_startPosition, m_endPosition);
        
        // Set flow direction
        vec3 flowDir = normalize(m_endPosition - m_startPosition);
        if (m_flowDirection < 0.0f) {
            flowDir = -flowDir;
        }
        m_flowParticles->setEmitDirection(flowDir);
    }
    
    if (m_glowParticles) {
        m_glowParticles->setEmitLine(m_startPosition, m_endPosition);
        vec3 flowDir = normalize(m_endPosition - m_startPosition);
        if (m_flowDirection < 0.0f) {
            flowDir = -flowDir;
        }
        m_glowParticles->setEmitDirection(flowDir);
    }
}

void ConnectionBeam::updateMaterialProperties() {
    if (!m_beamMaterial) return;
    
    // Base color from connection type
    vec4 typeColor = getConnectionTypeColor();
    vec4 stateColor = getConnectionStateColor();
    
    // Blend type and state colors
    vec4 finalColor = lerpVec4(typeColor, stateColor, 0.6f);
    finalColor.w *= m_intensity;
    
    m_beamMaterial->setAlbedo(make_vec3(finalColor.x, finalColor.y, finalColor.z));
    m_beamMaterial->setEmission(make_vec3(finalColor.x, finalColor.y, finalColor.z));
    m_beamMaterial->setEmissionStrength(m_intensity);
    m_beamMaterial->setAlpha(finalColor.w);
}

void ConnectionBeam::renderBeam(RenderContext& context) {
    if (!m_beamMesh) return;
    
    // Calculate current intensity with all modifiers
    float currentIntensity = m_intensity;
    
    // Apply pulse effect
    if (m_pulseIntensity > 0.0f) {
        currentIntensity += m_pulseIntensity * sin(m_pulsePhase);
    }
    
    // Apply