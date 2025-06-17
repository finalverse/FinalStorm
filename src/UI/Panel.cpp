#include "Panel.h"
#include "../Renderer/Renderer.h"

namespace FinalStorm {

Panel::Panel(uint32_t width, uint32_t height)
    : m_width(width), m_height(height) {}

void Panel::render(Renderer* renderer)
{
    if (!renderer) return;
    float4x4 model = matrix_identity();
    renderer->drawMesh(m_textureName, model);
}

void Panel::activate()
{
    if (m_onActivate) m_onActivate();
}

} // namespace FinalStorm
