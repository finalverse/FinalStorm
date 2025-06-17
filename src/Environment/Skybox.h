#pragma once
#include "Core/World/Entity.h"

namespace FinalStorm {

class Skybox : public Entity {
public:
    Skybox();
    
    void setTint(const glm::vec3& tint);
    void setCubemap(const std::string& path);
    
    void update(float deltaTime) override;
    void render(class MetalRenderer* renderer) override;
    
private:
    glm::vec3 m_tint;
    std::string m_cubemapPath;
    uint32_t m_cubemapTexture = 0;
};

} // namespace FinalStorm
