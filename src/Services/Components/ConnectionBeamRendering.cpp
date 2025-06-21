// ============================================================================
// File: FinalStorm/src/Services/Components/ConnectionBeamRendering.cpp
// Rendering helpers for ConnectionBeam
// ============================================================================

#include "Services/Components/ConnectionBeam.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include <cmath>
#include <memory>

namespace FinalStorm {

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

void ConnectionBeam::renderBeam(RenderContext& context) {
    if (!m_beamMesh) return;
    float currentIntensity = m_intensity;
    if (m_pulseIntensity > 0.0f)
        currentIntensity += m_pulseIntensity * sin(m_pulsePhase);
    if (m_quantumFlicker) {
        float flicker = 0.8f + 0.2f * sin(m_quantumPhase * 3.0f) * cos(m_quantumPhase * 1.7f);
        currentIntensity *= flicker;
    }
    if (m_maxDuration > 0.0f && m_duration > 0.0f) {
        float fade = 1.0f - clamp((m_duration - m_maxDuration + 2.0f) / 2.0f, 0.0f, 1.0f);
        currentIntensity *= fade;
    }
    currentIntensity *= calculateStateIntensityMultiplier();
    vec4 beamColor = getConnectionTypeColor();
    beamColor = lerpVec4(beamColor, getConnectionStateColor(), 0.4f);
    beamColor.w *= currentIntensity;
    context.setColor(beamColor);
    context.setEmission(make_vec3(beamColor.x, beamColor.y, beamColor.z) * currentIntensity);
    m_beamMesh->render(context);
}

void ConnectionBeam::renderFlowEffects(RenderContext&) {
    // Particle systems render themselves. Render additional indicators if needed.
}

void ConnectionBeam::renderFlowIndicators(RenderContext& context) {
    int indicatorCount = static_cast<int>(3 + m_bandwidth);
    for (int i = 0; i < indicatorCount; ++i) {
        float offset = (i / float(indicatorCount)) + m_flowPhase * 0.1f;
        offset = fmod(offset, 1.0f);
        if (m_flowDirection < 0.0f)
            offset = 1.0f - offset;
        vec3 pos = calculateBeamPosition(offset);
        context.pushTransform(mat4_identity());
        context.translate(pos);
        float indicatorIntensity = m_intensity * 0.8f;
        vec4 color = getConnectionTypeColor();
        color.w *= indicatorIntensity;
        context.setColor(color);
        float size = 0.02f + (m_thickness * 2.0f);
        context.drawGlowSphere(size);
        context.popTransform();
    }
}

void ConnectionBeam::renderDataPackets(RenderContext& context) {
    for (const auto& packet : m_dataPackets) {
        if (packet.life > 0.0f) {
            context.pushTransform(mat4_identity());
            context.translate(packet.position);
            float alpha = clamp(packet.life / packet.maxLife, 0.0f, 1.0f);
            vec4 packetColor = make_vec4(packet.color.x, packet.color.y, packet.color.z, alpha);
            context.setColor(packetColor);
            context.drawGlowSphere(packet.size);
            context.setColor(make_vec4(packetColor.x, packetColor.y, packetColor.z, alpha * 0.3f));
            context.drawGlowSphere(packet.size * 1.5f);
            context.popTransform();
        }
    }
}

void ConnectionBeam::renderGlowEffect(RenderContext& context) {
    float glowIntensity = m_intensity * 0.3f;
    if (glowIntensity > 0.1f) {
        vec4 glowColor = getConnectionTypeColor();
        glowColor.w = glowIntensity / m_glowFalloff;
        context.setColor(glowColor);
        context.setBlendMode(BlendMode::ADDITIVE);
        if (m_beamMesh) {
            std::vector<vec3> centerLine;
            for (int i = 0; i <= m_segments; ++i) {
                float t = i / float(m_segments);
                vec3 p = lerp(m_startPosition, m_endPosition, t);
                centerLine.push_back(p);
            }
            auto glowMesh = std::make_shared<BeamMesh>();
            glowMesh->setSegmentCount(m_segments);
            glowMesh->updateCenterLine(centerLine, m_thickness * 3.0f);
            glowMesh->render(context);
        }
    }
}

vec3 ConnectionBeam::calculateBeamPosition(float t) const {
    vec3 basePos = lerp(m_startPosition, m_endPosition, t);
    vec3 direction = normalize(m_endPosition - m_startPosition);
    float length = magnitude(m_endPosition - m_startPosition);
    if (m_connectionType == ConnectionType::DATA_FLOW || m_connectionType == ConnectionType::STREAM) {
        float curvature = sin(t * M_PI) * length * 0.02f;
        vec3 perpendicular = cross(direction, make_vec3(0, 1, 0));
        if (magnitude(perpendicular) < 0.1f)
            perpendicular = cross(direction, make_vec3(1, 0, 0));
        perpendicular = normalize(perpendicular);
        basePos = basePos + perpendicular * curvature;
    }
    return basePos;
}

vec3 ConnectionBeam::calculateFlowTurbulence(const vec3& pos, float time) const {
    float x = sin(pos.x * 0.3f + time * 2.0f) * cos(pos.y * 0.25f + time * 1.5f);
    float y = cos(pos.y * 0.3f + time * 1.8f) * sin(pos.z * 0.22f + time * 2.2f);
    float z = sin(pos.z * 0.3f + time * 1.9f) * cos(pos.x * 0.28f + time * 1.7f);
    return make_vec3(x, y, z) * 0.1f;
}

vec4 ConnectionBeam::getConnectionTypeColor() const {
    switch (m_connectionType) {
        case ConnectionType::DATA_FLOW:      return make_vec4(0.4f, 0.8f, 1.0f, 0.8f);
        case ConnectionType::QUERY:          return make_vec4(0.8f, 0.4f, 1.0f, 0.8f);
        case ConnectionType::API_CALL:       return make_vec4(0.2f, 1.0f, 0.4f, 0.8f);
        case ConnectionType::STREAM:         return make_vec4(1.0f, 0.6f, 0.2f, 0.8f);
        case ConnectionType::BLOCKCHAIN:     return make_vec4(1.0f, 0.8f, 0.2f, 0.8f);
        case ConnectionType::AI_INFERENCE:   return make_vec4(1.0f, 0.2f, 0.8f, 0.8f);
        case ConnectionType::AUDIO_SIGNAL:   return make_vec4(0.6f, 1.0f, 0.8f, 0.8f);
        default:                             return make_vec4(0.6f, 0.6f, 0.6f, 0.8f);
    }
}

vec4 ConnectionBeam::getConnectionStateColor() const {
    switch (m_connectionState) {
        case ConnectionState::ESTABLISHING: return make_vec4(0.8f, 0.8f, 0.2f, 0.6f);
        case ConnectionState::ACTIVE:       return make_vec4(0.2f, 1.0f, 0.4f, 0.8f);
        case ConnectionState::HIGH_LOAD:    return make_vec4(1.0f, 0.6f, 0.2f, 0.9f);
        case ConnectionState::ERROR:        return make_vec4(1.0f, 0.2f, 0.2f, 1.0f);
        case ConnectionState::TIMEOUT:      return make_vec4(0.8f, 0.4f, 0.2f, 0.7f);
        case ConnectionState::CLOSING:      return make_vec4(0.6f, 0.6f, 0.6f, 0.4f);
        default:                            return make_vec4(0.5f, 0.5f, 0.5f, 0.6f);
    }
}

float ConnectionBeam::calculateStateIntensityMultiplier() const {
    switch (m_connectionState) {
        case ConnectionState::ESTABLISHING: return 0.6f + 0.4f * sin(m_flowPhase * 3.0f);
        case ConnectionState::ACTIVE:       return 1.0f;
        case ConnectionState::HIGH_LOAD:    return 1.2f + 0.3f * sin(m_flowPhase * 5.0f);
        case ConnectionState::ERROR:        return 0.8f + 0.7f * sin(m_flowPhase * 8.0f);
        case ConnectionState::TIMEOUT:      return 0.4f;
        case ConnectionState::CLOSING:      return std::max(0.1f, 1.0f - (m_duration / std::max(m_maxDuration, 1.0f)));
        default:                            return 0.5f;
    }
}

vec4 ConnectionBeam::lerpVec4(const vec4& a, const vec4& b, float t) const {
    return make_vec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t));
}

