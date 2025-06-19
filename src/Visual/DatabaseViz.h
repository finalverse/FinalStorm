#pragma once

#include "Services/ServiceRepresentation.h"
#include <string>

namespace FinalStorm {

class DatabaseViz : public ServiceRepresentation {
public:
    DatabaseViz();

    void createVisualization() override;
    void updateMetrics(const std::string& metrics) override;

private:
    std::string m_loadText;
};

} // namespace FinalStorm
