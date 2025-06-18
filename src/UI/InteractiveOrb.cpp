#include "UI/InteractiveOrb.h"
#include "Renderer/Renderer.h"

namespace FinalStorm {

InteractiveOrb::InteractiveOrb()
    : m_onActivate(nullptr) {}

void InteractiveOrb::onRender(Renderer* renderer)
{
    (void)renderer; // placeholder
}

void InteractiveOrb::activate()
{
    if (m_onActivate) m_onActivate();
}

} // namespace FinalStorm
