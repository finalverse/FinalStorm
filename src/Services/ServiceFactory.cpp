// src/Services/ServiceFactory.cpp
// Service factory implementation
// Creates appropriate visualizations for different service types

#include "Services/ServiceFactory.h"
#include "Services/ServiceEntity.h"

namespace FinalStorm {

std::unique_ptr<ServiceEntity> ServiceFactory::createService(ServiceEntity::ServiceType type) {
    ServiceEntity::ServiceInfo info;
    info.type = type;
    info.isHealthy = true;
    
    switch (type) {
        case ServiceEntity::ServiceType::API_GATEWAY:
            info.name = "API Gateway";
            info.url = "http://localhost:8080";
            info.port = 8080;
            break;
        case ServiceEntity::ServiceType::WEBSOCKET_GATEWAY:
            info.name = "WebSocket Gateway";
            info.url = "ws://localhost:3000";
            info.port = 3000;
            break;
        case ServiceEntity::ServiceType::WORLD_ENGINE:
            info.name = "World Engine";
            info.url = "http://localhost:3002";
            info.port = 3002;
            break;
        case ServiceEntity::ServiceType::AI_ORCHESTRA:
            info.name = "AI Orchestra";
            info.url = "http://localhost:3004";
            info.port = 3004;
            break;
        case ServiceEntity::ServiceType::SONG_ENGINE:
            info.name = "Song Engine";
            info.url = "http://localhost:3001";
            info.port = 3001;
            break;
        case ServiceEntity::ServiceType::ECHO_ENGINE:
            info.name = "Echo Engine";
            info.url = "http://localhost:3003";
            info.port = 3003;
            break;
        case ServiceEntity::ServiceType::HARMONY_SERVICE:
            info.name = "Harmony Service";
            info.url = "http://localhost:3006";
            info.port = 3006;
            break;
        case ServiceEntity::ServiceType::ASSET_SERVICE:
            info.name = "Asset Service";
            info.url = "http://localhost:3007";
            info.port = 3007;
            break;
        case ServiceEntity::ServiceType::COMMUNITY:
            info.name = "Community Service";
            info.url = "http://localhost:3008";
            info.port = 3008;
            break;
        case ServiceEntity::ServiceType::SILENCE_SERVICE:
            info.name = "Silence Service";
            info.url = "http://localhost:3009";
            info.port = 3009;
            break;
        case ServiceEntity::ServiceType::PROCEDURAL_GEN:
            info.name = "Procedural Gen";
            info.url = "http://localhost:3010";
            info.port = 3010;
            break;
        case ServiceEntity::ServiceType::BEHAVIOR_AI:
            info.name = "Behavior AI";
            info.url = "http://localhost:3011";
            info.port = 3011;
            break;
        case ServiceEntity::ServiceType::DATABASE:
            info.name = "Database";
            info.url = "http://localhost:5432";
            info.port = 5432;
            break;
        default:
            info.name = "Unknown Service";
            info.url = "http://localhost:8000";
            info.port = 8000;
    }
    
    return std::make_unique<ServiceEntity>(info);
}

std::unique_ptr<ServiceEntity> ServiceFactory::createService(const ServiceEntity::ServiceInfo& info) {
    return std::make_unique<ServiceEntity>(info);
}

} // namespace FinalStorm