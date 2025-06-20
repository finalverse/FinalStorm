// src/Rendering/Mesh.h
// Abstract mesh interface
// Base class for platform-specific mesh implementations

#pragma once
#include <cstdint>
#include <cstddef>

namespace FinalStorm {

class Mesh {
public:
    virtual ~Mesh() = default;
    
    virtual void uploadVertices(const void* data, size_t size, uint32_t count) = 0;
    virtual void uploadIndices(const void* data, size_t size, uint32_t count) = 0;
    
    virtual uint32_t getVertexCount() const = 0;
    virtual uint32_t getIndexCount() const = 0;
};

} // namespace FinalStorm