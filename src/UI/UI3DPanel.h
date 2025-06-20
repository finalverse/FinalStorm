#pragma once

#include <memory>
#include "Core/Math/MathTypes.h"

namespace FinalStorm {

class Mesh;
class Material;
class Renderer;
class Transform;

class UI3DPanel {
public:
    UI3DPanel();
    ~UI3DPanel();
    
    // Initialization
    void init(float width, float height);
    
    // Properties
    void setPanelColor(const vec4& color);
    void setOpacity(float opacity);
    bool getVisible() const;
    void setVisible(bool visible);
    
    // Transform access
    Transform* getTransform() const { return m_transform.get(); }
    
    // Update and render
    void update(float deltaTime);
    void draw(Renderer* renderer);
    
private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    std::unique_ptr<Transform> m_transform;
    bool m_visible = true;
    float m_width = 1.0f;
    float m_height = 1.0f;
};

} // namespace FinalStorm