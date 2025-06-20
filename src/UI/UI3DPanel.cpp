// src/UI/UI3DPanel.cpp
// 3D UI panel implementation
// Base class for holographic UI elements

#include "UI/UI3DPanel.h"
#include "Rendering/RenderContext.h"

namespace FinalStorm {

UI3DPanel::UI3DPanel(float w, float h)
    : SceneNode("UI Panel")
    , width(w)
    , height(h)
    , backgroundColor(0.1f, 0.1f, 0.2f, 0.8f) {
}

UI3DPanel::~UI3DPanel() = default;

void UI3DPanel::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Render panel background
    context.setColor(backgroundColor);
    context.drawQuad(width, height);
    
    // Render border
    context.setColor(float4(0.2f, 0.8f, 1.0f, 1.0f));
    context.drawWireframeQuad(width, height);
    
    context.popTransform();
}

} // namespace FinalStorm