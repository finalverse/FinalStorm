//
//  Renderer.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include "../Core/Math/Math.h"
#include <memory>
#include <string>

namespace FinalStorm {

class Camera;
class WorldManager;

// Abstract renderer interface for cross-platform support
class Renderer {
public:
    virtual ~Renderer() = default;
    
    // Initialization
    virtual bool initialize(void* nativeWindowHandle) = 0;
    virtual void shutdown() = 0;
    
    // Frame rendering
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void present() = 0;
    
    // Resource management
    virtual bool loadMesh(const std::string& name, const void* vertexData, size_t vertexSize,
                         const void* indexData, size_t indexSize) = 0;
    virtual bool loadTexture(const std::string& name, const void* data,
                           uint32_t width, uint32_t height, uint32_t channels) = 0;
    
    // Rendering
    virtual void clear(const float4& color) = 0;
    virtual void setViewProjectionMatrix(const float4x4& matrix) = 0;
    virtual void drawMesh(const std::string& meshName, const float4x4& modelMatrix) = 0;
    
    // Camera and world
    virtual void setCamera(std::shared_ptr<Camera> camera) = 0;
    virtual void setWorldManager(std::shared_ptr<WorldManager> world) = 0;
       
       // Window events
       virtual void onResize(uint32_t width, uint32_t height) = 0;
       
       // Debug rendering
       virtual void drawLine(const float3& start, const float3& end, const float4& color) = 0;
       virtual void drawBox(const float3& min, const float3& max, const float4& color) = 0;
       
    protected:
       std::shared_ptr<Camera> m_camera;
       std::shared_ptr<WorldManager> m_worldManager;
    };

    // Renderer factory
    class RendererFactory {
    public:
       enum class RendererType {
           Metal,
           Vulkan,
           DirectX12
       };
       
       static std::unique_ptr<Renderer> create(RendererType type);
    };

    } // namespace FinalStorm
