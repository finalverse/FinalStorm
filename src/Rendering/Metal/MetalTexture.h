// src/Rendering/Metal/MetalTexture.h
// Metal texture interface
// Manages texture resources

#pragma once
#include <memory>
#include <cstdint>

namespace FinalStorm {

struct MetalTextureImpl;

enum class TextureFormat {
    RGBA8,
    RGB8,
    RG8,
    R8,
    RGBA16F,
    RGB16F,
    RG16F,
    R16F,
    RGBA32F,
    RGB32F,
    RG32F,
    R32F,
    Depth32F,
    Depth24Stencil8
};

class MetalTexture {
public:
    MetalTexture();
    ~MetalTexture();
    
    void upload(const void* data, uint32_t width, uint32_t height, TextureFormat format);
    
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    void* getHandle() const;
    
private:
    std::unique_ptr<MetalTextureImpl> impl;
};

} // namespace FinalStorm