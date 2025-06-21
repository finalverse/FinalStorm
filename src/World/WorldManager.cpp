// src/World/WorldManager.cpp
// World management implementation
// Handles entity lifecycle and spatial organization

#include "World/WorldManager.h"
#include "World/Entity.h"
#include "Core/Math/Math.h"
#include "Core/Math/Camera.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace FinalStorm {

// Grid implementation
Grid::Grid(const GridCoordinate& coord)
    : coordinate(coord) {
}

void Grid::addEntity(EntityPtr entity) {
    if (entity) {
        entities.push_back(entity);
    }
}

void Grid::removeEntity(uint32_t entityId) {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [entityId](const EntityPtr& entity) {
                return entity->getId() == entityId;
            }),
        entities.end()
    );
}

// WorldManager implementation
WorldManager::WorldManager() 
    : currentGrid(0, 0)
    , viewDistance(3) {
}

WorldManager::~WorldManager() = default;

void WorldManager::update(float deltaTime) {
    // Update all active entities
    for (auto& entity : entities) {
        if (entity && entity->isActive()) {
            entity->update(deltaTime);
        }
    }
    
    // Remove inactive entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const EntityPtr& entity) {
                return !entity || !entity->isActive();
            }),
        entities.end()
    );
    
    // Update player grid if we have a player
    if (playerEntity) {
        GridCoordinate newGrid = getGridFromPosition(playerEntity->getTransform().position);
        if (newGrid != currentGrid) {
            onPlayerGridChange(currentGrid, newGrid);
            currentGrid = newGrid;
        }
    }
}

void WorldManager::addEntity(EntityPtr entity) {
    if (entity) {
        entities.push_back(entity);
        
        // If this is a player entity, track it
        if (entity->getType() == EntityType::Player) {
            playerEntity = entity;
            currentGrid = getGridFromPosition(entity->getTransform().position);
        }
        
        // Add to appropriate grid
        GridCoordinate gridCoord = getGridFromPosition(entity->getTransform().position);
        Grid* grid = getGrid(gridCoord);
        if (!grid) {
            loadGrid(gridCoord);
            grid = getGrid(gridCoord);
        }
        if (grid) {
            grid->addEntity(entity);
        }
    }
}

void WorldManager::removeEntity(uint32_t entityId) {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entityId](const EntityPtr& entity) {
            return entity && entity->getId() == entityId;
        });
    
    if (it != entities.end()) {
        (*it)->setActive(false);
        
        // Remove from player tracking if needed
        if (*it == playerEntity) {
            playerEntity = nullptr;
        }
        
        // Remove from grid
        GridCoordinate gridCoord = getGridFromPosition((*it)->getTransform().position);
        Grid* grid = getGrid(gridCoord);
        if (grid) {
            grid->removeEntity(entityId);
        }
    }
}

EntityPtr WorldManager::getEntity(uint32_t entityId) const {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entityId](const EntityPtr& entity) {
            return entity && entity->getId() == entityId;
        });
    
    return (it != entities.end()) ? *it : nullptr;
}

std::vector<EntityPtr> WorldManager::getVisibleEntities(const Camera& camera) const {
    std::vector<EntityPtr> visibleEntities;
    
    for (const auto& entity : entities) {
        if (entity && entity->isActive()) {
            // Simple frustum culling
            if (entity->isInFrustum(camera.getViewProjectionMatrix())) {
                visibleEntities.push_back(entity);
            }
        }
    }
    
    return visibleEntities;
}

std::vector<EntityPtr> WorldManager::getEntitiesInRadius(const vec3& center, float radius) const {
    std::vector<EntityPtr> result;
    float radiusSq = radius * radius;
    
    for (const auto& entity : entities) {
        if (entity && entity->isActive()) {
            vec3 pos = entity->getTransform().position;
            vec3 delta = pos - center;
            float distSq = dot(delta, delta);
            
            if (distSq <= radiusSq) {
                result.push_back(entity);
            }
        }
    }
    
    return result;
}

void WorldManager::loadGrid(const GridCoordinate& coord) {
    // Create new grid if it doesn't exist
    if (grids.find(coord) == grids.end()) {
        auto grid = std::make_unique<Grid>(coord);
        generateGridContent(coord, *grid);
        grids[coord] = std::move(grid);
    }
}

void WorldManager::unloadGrid(const GridCoordinate& coord) {
    auto it = grids.find(coord);
    if (it != grids.end()) {
        grids.erase(it);
    }
}

Grid* WorldManager::getGrid(const GridCoordinate& coord) const {
    auto it = grids.find(coord);
    return (it != grids.end()) ? it->second.get() : nullptr;
}

GridCoordinate WorldManager::getGridFromPosition(const vec3& position) const {
    int32_t gridX = static_cast<int32_t>(std::floor(position.x / GRID_SIZE));
    int32_t gridY = static_cast<int32_t>(std::floor(position.z / GRID_SIZE));
    return GridCoordinate(gridX, gridY);
}

void WorldManager::onPlayerGridChange(const GridCoordinate& oldGrid, const GridCoordinate& newGrid) {
    // Load grids around new position
    for (int x = newGrid.x - viewDistance; x <= newGrid.x + viewDistance; ++x) {
        for (int y = newGrid.y - viewDistance; y <= newGrid.y + viewDistance; ++y) {
            loadGrid(GridCoordinate(x, y));
        }
    }
    
    // Unload grids that are too far from new position
    std::vector<GridCoordinate> gridsToUnload;
    for (const auto& pair : grids) {
        const GridCoordinate& coord = pair.first;
        int dx = std::abs(coord.x - newGrid.x);
        int dy = std::abs(coord.y - newGrid.y);
        
        if (dx > viewDistance || dy > viewDistance) {
            gridsToUnload.push_back(coord);
        }
    }
    
    for (const auto& coord : gridsToUnload) {
        unloadGrid(coord);
    }
}

void WorldManager::generateGridContent(const GridCoordinate& coord, Grid& grid) {
    // Generate procedural content for this grid
    // For now, this is just a placeholder
    
    // You could add terrain generation, NPC spawning, etc. here
    // Example: spawn some NPCs in certain grids
    if (coord.x % 3 == 0 && coord.y % 3 == 0) {
        // Spawn an NPC every 3rd grid
        auto npc = std::make_shared<NPCEntity>("guard");
        
        // Position within the grid
        vec3 gridWorldPos = make_vec3(
            coord.x * GRID_SIZE + GRID_SIZE * 0.5f,
            0.0f,
            coord.y * GRID_SIZE + GRID_SIZE * 0.5f
        );
        
        npc->getTransform().setPosition(gridWorldPos);
        grid.addEntity(npc);
        
        // Also add to main entity list
        entities.push_back(npc);
    }
}

} // namespace FinalStorm