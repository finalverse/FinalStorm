// Renderer/Mesh.h
#pragma once

#include "Core/MathTypes.h"
#include "Core/ResourceManager.h"
#include <vector>
#include <memory>

namespace FinalStorm {

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec4 color;
    
    Vertex() 
        : position(vec3_zero())
        , normal(vec3_up())
        , texCoord(0.0f, 0.0f)
        , color(vec3_one(), 1.0f) {}
    
    Vertex(const vec3& pos, const vec3& norm = vec3_up(), const vec2& uv = vec2(0, 0))
        : position(pos)
        , normal(norm)
        , texCoord(uv)
        , color(vec3_one(), 1.0f) {}
};

class Mesh : public Resource {
public:
    Mesh() = default;
    virtual ~Mesh() = default;
    
    // Resource interface
    bool load(const std::string& path) override;
    void unload() override;
    bool isLoaded() const override { return m_loaded && m_vertexBuffer != nullptr; }
    
    // Mesh data
    void setVertices(const std::vector<Vertex>& vertices) {
        m_vertices = vertices;
        m_dirty = true;
    }
    
    void setIndices(const std::vector<uint32_t>& indices) {
        m_indices = indices;
        m_dirty = true;
    }
    
    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    
    size_t getVertexCount() const { return m_vertices.size(); }
    size_t getIndexCount() const { return m_indices.size(); }
    
    // Bounds
    void calculateBounds();
    const vec3& getMinBounds() const { return m_minBounds; }
    const vec3& getMaxBounds() const { return m_maxBounds; }
    vec3 getCenter() const { return (m_minBounds + m_maxBounds) * 0.5f; }
    vec3 getSize() const { return m_maxBounds - m_minBounds; }
    
    // GPU resources
    void* getVertexBuffer() const { return m_vertexBuffer; }
    void* getIndexBuffer() const { return m_indexBuffer; }
    
    // Upload to GPU (platform-specific implementation)
    virtual bool upload() = 0;
    
    // Factory methods for common shapes
    static std::shared_ptr<Mesh> createCube(float size = 1.0f);
    static std::shared_ptr<Mesh> createSphere(float radius = 1.0f, int segments = 32);
    static std::shared_ptr<Mesh> createPlane(float width = 1.0f, float height = 1.0f);
    static std::shared_ptr<Mesh> createCylinder(float radius = 0.5f, float height = 1.0f, int segments = 32);
    static std::shared_ptr<Mesh> createQuad(float width = 1.0f, float height = 1.0f);
    
protected:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    
    vec3 m_minBounds = vec3_zero();
    vec3 m_maxBounds = vec3_zero();
    
    void* m_vertexBuffer = nullptr;
    void* m_indexBuffer = nullptr;
    
    bool m_dirty = true;
};

// Platform-specific mesh implementations
#ifdef __APPLE__
class MetalMesh : public Mesh {
public:
    MetalMesh();
    ~MetalMesh();
    
    bool upload() override;
    
private:
    void* m_device = nullptr;  // id<MTLDevice>
};
#endif

} // namespace FinalStorm