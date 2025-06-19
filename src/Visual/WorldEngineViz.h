#pragma once
#include "Services/ServiceEntity.h"

namespace FinalStorm {

class WorldEngineViz : public ServiceEntity {
public:
    WorldEngineViz(ServiceType type = ServiceType::WorldEngine);
    
protected:
    void createVisualComponents() override;
    void updateVisualEffects(float deltaTime) override;
};

} // namespace FinalStorm
