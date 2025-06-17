#pragma once
#include "Services/ServiceEntity.h"

namespace FinalStorm {

class AudioServiceViz : public ServiceEntity {
public:
    AudioServiceViz(ServiceType type = ServiceType::AudioService);
    
protected:
    void createVisualComponents() override;
    void updateVisualEffects(float deltaTime) override;
};

} // namespace FinalStorm
