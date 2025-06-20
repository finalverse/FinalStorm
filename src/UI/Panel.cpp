// src/UI/Panel.cpp
// Basic UI panel implementation
// Foundation for 2D UI elements in 3D space

#include "UI/Panel.h"
#include "Rendering/RenderContext.h"

namespace FinalStorm {

Panel::Panel(float w, float h)
    : SceneNode("Panel")
    , width(w)
    , height(h)
    , backgroundColor(0.1f, 0.1f, 0.2f, 0.8f)
    , borderColor(0.2f, 0.8f, 1.0f, 1.0f)
    , showBorder(true) {
}

Panel::~Panel() = default;

void Panel::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Draw background
    context.setColor(backgroundColor);
    context.drawQuad(width, height);
    
    // Draw border if enabled
    if (showBorder) {
        context.setColor(borderColor);
        context.drawWireframeQuad(width, height);
    }
    
    context.popTransform();
}

} // namespace FinalStorm