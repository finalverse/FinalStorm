#pragma once
#include "Services/ServiceEntity.h"

namespace FinalStorm {

class CommunityViz : public ServiceEntity {
public:
    CommunityViz(ServiceType type = ServiceType::Community);
    
protected:
    void createVisualComponents() override;
    void updateVisualEffects(float deltaTime) override;
};

} // namespace FinalStorm
