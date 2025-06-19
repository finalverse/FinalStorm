#include "Visual/DatabaseViz.h"
#include "Scene/SceneNode.h"

namespace FinalStorm {

DatabaseViz::DatabaseViz()
    : m_loadText("idle") {}

void DatabaseViz::createVisualization()
{
    // Placeholder for database visualization setup
}

void DatabaseViz::updateMetrics(const std::string& metrics)
{
    m_loadText = metrics;
}

} // namespace FinalStorm
