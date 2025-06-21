// ============================================================================
// File: FinalStorm/src/Services/Components/ConnectionBeam.cpp
// Non-rendering logic for connection beams
// ============================================================================

#include "Services/Components/ConnectionBeam.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/Material.h"
#include "Core/Math/Math.h"
#include <algorithm>
#include <iostream>

namespace FinalStorm {

ConnectionBeam::ConnectionBeam()
    : SceneNode("Connection Beam"),
      m_startPosition(vec3_zero()),
      m_endPosition(make_vec3(0, 0, 1)),
      m_color(make_vec3(0.4f, 0.8f, 1.0f)),
      m_intensity(1.0f),
      m_thickness(0.02f),
      m_flowSpeed(2.0f),
      m_flowDirection(1.0f),
      m_connectionType(ConnectionType::DATA_FLOW),
      m_connectionState(ConnectionState::ACTIVE),
      m_connectionId(0),
      m_latency(0.0f),
      m_bandwidth(1.0f),
      m_duration(0.0f),
      m_maxDuration(0.0f),
      m_isExpired(false),
      m_flowPhase(0.0f),
      m_pulsePhase(0.0f),
      m_pulseIntensity(0.0f),
      m_turbulence(0.0f),
      m_glowFalloff(1.0f),
      m_quantumFlicker(false),
      m_holographicNoise(0.0f),
      m_quantumPhase(0.0f),
      m_hologramPhase(0.0f),
      m_geometryDirty(true),
      m_segments(32) {
    createBeamGeometry();
    createFlowEffects();
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
}

void ConnectionBeam::showDataPacket(const vec3& color, float size) {
    DataPacket packet{};
    packet.position = m_startPosition;
    packet.color = color;
    packet.size = size;
    packet.life = 2.0f;
    packet.maxLife = 2.0f;
    packet.speed = 5.0f;
    m_dataPackets.push_back(packet);
}

void ConnectionBeam::setLatency(float milliseconds) {
    m_latency = milliseconds;
}

void ConnectionBeam::setBandwidth(float mbps) {
    m_bandwidth = mbps;
    float normalizedBandwidth = clamp(mbps / 100.0f, 0.1f, 2.0f);
    setThickness(0.02f * normalizedBandwidth);
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
// Private implementation
// ============================================================================

void ConnectionBeam::createBeamGeometry() {
    m_beamMesh = std::make_shared<BeamMesh>();
    m_beamMesh->setSegmentCount(m_segments);
    updateBeamMesh();

    m_beamMaterial = std::make_shared<Material>("connection_beam");
    updateMaterialProperties();
}

void ConnectionBeam::createFlowEffects() {
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

    if (m_flowParticles) {
        updateFlowLine();
        float stateMul = calculateStateIntensityMultiplier();
        float emission = 15.0f * m_intensity * stateMul;
        m_flowParticles->setEmitRate(emission);
        vec4 flowColor = getConnectionTypeColor();
        vec4 stateColor = getConnectionStateColor();
        vec4 finalColor = lerpVec4(flowColor, stateColor, 0.5f);
        finalColor.w *= m_intensity;
        m_flowParticles->setParticleColor(finalColor);
    }

    if (m_glowParticles) {
        float glowIntensity = m_intensity * 0.3f;
        if (m_pulseIntensity > 0.0f)
            glowIntensity += m_pulseIntensity * sin(m_pulsePhase * 6.0f) * 0.2f;
        vec4 glowColor = make_vec4(m_color.x, m_color.y, m_color.z, glowIntensity);
        m_glowParticles->setParticleColor(glowColor);
    }
}

void ConnectionBeam::updatePulse(float deltaTime) {
    if (m_pulseIntensity > 0.0f) {
        m_pulsePhase += deltaTime * 8.0f;
        float decayRate = 2.0f;
        m_pulseIntensity = std::max(0.0f, m_pulseIntensity - deltaTime * decayRate);
    }
}

void ConnectionBeam::updateDuration(float deltaTime) {
    if (m_maxDuration > 0.0f) {
        m_duration += deltaTime;
        if (m_duration >= m_maxDuration) {
            m_isExpired = true;
        }
    }
}

void ConnectionBeam::updateDataPackets(float deltaTime) {
    for (auto& packet : m_dataPackets) {
        packet.life -= deltaTime;
        if (packet.life > 0.0f) {
            float progress = 1.0f - (packet.life / packet.maxLife);
            if (m_flowDirection < 0.0f)
                progress = 1.0f - progress;
            packet.position = calculateBeamPosition(progress);
            if (m_turbulence > 0.0f) {
                vec3 turb = calculateFlowTurbulence(packet.position, m_flowPhase);
                packet.position = packet.position + turb * m_turbulence * 0.1f;
            }
        }
    }
    m_dataPackets.erase(std::remove_if(m_dataPackets.begin(), m_dataPackets.end(),
                                       [](const DataPacket& p) { return p.life <= 0.0f; }),
                        m_dataPackets.end());
}

void ConnectionBeam::updateQuantumEffects(float deltaTime) {
    if (m_quantumFlicker) {
        m_quantumPhase += deltaTime * 15.0f;
    }
}

void ConnectionBeam::updateHolographicNoise(float deltaTime) {
    if (m_holographicNoise > 0.0f) {
        m_hologramPhase += deltaTime * 10.0f;
    }
}

void ConnectionBeam::updateGeometry() {
    if (m_geometryDirty) {
        updateBeamMesh();
        updateFlowLine();
        m_geometryDirty = false;
    }
}

// ============================================================================
// ConnectionManager implementation (non-rendering)
// ============================================================================

ConnectionManager::ConnectionManager()
    : SceneNode("Connection Manager"),
      m_nextConnectionId(1),
      m_globalIntensity(1.0f),
      m_globalFlowSpeed(1.0f) {}

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
    return connection;
}

void ConnectionManager::removeConnection(uint32_t connectionId) {
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [connectionId](const std::shared_ptr<ConnectionBeam>& c) {
            return c->getConnectionId() == connectionId;
        });
    if (it != m_connections.end()) {
        removeChild(*it);
        m_connections.erase(it);
    }
}

