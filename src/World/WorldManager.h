//
//  WorldManager.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include "World/Entity.h"
#include "Core/Math/MathTypes.h"
#include "Core/Math/Camera.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace FinalStorm {

struct GridCoordinate {
    int32_t x;
    int32_t y;
    
    GridCoordinate() : x(0), y(0) {}
    GridCoordinate(int32_t x, int32_t y) : x(x), y(y) {}
    
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
    void removeEntity(uint32_t entityId);
    const std::vector<EntityPtr>& getEntities() const { return entities; }
    
    const GridCoordinate& getCoordinate() const { return coordinate; }
    
private:
    GridCoordinate coordinate;
    std::vector<EntityPtr> entities;
};

class WorldManager {
public:
    WorldManager();
    ~WorldManager();
    
    void update(float deltaTime);
    
    void addEntity(EntityPtr entity);
    void removeEntity(uint32_t entityId);
    EntityPtr getEntity(uint32_t entityId) const;
    
    std::vector<EntityPtr> getVisibleEntities(const Camera& camera) const;
    std::vector<EntityPtr> getEntitiesInRadius(const vec3& center, float radius) const;
    
    void loadGrid(const GridCoordinate& coord);
    void unloadGrid(const GridCoordinate& coord);
    
    // Get all entities (for simple iteration)
    const std::vector<EntityPtr>& getAllEntities() const { return entities; }
    
private:
    // Private methods
    Grid* getGrid(const GridCoordinate& coord) const;
    GridCoordinate getGridFromPosition(const vec3& position) const;
    void onPlayerGridChange(const GridCoordinate& oldGrid, const GridCoordinate& newGrid);
    void generateGridContent(const GridCoordinate& coord, Grid& grid);
    
    // Entity storage - using simple vector for now, can optimize to grid later
    std::vector<EntityPtr> entities;
    std::unordered_map<GridCoordinate, std::unique_ptr<Grid>, GridCoordinateHash> grids;
    
    // Player tracking
    EntityPtr playerEntity;
    GridCoordinate currentGrid;
    
    // Configuration
    int viewDistance;
    static constexpr float GRID_SIZE = 256.0f; // Size of each grid cell in world units
};

} // namespace FinalStorm