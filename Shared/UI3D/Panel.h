#pragma once

#include <functional>
#include <string>
#include "../Core/Math/Math.h"

namespace FinalStorm {

class Renderer;

class Panel {
public:
    using ActivateCallback = std::function<void()>;

    Panel(uint32_t width, uint32_t height);

    void setTextureName(const std::string& name) { m_textureName = name; }
    const std::string& getTextureName() const { return m_textureName; }

    void render(Renderer* renderer);
    void activate();
    void setActivateCallback(ActivateCallback callback) { m_onActivate = callback; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

private:
    uint32_t m_width;
    uint32_t m_height;
    std::string m_textureName;
    ActivateCallback m_onActivate;
};

} // namespace FinalStorm
