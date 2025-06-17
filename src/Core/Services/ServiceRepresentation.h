#pragma once

#include "../../SceneGraph/ServiceNode.h"
#include <string>

namespace FinalStorm {

// Base class for visualizing running services in the scene graph
class ServiceRepresentation : public ServiceNode {
public:
    virtual ~ServiceRepresentation() = default;

    virtual void createVisualization() = 0;
    virtual void updateMetrics(const std::string& metrics) = 0;
};

} // namespace FinalStorm
