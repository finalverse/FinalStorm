// src/Services/ServiceFactory.cpp
// Service factory implementation
// Creates appropriate visualizations for different service types

#include "Services/ServiceFactory.h"
#include "Services/ServiceEntity.h"

namespace FinalStorm {

// Simple service entity for now - later we'll add specific visualizations
class GenericServiceEntity : public ServiceEntity {
public:
    GenericServiceEntity(ServiceType type, const std::string& name)
        : ServiceEntity(type, name) {}
    
    void onRender(RenderContext& context) override {
        // Basic rendering - will be replaced with specific visualizations
        context.pushTransform(getWorldMatrix());
        
        // Render a simple cube for now
        float4 color(0.2f, 0.8f, 1.0f, 1.0f);
        context.setColor(color * (0.5f + 0.5f * health));
        context.drawCube(1.0f);
        
        context.popTransform();
    }
};

std::shared_ptr<ServiceEntity> ServiceFactory::createService(ServiceType type) {
    std::string name;
    
    switch (type) {
        case ServiceType::APIGateway:
            name = "API Gateway";
            break;
        case ServiceType::WebSocket:
            name = "WebSocket Gateway";
            break;
        case ServiceType::WorldEngine:
            name = "World Engine";
            break;
        case ServiceType::AIService:
            name = "AI Orchestra";
            break;
        case ServiceType::AudioService:
            name = "Symphony Engine";
            break;
        case ServiceType::Database:
            name = "Database";
            break;
        case ServiceType::Community:
            name = "Community Service";
            break;
        default:
            name = "Unknown Service";
    }
    
    return std::make_shared<GenericServiceEntity>(type, name);
}

} // namespace FinalStorm