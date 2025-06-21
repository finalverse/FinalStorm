#pragma once
#include "Scene/SceneNode.h"
#include <glm/glm.hpp>

namespace FinalStorm {

class ParticleEmitter : public SceneNode {
public:
    enum class Shape {
        POINT,
        SPHERE,
        BOX,
        LINE,
        CIRCLE,
        NOTE  // Musical note shape
    };
    
    struct Config {
        Shape emitShape = Shape::POINT;
        float emitRadius = 1.0f;
        glm::vec3 emitSize = glm::vec3(1.0f);
        float emitRate = 10.0f;
        float particleLifetime = 2.0f;
        float startSize = 0.1f;
        float endSize = 0.05f;
        glm::vec4 startColor = glm::vec4(1.0f);
        glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        float velocity = 1.0f;
        glm::vec3 gravity = glm::vec3(0, -0.5f, 0);
    };
    
    ParticleEmitter(const Config& config = Config());
    
    void update(float deltaTime) override;
    void render(class MetalRenderer* renderer) override;
    
    void burst(int count);
    void setEmitRate(float rate);
    void setEmitPosition(const glm::vec3& pos) { setPosition(pos); }
    void setEmitShape(Shape shape) { m_config.emitShape = shape; }
    void setParticleColor(const glm::vec4& color) { m_config.startColor = color; }
    void setParticleLifetime(float lifetime) { m_config.particleLifetime = lifetime; }
    void setGravity(const glm::vec3& gravity) { m_config.gravity = gravity; }
    
    const Config& getParams() const { return m_config; }
    void setParams(const Config& params) { m_config = params; }
    
private:
    Config m_config;
};

} // namespace FinalStorm