void ConnectionManager::removeConnection(std::shared_ptr<ConnectionBeam> beam) {
    if (beam) removeConnection(beam->getConnectionId());
}

void ConnectionManager::removeAllConnections() {
    for (auto& c : m_connections) {
        removeChild(c);
    }
    m_connections.clear();
}

void ConnectionManager::update(float deltaTime) {
    SceneNode::update(deltaTime);
    updateAllConnections(deltaTime);
    cleanupExpiredConnections();
}

void ConnectionManager::render(RenderContext& context) {
    SceneNode::render(context);
}

void ConnectionManager::updateAllConnections(float deltaTime) {
    for (auto& c : m_connections) {
        if (c && !c->isExpired()) {
            c->setIntensity(c->getIntensity() * m_globalIntensity);
            c->setFlowSpeed(c->getFlowSpeed() * m_globalFlowSpeed);
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
    for (auto& c : m_connections) {
        if (c && !c->isExpired()) c->pulse(intensity);
    }
}

std::shared_ptr<ConnectionBeam> ConnectionManager::findConnection(uint32_t id) {
    auto it = std::find_if(m_connections.begin(), m_connections.end(),
        [id](const std::shared_ptr<ConnectionBeam>& c) { return c->getConnectionId() == id; });
    return it != m_connections.end() ? *it : nullptr;
}

std::vector<std::shared_ptr<ConnectionBeam>> ConnectionManager::getConnectionsByType(ConnectionBeam::ConnectionType type) {
    std::vector<std::shared_ptr<ConnectionBeam>> result;
    for (auto& c : m_connections) {
        if (c && c->getType() == type) result.push_back(c);
    }
    return result;
}

std::vector<std::shared_ptr<ConnectionBeam>> ConnectionManager::getActiveConnections() {
    std::vector<std::shared_ptr<ConnectionBeam>> result;
    for (auto& c : m_connections) {
        if (c && !c->isExpired()) result.push_back(c);
    }
    return result;
}

void ConnectionManager::connectServices(const std::string& a, const std::string& b,
                                        ConnectionBeam::ConnectionType type) {
    vec3 posA = getServicePosition(a);
    vec3 posB = getServicePosition(b);
    auto conn = createConnection(posA, posB, type);
    m_serviceConnections[{a, b}] = conn;
    registerServicePosition(a, posA);
    registerServicePosition(b, posB);
}

void ConnectionManager::disconnectServices(const std::string& a, const std::string& b) {
    auto key = std::make_pair(a, b);
    auto it = m_serviceConnections.find(key);
    if (it != m_serviceConnections.end()) {
        removeConnection(it->second);
        m_serviceConnections.erase(it);
    }
}

void ConnectionManager::onServiceActivity(const std::string& service, float intensity) {
    for (auto& p : m_serviceConnections) {
        if (p.first.first == service || p.first.second == service) {
            if (p.second && !p.second->isExpired()) p.second->pulse(intensity);
        }
    }
}

void ConnectionManager::onDataTransfer(const std::string& from, const std::string& to, float amount) {
    auto it = m_serviceConnections.find({from, to});
    if (it != m_serviceConnections.end() && it->second && !it->second->isExpired()) {
        vec3 color = make_vec3(0.2f, 1.0f, 0.6f);
        float size = 0.05f + amount * 0.1f;
        it->second->showDataPacket(color, size);
    }
}

void ConnectionManager::onConnectionError(const std::string& from, const std::string& to) {
    auto it = m_serviceConnections.find({from, to});
    if (it != m_serviceConnections.end() && it->second && !it->second->isExpired()) {
        it->second->setConnectionState(ConnectionBeam::ConnectionState::ERROR);
        it->second->pulse(1.5f);
    }
}

void ConnectionManager::cleanupExpiredConnections() {
    auto it = std::remove_if(m_connections.begin(), m_connections.end(),
        [this](const std::shared_ptr<ConnectionBeam>& c) {
            if (c && c->isExpired()) {
                removeChild(c);
                return true;
            }
            return false;
        });
    if (it != m_connections.end()) {
        m_connections.erase(it, m_connections.end());
    }
    for (auto itr = m_serviceConnections.begin(); itr != m_serviceConnections.end();) {
        if (!itr->second || itr->second->isExpired()) {
            itr = m_serviceConnections.erase(itr);
        } else {
            ++itr;
        }
    }
}

uint32_t ConnectionManager::generateConnectionId() {
    return m_nextConnectionId++;
}

vec3 ConnectionManager::getServicePosition(const std::string& name) {
    auto it = m_servicePositions.find(name);
    return it != m_servicePositions.end() ? it->second : vec3_zero();
}

void ConnectionManager::registerServicePosition(const std::string& name, const vec3& pos) {
    m_servicePositions[name] = pos;
}

} // namespace FinalStorm
