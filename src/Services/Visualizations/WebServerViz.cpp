#include "Services/Visualizations/WebServerViz.h"
#include "Scene/SceneNode.h"

namespace FinalStorm {

WebServerViz::WebServerViz()
    : m_statusText("starting") {}

void WebServerViz::createVisualization()
{
    // Placeholder: create child nodes representing the web server
    // In real implementation, this might load meshes or UI elements
}

void WebServerViz::updateMetrics(const std::string& metrics)
{
    m_statusText = metrics;
}

} // namespace FinalStorm
