#pragma once

#include "SceneNode.h"

namespace FinalStorm {

// Simple node type used for non-rendered services (e.g., logic or audio)
class ServiceNode : public SceneNode
{
public:
    ServiceNode() = default;
    void render(Renderer* renderer) override {} // no drawing
};

} // namespace FinalStorm
