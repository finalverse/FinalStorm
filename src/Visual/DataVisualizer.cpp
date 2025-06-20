// src/Visual/DataVisualizer.cpp
// Data visualization implementation
// Converts data to visual representations

#include "Visual/DataVisualizer.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

DataVisualizer::DataVisualizer()
    : animationTime(0.0f) {
}

DataVisualizer::~DataVisualizer() = default;

void DataVisualizer::update(float deltaTime) {
    animationTime += deltaTime;
}

void DataVisualizer::visualizeTimeSeries(RenderContext& context, 
                                        const std::vector<float>& data,
                                        const float3& position,
                                        const float3& size) {
    if (data.empty()) return;
    
    context.pushTransform(float4x4(1.0f));
    context.translate(position);
    
    // Find min/max for normalization
    float minVal = *std::min_element(data.begin(), data.end());
    float maxVal = *std::max_element(data.begin(), data.end());
    float range = maxVal - minVal;
    
    if (range < 0.001f) range = 1.0f;
    
    // Draw data points
    float stepX = size.x / (data.size() - 1);
    for (size_t i = 0; i < data.size(); ++i) {
        float normalizedValue = (data[i] - minVal) / range;
        float x = -size.x * 0.5f + i * stepX;
        float y = normalizedValue * size.y;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(x, y, 0));
        
        // Color based on value
        float4 color = float4(
            normalizedValue,
            1.0f - normalizedValue,
            0.5f,
            0.8f
        );
        context.setColor(color);
        context.drawSphere(0.05f);
        
        context.popTransform();
    }
    
    context.popTransform();
}

void DataVisualizer::visualizeBarChart(RenderContext& context,
                                      const std::vector<float>& data,
                                      const float3& position,
                                      const float3& size) {
    if (data.empty()) return;
    
    context.pushTransform(float4x4(1.0f));
    context.translate(position);
    
    float barWidth = size.x / data.size();
    float maxVal = *std::max_element(data.begin(), data.end());
    
    for (size_t i = 0; i < data.size(); ++i) {
        float barHeight = (data[i] / maxVal) * size.y;
        float x = -size.x * 0.5f + barWidth * (i + 0.5f);
        float y = barHeight * 0.5f;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(x, y, 0));
        
        // Animated glow
        float glow = 0.7f + 0.3f * sinf(animationTime * 2.0f + i * 0.5f);
        context.setColor(float4(0.2f, 0.8f, 1.0f, 0.8f) * glow);
        
        context.scale(float3(barWidth * 0.8f, barHeight, 0.1f));
        context.drawCube(1.0f);
        
        context.popTransform();
    }
    
    context.popTransform();
}

} // namespace FinalStorm