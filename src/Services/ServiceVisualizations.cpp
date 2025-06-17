#include "Services/ServiceVisualizations.h"

namespace FinalStorm {

ServiceVisualization::ServiceVisualization(const ServiceInfo& info)
    : ServiceNode(info)
{
    (void)info;
}

void ServiceVisualization::createBaseStructure() {}
void ServiceVisualization::createActivityIndicators() {}
void ServiceVisualization::createInfoDisplay() {}
void ServiceVisualization::updateVisualComponents() {}

WebServerVisualization::WebServerVisualization(const ServiceInfo& info)
    : ServiceVisualization(info), rotationSpeed(0.0f) {}

void WebServerVisualization::createBaseStructure() {}
void WebServerVisualization::createActivityIndicators() {}
void WebServerVisualization::onUpdate(float) {}
void WebServerVisualization::createConnectionBeam(const float3&) {}
void WebServerVisualization::updateConnectionBeams() {}

DatabaseVisualization::DatabaseVisualization(const ServiceInfo& info)
    : ServiceVisualization(info), numLayers(0) {}

void DatabaseVisualization::createBaseStructure() {}
void DatabaseVisualization::createActivityIndicators() {}
void DatabaseVisualization::onUpdate(float) {}
void DatabaseVisualization::createIndexStructures() {}

BlockchainVisualization::BlockchainVisualization(const ServiceInfo& info)
    : ServiceVisualization(info), numBlocks(0), blockSpacing(0.0f) {}

void BlockchainVisualization::createBaseStructure() {}
void BlockchainVisualization::createActivityIndicators() {}
void BlockchainVisualization::onUpdate(float) {}
void BlockchainVisualization::createChainLink(std::shared_ptr<MeshNode>, std::shared_ptr<MeshNode>) {}
void BlockchainVisualization::createConsensusPulses() {}
void BlockchainVisualization::triggerConsensusPulse() {}
void BlockchainVisualization::updateChainLinks(float) {}

AIServiceVisualization::AIServiceVisualization(const ServiceInfo& info)
    : ServiceVisualization(info) {}

void AIServiceVisualization::createBaseStructure() {}
void AIServiceVisualization::createActivityIndicators() {}
void AIServiceVisualization::onUpdate(float) {}
void AIServiceVisualization::createLayerConnections(const std::vector<std::shared_ptr<MeshNode>>&, const std::vector<std::shared_ptr<MeshNode>>&) {}
void AIServiceVisualization::updateSynapses(float) {}

std::shared_ptr<ServiceVisualization> ServiceVisualizationFactory::create(const ServiceNode::ServiceInfo& info)
{
    (void)info;
    return nullptr;
}

ServiceDiscoveryUI::ServiceDiscoveryUI() : expanded(false), animationTime(0.0f) {}

void ServiceDiscoveryUI::setAvailableServices(const std::vector<ServiceNode::ServiceInfo>& services)
{
    (void)services;
}

void ServiceDiscoveryUI::onUpdate(float) {}

void ServiceDiscoveryUI::createDiscoveryOrb() {}
void ServiceDiscoveryUI::toggleExpanded() {}
void ServiceDiscoveryUI::createServiceRing() {}
void ServiceDiscoveryUI::refreshServiceRing() {}
std::shared_ptr<SceneNode> ServiceDiscoveryUI::createServiceIcon(const ServiceNode::ServiceInfo& info)
{
    (void)info;
    return nullptr;
}

FinalverseEnvironment::FinalverseEnvironment() {}

void FinalverseEnvironment::updateSystemHealth(float) {}
void FinalverseEnvironment::setNetworkActivity(float) {}
void FinalverseEnvironment::onUpdate(float) {}

void FinalverseEnvironment::createSkybox() {}
void FinalverseEnvironment::createGroundGrid() {}
void FinalverseEnvironment::createAmbientEffects() {}
void FinalverseEnvironment::createDataMotes() {}
void FinalverseEnvironment::updateAmbientEffects(float) {}

} // namespace FinalStorm

