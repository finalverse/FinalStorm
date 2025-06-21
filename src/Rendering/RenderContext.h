// src/Rendering/RenderContext.h
// Render context interface
// Provides drawing commands for a frame

#pragma once
#include "Core/Math/MathTypes.h"
#include <stack>

namespace FinalStorm {

class Camera;
class Mesh;

class RenderContext {
public:
    virtual ~RenderContext() = default;
    
    // Camera
    virtual void setCamera(const Camera& camera) = 0;
    
    // Transform stack
    virtual void pushTransform(const mat4& transform) = 0;
    virtual void popTransform() = 0;
    virtual void translate(const vec3& translation) = 0;
    virtual void rotate(const quat& rotation) = 0;
    virtual void scale(const vec3& scale) = 0;
    
    // State
    virtual void setColor(const vec4& color) = 0;
    
    // Drawing
    virtual void drawMesh(Mesh* mesh) = 0;
    virtual void drawCube(float size) = 0;
    virtual void drawSphere(float radius, int segments = 16) = 0;
    virtual void drawQuad(float width, float height) = 0;
    virtual void drawWireframeQuad(float width, float height) = 0;
    virtual void drawGrid(float size, float spacing) = 0;
    virtual void drawSkybox() = 0;
};

} // namespace FinalStorm