// UI/UI3DPanel.cpp
#include "UI3DPanel.h"
#include "Core/ResourceManager.h"      // You need to add this
#include "Core/Math/Transform.h"       // Existing
#include "Renderer/Material.h"         // You need to add this
#include "Renderer/Mesh.h"            // You need to add this
#include "Renderer/MetalRenderer.h"    // Use existing MetalRenderer instead of generic Renderer

namespace FinalStorm {

UI3DPanel::UI3DPanel(float width, float height)
    : m_width(width)
    , m_height(height)
    , m_backgroundColor(0.1f, 0.1f, 0.1f, 0.9f)
    , m_borderColor(0.0f, 0.8f, 1.0f, 1.0f)
    , m_borderWidth(2.0f)
    , m_cornerRadius(10.0f) {
    
    createPanelMesh();
    setupMaterial();
}

UI3DPanel::~UI3DPanel() {
    // Cleanup handled by smart pointers
}

void UI3DPanel::createPanelMesh() {
    // Create vertices for a simple quad
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    float halfWidth = m_width * 0.5f;
    float halfHeight = m_height * 0.5f;
    
    // Using existing math functions from MathTypes.h
    vertices.push_back(Vertex(make_vec3(-halfWidth, -halfHeight, 0), vec3_up(), vec2(0, 0)));
    vertices.push_back(Vertex(make_vec3(halfWidth, -halfHeight, 0), vec3_up(), vec2(1, 0)));
    vertices.push_back(Vertex(make_vec3(halfWidth, halfHeight, 0), vec3_up(), vec2(1, 1)));
    vertices.push_back(Vertex(make_vec3(-halfWidth, halfHeight, 0), vec3_up(), vec2(0, 1)));
    
    indices = {0, 1, 2, 0, 2, 3};
    
    // Create mesh (you'll need to implement MetalMesh or a mesh factory)
    m_mesh = std::make_shared<MetalMesh>();
    m_mesh->setVertices(vertices);
    m_mesh->setIndices(indices);
    m_mesh->upload();
}

void UI3DPanel::setupMaterial() {
    m_material = std::make_shared<Material>(MaterialType::UI);
    m_material->setAlbedo(m_backgroundColor);
    m_material->setEmissive(make_vec3(0.1f, 0.1f, 0.1f));
    
    // UI-specific properties
    m_material->setFloat("borderWidth", m_borderWidth);
    m_material->setVector("borderColor", m_borderColor);
    m_material->setFloat("cornerRadius", m_cornerRadius);
}

void UI3DPanel::update(float deltaTime) {
    // Update any animations or dynamic properties
    
    // Update transform if needed
    updateTransform();
    
    // Update children
    for (auto& child : m_children) {
        child->update(deltaTime);
    }
}

void UI3DPanel::render(MetalRenderer* renderer) {
    if (!m_visible) return;
    
    // Set transform for this panel
    mat4 worldMatrix = getWorldMatrix();
    renderer->setModelMatrix(worldMatrix);
    
    // Render the panel mesh with material
    if (m_mesh && m_material) {
        renderer->setMaterial(m_material);
        renderer->drawMesh(m_mesh);
    }
    
    // Render children
    for (auto& child : m_children) {
        child->render(renderer);
    }
}

void UI3DPanel::setSize(float width, float height) {
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        createPanelMesh();
    }
}

void UI3DPanel::setBackgroundColor(const vec4& color) {
    m_backgroundColor = color;
    if (m_material) {
        m_material->setAlbedo(color);
    }
}

void UI3DPanel::setBorderColor(const vec4& color) {
    m_borderColor = color;
    if (m_material) {
        m_material->setVector("borderColor", color);
    }
}

void UI3DPanel::setBorderWidth(float width) {
    m_borderWidth = width;
    if (m_material) {
        m_material->setFloat("borderWidth", width);
    }
}

void UI3DPanel::setCornerRadius(float radius) {
    m_cornerRadius = radius;
    if (m_material) {
        m_material->setFloat("cornerRadius", radius);
    }
}

void UI3DPanel::setPosition(const vec3& position) {
    m_transform.position = position;
    m_transformDirty = true;
}

void UI3DPanel::setRotation(const quat& rotation) {
    m_transform.rotation = rotation;
    m_transformDirty = true;
}

void UI3DPanel::setScale(const vec3& scale) {
    m_transform.scale = scale;
    m_transformDirty = true;
}

mat4 UI3DPanel::getWorldMatrix() const {
    if (m_transformDirty) {
        updateTransform();
    }
    
    if (m_parent) {
        return m_parent->getWorldMatrix() * m_localMatrix;
    }
    
    return m_localMatrix;
}

void UI3DPanel::updateTransform() const {
    // Build transform matrix from position, rotation, scale
    m_localMatrix = translate(mat4(1.0f), m_transform.position);
    
    // Convert quaternion to matrix if needed
    // This depends on your Transform implementation
    m_localMatrix = m_localMatrix * m_transform.getMatrix();
    
    m_transformDirty = false;
}

void UI3DPanel::addChild(std::shared_ptr<UI3DPanel> child) {
    if (child && child.get() != this) {
        m_children.push_back(child);
        child->m_parent = this;
    }
}

void UI3DPanel::removeChild(std::shared_ptr<UI3DPanel> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
    }
}

vec2 UI3DPanel::worldToPanel(const vec3& worldPos) const {
    // Transform world position to panel space
    mat4 invTransform = inverse(getWorldMatrix());
    vec4 localPos = invTransform * make_vec4(worldPos, 1.0f);
    
    // Convert to 2D panel coordinates (0,0 to 1,1)
    float x = (localPos.x / m_width) + 0.5f;
    float y = (localPos.y / m_height) + 0.5f;
    
    return vec2(x, y);
}

vec3 UI3DPanel::panelToWorld(const vec2& panelPos) const {
    // Convert panel coordinates to local 3D position
    float x = (panelPos.x - 0.5f) * m_width;
    float y = (panelPos.y - 0.5f) * m_height;
    vec4 localPos = make_vec4(x, y, 0, 1);
    
    // Transform to world space
    vec4 worldPos = getWorldMatrix() * localPos;
    return vec3(worldPos.x, worldPos.y, worldPos.z);
}

bool UI3DPanel::containsPoint(const vec3& worldPoint) const {
    vec2 panelPos = worldToPanel(worldPoint);
    return panelPos.x >= 0 && panelPos.x <= 1 &&
           panelPos.y >= 0 && panelPos.y <= 1;
}

} // namespace FinalStorm