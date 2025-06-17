#pragma once
#include "Services/ServiceEntity.h"

namespace FinalStorm {

class AIServiceViz : public ServiceEntity {
public:
    AIServiceViz(ServiceType type = ServiceType::AIService);
    
protected:
    void createVisualComponents() override;
    void updateVisualEffects(float deltaTime) override;
};

} // namespace FinalStorm
