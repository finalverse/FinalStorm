// src/Services/ServiceVisualizations.cpp
// Service visualization implementations
// Creates visual representations for different service types

#include "Services/ServiceVisualizations.h"
#include "Services/Components/ParticleEmitter.h"
#include "Rendering/RenderContext.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

// Web Server Visualization
WebServerVisualization::WebServerVisualization()
    : ServiceNode("Web Server")
    , crystalRotation(0.0f) {
    baseColor = float3(0.2f, 0.8f, 1.0f);
}

void WebServerVisualization::onUpdate(float deltaTime) {
    ServiceNode::onUpdate(deltaTime);
    crystalRotation += deltaTime * 0.5f;
}

void WebServerVisualization::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Rotate the crystal
    context.rotate(quaternion(crystalRotation, float3(0, 1, 0)));
    
    // Draw main crystal structure
    float4 color = float4(baseColor.x, baseColor.y, baseColor.z, 1.0f) * glowIntensity;
    context.setColor(color);
    
    // Draw octahedron (simplified as stretched cube for now)
    context.pushTransform(float4x4(1.0f));
    context.scale(float3(0.7f, 1.5f, 0.7f));
    context.drawCube(1.0f);
    context.popTransform();
    
    // Draw data flow particles
    for (int i = 0; i < 8; ++i) {
        float angle = (float(i) / 8.0f) * 2.0f * M_PI + animationTime;
        float x = cosf(angle) * 1.5f;
        float z = sinf(angle) * 1.5f;
        float y = sinf(animationTime * 2.0f + angle) * 0.5f;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(x, y, z));
        context.setColor(float4(1.0f, 1.0f, 1.0f, 0.5f));
        context.drawSphere(0.1f);
        context.popTransform();
    }
    
    context.popTransform();
}

// Database Visualization
DatabaseVisualization::DatabaseVisualization()
    : ServiceNode("Database")
    , diskRotation(0.0f) {
    baseColor = float3(0.8f, 0.5f, 0.2f);
}

void DatabaseVisualization::onUpdate(float deltaTime) {
    ServiceNode::onUpdate(deltaTime);
    diskRotation += deltaTime * 0.3f * activityLevel;
}

void DatabaseVisualization::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Draw stacked disks
    const int diskCount = 5;
    for (int i = 0; i < diskCount; ++i) {
        float y = (i - diskCount/2) * 0.3f;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(0, y, 0));
        context.rotate(quaternion(diskRotation + i * 0.2f, float3(0, 1, 0)));
        
        float4 color = float4(baseColor.x, baseColor.y, baseColor.z, 1.0f);
        color *= (0.5f + 0.5f * glowIntensity);
        context.setColor(color);
        
        // Draw disk (cylinder)
        context.scale(float3(1.5f, 0.1f, 1.5f));
        context.drawCube(1.0f);
        context.popTransform();
    }
    
    context.popTransform();
}

// AI Service Visualization
AIServiceVisualization::AIServiceVisualization()
    : ServiceNode("AI Service")
    , neuronPhase(0.0f) {
    baseColor = float3(0.8f, 0.2f, 0.8f);
}

void AIServiceVisualization::onUpdate(float deltaTime) {
    ServiceNode::onUpdate(deltaTime);
    neuronPhase += deltaTime * 2.0f * activityLevel;
}

void AIServiceVisualization::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Draw neural network structure
    const int layers = 3;
    const int nodesPerLayer = 4;
    
    for (int layer = 0; layer < layers; ++layer) {
        float x = (layer - 1) * 1.0f;
        
        for (int node = 0; node < nodesPerLayer; ++node) {
            float y = (node - nodesPerLayer/2 + 0.5f) * 0.8f;
            
            // Node position with slight animation
            float offset = sinf(neuronPhase + layer * 0.5f + node * 0.3f) * 0.1f;
            
            context.pushTransform(float4x4(1.0f));
            context.translate(float3(x + offset, y, 0));
            
            // Node glow based on activity
            float nodeActivity = (sinf(neuronPhase + layer + node) + 1.0f) * 0.5f;
            float4 color = float4(baseColor.x, baseColor.y, baseColor.z, 1.0f);
            color *= (0.5f + nodeActivity * 0.5f) * glowIntensity;
            context.setColor(color);
            
            context.drawSphere(0.2f);
            context.popTransform();
        }
    }
    
    // TODO: Draw connections between nodes
    
    context.popTransform();
}

// Blockchain Visualization
BlockchainVisualization::BlockchainVisualization()
    : ServiceNode("Blockchain")
    , chainOffset(0.0f) {
    baseColor = float3(1.0f, 0.6f, 0.0f);
}

void BlockchainVisualization::onUpdate(float deltaTime) {
    ServiceNode::onUpdate(deltaTime);
    chainOffset += deltaTime * 0.5f * activityLevel;
}

void BlockchainVisualization::onRender(RenderContext& context) {
    context.pushTransform(getWorldMatrix());
    
    // Draw chain of blocks
    const int blockCount = 5;
    for (int i = 0; i < blockCount; ++i) {
        float x = (i - blockCount/2) * 1.2f;
        float y = sinf(chainOffset + i * 0.5f) * 0.2f;
        
        context.pushTransform(float4x4(1.0f));
        context.translate(float3(x, y, 0));
        context.rotate(quaternion(animationTime * 0.3f + i * 0.2f, float3(1, 1, 1)));
        
        float4 color = float4(baseColor.x, baseColor.y, baseColor.z, 1.0f);
        color *= glowIntensity;
        context.setColor(color);
        
        context.drawCube(0.8f);
        context.popTransform();
    }
    
    context.popTransform();
}

} // namespace FinalStorm