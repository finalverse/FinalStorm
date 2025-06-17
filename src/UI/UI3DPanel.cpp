#include "UI/UI3DPanel.h"
#include "Rendering/Metal/MetalRenderer.h"

namespace FinalStorm {

UI3DPanel::UI3DPanel(float width, float height)
    : Entity("UI3DPanel"),
      m_width(width),
      m_height(height),
      m_backgroundColor(0.1f, 0.1f, 0.2f, 0.8f),
      m_borderColor(0.0f, 0.8f, 1.0f, 1.0f) {
}

void UI3DPanel::setText(const std::string& text) {
    m_text = text;
    // TODO: Update text mesh
}

void UI3DPanel::setBackgroundColor(const glm::vec4& color) {
    m_backgroundColor = color;
}

void UI3DPanel::setBorderColor(const glm::vec4& color) {
    m_borderColor = color;
}

void UI3DPanel::setAlpha(float alpha) {
    m_backgroundColor.a = alpha;
    m_borderColor.a = alpha;
}

bool UI3DPanel::isVisible() const {
    return m_isVisible;
}

void UI3DPanel::setVisible(bool visible) {
    m_isVisible = visible;
}

void UI3DPanel::update(float deltaTime) {
    Entity::update(deltaTime);
    
    if (m_isHovered) {
        transform.scale = glm::vec3(m_width * 1.05f, m_height * 1.05f, 1.0f);
    } else {
        transform.scale = glm::vec3(m_width, m_height, 1.0f);
    }
}

void UI3DPanel::render(MetalRenderer* renderer) {
    if (!m_isVisible) return;
    Entity::render(renderer);
}

bool UI3DPanel::hitTest(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
    glm::vec3 min = transform.position - transform.scale * 0.5f;
    glm::vec3 max = transform.position + transform.scale * 0.5f;
    
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t1 = (min - rayOrigin) * invDir;
    glm::vec3 t2 = (max - rayOrigin) * invDir;
    
    float tMin = glm::max(glm::max(glm::min(t1.x, t2.x), glm::min(t1.y, t2.y)), glm::min(t1.z, t2.z));
    float tMax = glm::min(glm::min(glm::max(t1.x, t2.x), glm::max(t1.y, t2.y)), glm::max(t1.z, t2.z));
    
    return tMax >= 0 && tMin <= tMax;
}

} // namespace FinalStorm
