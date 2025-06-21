#include "Services/Components/ConnectionBeam.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include <algorithm>
#include <cmath>

namespace FinalStorm {

BeamMesh::BeamMesh()
    : m_segmentCount(32), m_isDirty(true), m_vertexBuffer(nullptr), m_indexBuffer(nullptr) {}

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
    int radialSegments = 8;
    float radius = thickness * 0.5f;
    for (size_t i = 0; i < centerLine.size(); ++i) {
        vec3 center = centerLine[i];
        vec3 tangent = calculateTangent(centerLine, static_cast<int>(i));
        vec3 binormal = calculateBinormal(tangent);
        vec3 normal = cross(tangent, binormal);
        float v = static_cast<float>(i) / (centerLine.size() - 1);
        for (int j = 0; j < radialSegments; ++j) {
            float angle = (j / float(radialSegments)) * 2.0f * M_PI;
            float cosA = cos(angle);
            float sinA = sin(angle);
            vec3 offset = (binormal * cosA + normal * sinA) * radius;
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
    for (size_t i = 0; i < centerLine.size(); ++i) {
        vec3 center = centerLine[i];
        vec3 tangent = calculateTangent(centerLine, static_cast<int>(i));
        vec3 binormal = calculateBinormal(tangent, make_vec3(0, 1, 0));
        float v = static_cast<float>(i) / (centerLine.size() - 1);
        vec3 leftVertex = center - binormal * halfWidth;
        m_vertices.push_back(leftVertex);
        m_normals.push_back(make_vec3(0, 0, 1));
        m_uvs.push_back(make_vec2(0.0f, v));
        vec3 rightVertex = center + binormal * halfWidth;
        m_vertices.push_back(rightVertex);
        m_normals.push_back(make_vec3(0, 0, 1));
        m_uvs.push_back(make_vec2(1.0f, v));
    }
    for (size_t i = 0; i < centerLine.size() - 1; ++i) {
        uint32_t base = static_cast<uint32_t>(i * 2);
        m_indices.push_back(base);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);
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
            m_indices.push_back(current);
            m_indices.push_back(next);
            m_indices.push_back(currentNext);
            m_indices.push_back(currentNext);
            m_indices.push_back(next);
            m_indices.push_back(nextNext);
        }
    }
}

void BeamMesh::updateBuffers() {
    // Placeholder for real buffer update
}

vec3 BeamMesh::calculateTangent(const std::vector<vec3>& centerLine, int index) const {
    if (centerLine.size() < 2) return make_vec3(1, 0, 0);
    if (index == 0) {
        return normalize(centerLine[1] - centerLine[0]);
    } else if (index == static_cast<int>(centerLine.size()) - 1) {
        return normalize(centerLine[index] - centerLine[index - 1]);
    } else {
        vec3 t1 = centerLine[index] - centerLine[index - 1];
        vec3 t2 = centerLine[index + 1] - centerLine[index];
        return normalize(t1 + t2);
    }
}

vec3 BeamMesh::calculateBinormal(const vec3& tangent, const vec3& up) const {
    vec3 binormal = cross(tangent, up);
    if (magnitude(binormal) < 0.1f) {
        binormal = cross(tangent, make_vec3(1, 0, 0));
        if (magnitude(binormal) < 0.1f) {
            binormal = cross(tangent, make_vec3(0, 0, 1));
        }
    }
    return normalize(binormal);
}

} // namespace FinalStorm

