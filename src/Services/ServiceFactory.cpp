// ServiceFactory.cpp - Implementation of service factory

#include "Services/ServiceFactory.h"
#include "Services/Visual/APIGatewayViz.h"
#include "Services/Visual/AIServiceViz.h"
#include "Services/Visual/AudioServiceViz.h"
#include "Services/Visual/WorldEngineViz.h"
#include "Services/Visual/DatabaseViz.h"
#include "Services/Visual/CommunityViz.h"

namespace FinalStorm {

std::unique_ptr<ServiceEntity> ServiceFactory::createService(ServiceEntity::ServiceType type) {
    return createService(ServiceEntity::ServiceInfo::fromType(type));
}

std::unique_ptr<ServiceEntity> ServiceFactory::createService(const ServiceEntity::ServiceInfo& info) {
    switch (info.type) {
        case ServiceEntity::ServiceType::API_GATEWAY:
            return std::make_unique<APIGatewayViz>();
            
        case ServiceEntity::ServiceType::AI_ORCHESTRA:
        case ServiceEntity::ServiceType::BEHAVIOR_AI:
            return std::make_unique<AIServiceViz>(info.type);
            
        case ServiceEntity::ServiceType::SONG_ENGINE:
        case ServiceEntity::ServiceType::HARMONY_SERVICE:
        case ServiceEntity::ServiceType::ECHO_ENGINE:
        case ServiceEntity::ServiceType::SILENCE_SERVICE:
            return std::make_unique<AudioServiceViz>(info.type);
            
        case ServiceEntity::ServiceType::WORLD_ENGINE:
        case ServiceEntity::ServiceType::PROCEDURAL_GEN:
            return std::make_unique<WorldEngineViz>();
            
        case ServiceEntity::ServiceType::DATABASE:
            return std::make_unique<DatabaseViz>();
            
        case ServiceEntity::ServiceType::COMMUNITY:
            return std::make_unique<CommunityViz>();
            
        case ServiceEntity::ServiceType::ASSET_SERVICE:
        default:
            return std::make_unique<ServiceEntity>(info);
    }
}

} // namespace FinalStorm
