#include "UI/HolographicDisplay.h"
#include "Renderer/Renderer.h"

namespace FinalStorm {

HolographicDisplay::HolographicDisplay(uint32_t width, uint32_t height)
    : m_panel(std::make_unique<Panel>(width, height)) {}

void HolographicDisplay::onRender(Renderer* renderer)
{
    if (m_panel)
        m_panel->render(renderer);
}

} // namespace FinalStorm
