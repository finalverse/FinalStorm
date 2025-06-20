// src/Rendering/Metal/MetalMesh.h
// Metal mesh interface
// Manages vertex and index buffers

#pragma once
#include "Rendering/Mesh.h"
#include <memory>

namespace FinalStorm {

struct MetalMeshImpl;

class MetalMesh : public Mesh {
public:
    MetalMesh();
    ~MetalMesh() override;
    
    void uploadVertices(const void* data, size_t size, uint32_t count) override;
    void uploadIndices(const void* data, size_t size, uint32_t count) override;
    
    uint32_t getVertexCount() const override;
    uint32_t getIndexCount() const override;
    
    // Metal-specific
    void* getVertexBuffer() const;
    void* getIndexBuffer() const;
    
private:
    std::unique_ptr<MetalMeshImpl> impl;
};

} // namespace FinalStorm