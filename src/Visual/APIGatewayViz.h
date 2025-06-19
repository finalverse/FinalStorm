#pragma once
#include "Services/ServiceEntity.h"

namespace FinalStorm {

class APIGatewayViz : public ServiceEntity {
public:
    APIGatewayViz();
    
protected:
    void createVisualComponents() override;
    void updateVisualEffects(float deltaTime) override;
    
private:
    std::vector<std::unique_ptr<Entity>> m_connectionBeams;
    void updateConnectionBeams(float deltaTime);
};

} // namespace FinalStorm
