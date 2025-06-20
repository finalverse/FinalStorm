// src/Rendering/Metal/MetalTexture.mm
// Metal texture implementation
// Handles texture loading and management

#import <Metal/Metal.h>
#include "Rendering/Metal/MetalTexture.h"

namespace FinalStorm {

struct MetalTextureImpl {
    id<MTLTexture> texture;
    uint32_t width;
    uint32_t height;
};

MetalTexture::MetalTexture()
    : impl(std::make_unique<MetalTextureImpl>()) {
    impl->width = 0;
    impl->height = 0;
}

MetalTexture::~MetalTexture() = default;

void MetalTexture::upload(const void* data, uint32_t width, uint32_t height, TextureFormat format) {
    // TODO: Create texture from data
    impl->width = width;
    impl->height = height;
}

uint32_t MetalTexture::getWidth() const {
    return impl->width;
}

uint32_t MetalTexture::getHeight() const {
    return impl->height;
}

void* MetalTexture::getHandle() const {
    return (__bridge void*)impl->texture;
}

} // namespace FinalStorm