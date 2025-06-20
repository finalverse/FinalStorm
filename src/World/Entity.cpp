// src/World/Entity.cpp
// Base entity class implementation
// Represents objects in the game world with position and behavior

#include "World/Entity.h"

namespace FinalStorm {

uint32_t Entity::nextId = 1;

Entity::Entity(EntityType entityType)
    : id(nextId++)
    , type(entityType)
    , active(true) {
}

Entity::~Entity() = default;

void Entity::update(float deltaTime) {
    // Base implementation - derived classes override
}

} // namespace FinalStorm