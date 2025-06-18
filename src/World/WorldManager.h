//
//  WorldManager.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include "World/Entity.h"
#include "Core/Math/Math.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace FinalStorm {

struct GridCoordinate {
    int32_t x;
    int32_t y;
    
    bool operator==(const GridCoordinate& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const GridCoordinate& other) const {
        return !(*this == other);
    }
};

struct GridCoordinateHash {
    std::size_t operator()(const GridCoordinate& coord) const {
        return std::hash<int32_t>()(coord.x) ^ (std::hash<int32_t>()(coord.y) << 1);
    }
};

class Grid {
public:
    Grid(const GridCoordinate& coord);
    
    void addEntity(EntityPtr entity);
    void removeEntity(uint64_t entityId);
    const std::vector<EntityPtr>& getEntities() const { return m_entities; }
    
private:
    GridCoordinate m_coordinate;
    std::vector<EntityPtr> m_entities;
};

class WorldManager {
public:
    WorldManager();
    ~WorldManager();
    
    void update(float deltaTime);
    
    void addEntity(EntityPtr entity);
    void removeEntity(uint64_t entityId);
    EntityPtr getEntity(uint64_t entityId) const;
    
    std::vector<EntityPtr> getVisibleEntities(const Camera& camera) const;
    
    void loadGrid(const GridCoordinate& coord);
    void unloadGrid(const GridCoordinate& coord);
    
private:
    // Add these private methods
    Grid* getGrid(const GridCoordinate& coord) const;
    GridCoordinate getGridFromPosition(const float3& position) const;
    void onPlayerGridChange(const GridCoordinate& oldGrid, const GridCoordinate& newGrid);
    void generateGridContent(const GridCoordinate& coord, Grid& grid);
    
    std::unordered_map<uint64_t, EntityPtr> m_entities;
    std::unordered_map<GridCoordinate, std::unique_ptr<Grid>, GridCoordinateHash> m_grids;
    
    // Player tracking
    EntityPtr m_playerEntity;
    GridCoordinate m_currentGrid;
    
    // Add this member variable
    int m_viewDistance;
};

} // namespace FinalStorm