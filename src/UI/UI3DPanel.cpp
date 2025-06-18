#include "UI/UI3DPanel.h"
#include "Core/ResourceManager.h"
#include "Core/Transform.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"

namespace FinalStorm {

UI3DPanel::UI3DPanel() {
    m_transform = std::make_unique<Transform>();
}

UI3DPanel::~UI3DPanel() = default;

void UI3DPanel::init(float width, float height) {
    m_width = width;
    m_height = height;
    
    // Create panel mesh
    auto& resourceManager = ResourceManager::getInstance();
    m_mesh = resourceManager.getMesh("quad");
    
    if (!m_mesh) {
        // Create a simple quad mesh if not found
        std::vector<Vertex> vertices = {
            {{-width/2, -height/2, 0}, {0, 0, 1}, {0, 0}, {1, 0, 0}, {0, 1, 0}},
            {{ width/2, -height/2, 0}, {0, 0, 1}, {1, 0}, {1, 0, 0}, {0, 1, 0}},
            {{ width/2,  height/2, 0}, {0, 0, 1}, {1, 1}, {1, 0, 0}, {0, 1, 0}},
            {{-width/2,  height/2, 0}, {0, 0, 1}, {0, 1}, {1, 0, 0}, {0, 1, 0}}
        };
        
        std::vector<uint32_t> indices = {
            0, 1, 2,
            0, 2, 3
        };
        
        m_mesh = std::make_shared<Mesh>();
        m_mesh->create(vertices, indices);
        resourceManager.addMesh("quad", m_mesh);
    }
    
    // Create material
    m_material = std::make_shared<Material>();
    m_material->setShader(resourceManager.getShader("ui"));
    m_material->setAlbedo(vec3_one());
    m_material->setOpacity(0.9f);
}

void UI3DPanel::setPanelColor(const vec4& color) {
    if (m_material) {
        m_material->setAlbedo(make_vec3(color.x, color.y, color.z));
        m_material->setOpacity(color.w);
    }
}

void UI3DPanel::setOpacity(float opacity) {
    if (m_material) {
        m_material->setOpacity(opacity);
    }
}

bool UI3DPanel::getVisible() const {
    return m_visible;
}

void UI3DPanel::setVisible(bool visible) {
    m_visible = visible;
}

void UI3DPanel::update(float deltaTime) {
    if (!m_visible) return;
    
    // Update any animations
    if (m_material) {
        // Example: subtle pulse effect
        vec3 pos = m_transform->getPosition();
        float time = pos.x * 0.1f;
        float pulse = sin(time) * 0.05f + 0.95f;
        m_material->setEmission(pulse * 0.1f);
    }
}

void UI3DPanel::draw(Renderer* renderer) {
    if (!m_visible || !m_mesh || !m_material) return;
    
    // UI panels typically render with transparency
    renderer->setBlendMode(BlendMode::Alpha);
    
    // Draw the panel mesh with material
    renderer->drawMesh(m_mesh.get(), m_material.get(), m_transform->getModelMatrix());
}

} // namespace FinalStorm