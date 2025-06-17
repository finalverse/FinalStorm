#pragma once
#include "../Core/World/Entity.h"
#include <string>
#include <functional>

namespace FinalStorm {

class UI3DPanel : public Entity {
public:
    UI3DPanel(float width, float height);
    
    void setText(const std::string& text);
    void setBackgroundColor(const glm::vec4& color);
    void setBorderColor(const glm::vec4& color);
    
    void update(float deltaTime) override;
    void render(class MetalRenderer* renderer) override;
    
    // Interaction
    bool hitTest(const glm::vec3& rayOrigin, const glm::vec3& rayDir);
    std::function<void()> onPress;
    std::function<void()> onHover;
    
private:
    float m_width, m_height;
    std::string m_text;
    glm::vec4 m_backgroundColor;
    glm::vec4 m_borderColor;
    bool m_isHovered = false;
};

} // namespace FinalStorm
