#pragma once
#include "World/Entity.h"

namespace FinalStorm {

class GridMesh : public Entity {
public:
    GridMesh(float size = 100.0f, int divisions = 50);
    
    void setPulseIntensity(float intensity);
    void setLineColor(const glm::vec4& color);
    
    void update(float deltaTime) override;
    void render(class MetalRenderer* renderer) override;
    
private:
    float m_size;
    int m_divisions;
    float m_pulseIntensity = 1.0f;
    float m_pulsePhase = 0.0f;
    glm::vec4 m_lineColor;
    
    void generateMesh();
};

} // namespace FinalStorm
