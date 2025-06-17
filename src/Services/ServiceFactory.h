// ServiceFactory.h - Factory for creating service visualizations

#pragma once

#include "Services/ServiceEntity.h"
#include <memory>

namespace FinalStorm {

class ServiceFactory {
public:
    static std::unique_ptr<ServiceEntity> createService(ServiceEntity::ServiceType type);
    static std::unique_ptr<ServiceEntity> createService(const ServiceEntity::ServiceInfo& info);
    
private:
    ServiceFactory() = delete; // Static class
};

} // namespace FinalStorm
