// Rendering/Material.h
#pragma once

#include "Core/MathTypes.h"
#include "Core/ResourceManager.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace FinalStorm {

class Texture;
class Shader;

enum class MaterialType {
    Standard,
    Unlit,
    Holographic,
    Particle,
    UI
};

class Material : public Resource {
public:
    Material(MaterialType type = MaterialType::Standard) 
        : m_type(type)
        , m_albedo(1.0f, 1.0f, 1.0f, 1.0f)
        , m_emissive(0.0f, 0.0f, 0.0f)
        , m_metallic(0.0f)
        , m_roughness(1.0f)
        , m_opacity(1.0f) {}
    
    // Resource interface
    bool load(const std::string& path) override;
    void unload() override;
    bool isLoaded() const override { return m_loaded; }
    
    // Material properties
    MaterialType getType() const { return m_type; }
    void setType(MaterialType type) { m_type = type; }
    
    // Colors
    const vec4& getAlbedo() const { return m_albedo; }
    void setAlbedo(const vec4& color) { m_albedo = color; }
    
    const vec3& getEmissive() const { return m_emissive; }
    void setEmissive(const vec3& color) { m_emissive = color; }
    
    // PBR properties
    float getMetallic() const { return m_metallic; }
    void setMetallic(float metallic) { m_metallic = metallic; }
    
    float getRoughness() const { return m_roughness; }
    void setRoughness(float roughness) { m_roughness = roughness; }
    
    float getOpacity() const { return m_opacity; }
    void setOpacity(float opacity) { m_opacity = opacity; }
    
    // Textures
    void setTexture(const std::string& name, std::shared_ptr<Texture> texture) {
        m_textures[name] = texture;
    }
    
    std::shared_ptr<Texture> getTexture(const std::string& name) const {
        auto it = m_textures.find(name);
        return (it != m_textures.end()) ? it->second : nullptr;
    }
    
    // Shader
    void setShader(std::shared_ptr<Shader> shader) { m_shader = shader; }
    std::shared_ptr<Shader> getShader() const { return m_shader; }
    
    // Custom properties for special effects
    void setFloat(const std::string& name, float value) {
        m_floatProperties[name] = value;
    }
    
    float getFloat(const std::string& name, float defaultValue = 0.0f) const {
        auto it = m_floatProperties.find(name);
        return (it != m_floatProperties.end()) ? it->second : defaultValue;
    }
    
    void setVector(const std::string& name, const vec4& value) {
        m_vectorProperties[name] = value;
    }
    
    vec4 getVector(const std::string& name, const vec4& defaultValue = vec4(0)) const {
        auto it = m_vectorProperties.find(name);
        return (it != m_vectorProperties.end()) ? it->second : defaultValue;
    }
    
    // Rendering states
    bool isTransparent() const { return m_opacity < 1.0f || m_type == MaterialType::Holographic; }
    bool isDoubleSided() const { return m_doubleSided; }
    void setDoubleSided(bool doubleSided) { m_doubleSided = doubleSided; }
    
private:
    MaterialType m_type;
    
    // Basic properties
    vec4 m_albedo;
    vec3 m_emissive;
    float m_metallic;
    float m_roughness;
    float m_opacity;
    bool m_doubleSided = false;
    
    // Resources
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    
    // Custom properties
    std::unordered_map<std::string, float> m_floatProperties;
    std::unordered_map<std::string, vec4> m_vectorProperties;
};

// Factory function
inline std::shared_ptr<Material> createMaterial(MaterialType type = MaterialType::Standard) {
    return std::make_shared<Material>(type);
}

} // namespace FinalStorm