void ConnectionBeam::updateBeamMesh() {
    if (!m_beamMesh) return;
    vec3 direction = m_endPosition - m_startPosition;
    float length = magnitude(direction);
    vec3 normalizedDir = normalize(direction);
    std::vector<vec3> centerLine;
    for (int i = 0; i <= m_segments; ++i) {
        float t = i / float(m_segments);
        vec3 point = lerp(m_startPosition, m_endPosition, t);
        if (m_connectionType == ConnectionType::DATA_FLOW || m_connectionType == ConnectionType::STREAM) {
            float curvature = sin(t * M_PI) * length * 0.02f;
            vec3 perpendicular = cross(normalizedDir, make_vec3(0, 1, 0));
            if (magnitude(perpendicular) < 0.1f) perpendicular = cross(normalizedDir, make_vec3(1, 0, 0));
            perpendicular = normalize(perpendicular);
            point = point + perpendicular * curvature;
        }
        if (m_holographicNoise > 0.0f) {
            vec3 noise = make_vec3(
                sin(t * 20.0f + m_hologramPhase) * 0.02f,
                cos(t * 15.0f + m_hologramPhase * 1.3f) * 0.015f,
                sin(t * 25.0f + m_hologramPhase * 0.8f) * 0.018f);
            point = point + noise * m_holographicNoise;
        }
        centerLine.push_back(point);
    }
    m_beamMesh->updateCenterLine(centerLine, m_thickness);
}

void ConnectionBeam::updateFlowLine() {
    if (m_flowParticles) {
        m_flowParticles->setEmitLine(m_startPosition, m_endPosition);
        vec3 dir = normalize(m_endPosition - m_startPosition);
        if (m_flowDirection < 0.0f) dir = -dir;
        m_flowParticles->setEmitDirection(dir);
    }
    if (m_glowParticles) {
        m_glowParticles->setEmitLine(m_startPosition, m_endPosition);
        vec3 dir = normalize(m_endPosition - m_startPosition);
        if (m_flowDirection < 0.0f) dir = -dir;
        m_glowParticles->setEmitDirection(dir);
    }
}

void ConnectionBeam::updateMaterialProperties() {
    if (!m_beamMaterial) return;
    vec4 typeColor = getConnectionTypeColor();
    vec4 stateColor = getConnectionStateColor();
    vec4 finalColor = lerpVec4(typeColor, stateColor, 0.6f);
    finalColor.w *= m_intensity;
    m_beamMaterial->setAlbedo(make_vec3(finalColor.x, finalColor.y, finalColor.z));
    m_beamMaterial->setEmission(make_vec3(finalColor.x, finalColor.y, finalColor.z));
    m_beamMaterial->setEmissionStrength(m_intensity);
    m_beamMaterial->setAlpha(finalColor.w);
}

} // namespace FinalStorm

