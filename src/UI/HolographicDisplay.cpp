// src/UI/HolographicDisplay.cpp
// Holographic display implementation
// Renders data with holographic effects

#include "UI/HolographicDisplay.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"
#include <algorithm>

namespace FinalStorm {

HolographicDisplay::HolographicDisplay(float width, float height)
    : UI3DPanel(width, height)
    , animationTime(0.0f) {
    setName("Holographic Display");
    backgroundColor = float4(0.0f, 0.2f, 0.3f, 0.3f);
}

HolographicDisplay::~HolographicDisplay() = default;

void HolographicDisplay::onUpdate(float deltaTime) {
    UI3DPanel::onUpdate(deltaTime);
    animationTime += deltaTime;
}

void HolographicDisplay::onRender(RenderContext& context) {
    // Render base panel with holographic shader
    UI3DPanel::onRender(context);
    
    // Render data visualization
    if (!data.empty()) {
        context.pushTransform(getWorldMatrix());
        
        // Draw data as bars
        float barWidth = width / data.size();
        float maxValue = *std::max_element(data.begin(), data.end());
        
        for (size_t i = 0; i < data.size(); ++i) {
            float barHeight = (data[i] / maxValue) * height * 0.8f;
            float x = -width * 0.5f + barWidth * (i + 0.5f);
            float y = -height * 0.5f + barHeight * 0.5f;
            
            context.pushTransform(float4x4(1.0f));
            context.translate(float3(x, y, 0.1f));
            
            // Animated glow
            float glow = 0.5f + 0.5f * sinf(animationTime * 2.0f + i * 0.5f);
            context.setColor(float4(0.2f, 0.8f, 1.0f, 0.8f) * glow);
            context.drawQuad(barWidth * 0.8f, barHeight);
            
            context.popTransform();
        }
        
        context.popTransform();
    }
}

} // namespace FinalStorm