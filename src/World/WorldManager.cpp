//
//  WorldManager.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#include <stdio.h>
#include <algorithm>
#include "World/WorldManager.h"

namespace FinalStorm {

// Grid implementation
Grid::Grid(const GridCoordinate& coord)
    : m_coordinate(coord)
{
}

void Grid::addEntity(EntityPtr entity) {
    m_entities.push_back(entity);
}

void Grid::removeEntity(uint64_t entityId) {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [entityId](const EntityPtr& entity) {
                return entity->getId() == entityId;
            }),
        m_entities.end()
    );
}

// WorldManager implementation
WorldManager::WorldManager()
    : m_currentGrid{0, 0}
    , m_viewDistance(3)
{
    // Initialize with a default grid
    loadGrid(m_currentGrid);
}

WorldManager::~WorldManager() {
}

void WorldManager::update(float deltaTime) {
    // Update all entities
    for (auto& [id, entity] : m_entities) {
        entity->update(deltaTime);
    }
    
    // Check if player has moved to a new grid
    if (m_playerEntity) {
        GridCoordinate newGrid = getGridFromPosition(m_playerEntity->getTransform().position);
        if (newGrid != m_currentGrid) {
            onPlayerGridChange(m_currentGrid, newGrid);
            m_currentGrid = newGrid;
        }
    }
}

void WorldManager::addEntity(EntityPtr entity) {
    m_entities[entity->getId()] = entity;
    
    // Add to appropriate grid
    GridCoordinate gridCoord = getGridFromPosition(entity->getTransform().position);
    if (auto grid = getGrid(gridCoord)) {
        grid->addEntity(entity);
    }
    
    // Track player entity
    if (entity->getType() == EntityType::Player) {
        m_playerEntity = entity;
    }
}

void WorldManager::removeEntity(uint64_t entityId) {
    auto it = m_entities.find(entityId);
    if (it != m_entities.end()) {
        // Remove from grid
        GridCoordinate gridCoord = getGridFromPosition(it->second->getTransform().position);
        if (auto grid = getGrid(gridCoord)) {
            grid->removeEntity(entityId);
        }
        
        // Remove from entities map
        m_entities.erase(it);
    }
}

EntityPtr WorldManager::getEntity(uint64_t entityId) const {
    auto it = m_entities.find(entityId);
    return (it != m_entities.end()) ? it->second : nullptr;
}

std::vector<EntityPtr> WorldManager::getVisibleEntities(const Camera& camera) const {
    std::vector<EntityPtr> visibleEntities;
    
    // Get view-projection matrix for frustum culling
    float4x4 viewProj = camera.getViewProjectionMatrix();
    
    // Check entities in nearby grids
    GridCoordinate cameraGrid = getGridFromPosition(camera.getPosition());
    
    for (int dx = -m_viewDistance; dx <= m_viewDistance; dx++) {
        for (int dy = -m_viewDistance; dy <= m_viewDistance; dy++) {
            GridCoordinate checkGrid{cameraGrid.x + dx, cameraGrid.y + dy};
            
            if (auto grid = getGrid(checkGrid)) {
                for (const auto& entity : grid->getEntities()) {
                    if (entity->isVisible() && entity->isInFrustum(viewProj)) {
                        visibleEntities.push_back(entity);
                    }
                }
            }
        }
    }
    
    return visibleEntities;
}

void WorldManager::loadGrid(const GridCoordinate& coord) {
    // Check if already loaded
    if (m_grids.find(coord) != m_grids.end()) {
        return;
    }
    
    // Create new grid
    auto grid = std::make_unique<Grid>(coord);
    
    // Generate terrain and entities for this grid
    generateGridContent(coord, *grid);
    
    m_grids[coord] = std::move(grid);
}

void WorldManager::unloadGrid(const GridCoordinate& coord) {
    auto it = m_grids.find(coord);
    if (it != m_grids.end()) {
        // Remove entities from this grid
        for (const auto& entity : it->second->getEntities()) {
            m_entities.erase(entity->getId());
        }
        
        m_grids.erase(it);
    }
}

Grid* WorldManager::getGrid(const GridCoordinate& coord) const {
    auto it = m_grids.find(coord);
    return (it != m_grids.end()) ? it->second.get() : nullptr;
}

GridCoordinate WorldManager::getGridFromPosition(const float3& position) const {
    const float gridSize = 256.0f; // Size of each grid in world units
    return GridCoordinate{
        static_cast<int32_t>(floor(position.x / gridSize)),
        static_cast<int32_t>(floor(position.z / gridSize))
    };
}

void WorldManager::onPlayerGridChange(const GridCoordinate& oldGrid, const GridCoordinate& newGrid) {
    // Load new grids
    for (int dx = -m_viewDistance; dx <= m_viewDistance; dx++) {
        for (int dy = -m_viewDistance; dy <= m_viewDistance; dy++) {
            GridCoordinate gridToLoad{newGrid.x + dx, newGrid.y + dy};
            loadGrid(gridToLoad);
        }
    }
    
    // Unload distant grids
    std::vector<GridCoordinate> gridsToUnload;
    for (const auto& [coord, grid] : m_grids) {
        int distance = std::max(abs(coord.x - newGrid.x), std::abs(coord.y - newGrid.y));
        if (distance > m_viewDistance + 1) {
            gridsToUnload.push_back(coord);
        }
    }
    
    for (const auto& coord : gridsToUnload) {
        unloadGrid(coord);
    }
}

void WorldManager::generateGridContent(const GridCoordinate& coord, Grid& grid) {
    // Generate procedural content for this grid
    // This is a placeholder - in real implementation, this would use
    // noise functions, biome data, etc.
    
    // Add some test entities
    uint64_t baseId = (coord.x * 1000 + coord.y) * 100;
    
    // Add some NPCs
    for (int i = 0; i < 3; i++) {
        auto npc = std::make_shared<NPCEntity>(baseId + i, "villager");
        
        float3 position = simd_make_float3(
            coord.x * 256.0f + (rand() % 200) - 100.0f,
            0.0f,
            coord.y * 256.0f + (rand() % 200) - 100.0f
        );
        npc->getTransform().position = position;
        
        addEntity(npc);
    }
}

} // namespace FinalStorm
