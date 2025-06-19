#pragma once

#include "Services/ServiceRepresentation.h"
#include <string>

namespace FinalStorm {

class WebServerViz : public ServiceRepresentation {
public:
    WebServerViz();

    void createVisualization() override;
    void updateMetrics(const std::string& metrics) override;

private:
    std::string m_statusText;
};

} // namespace FinalStorm
