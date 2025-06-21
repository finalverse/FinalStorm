// ============================================================================
// File: FinalStorm/src/Services/Components/ConnectionBeam.h
// Advanced Connection Visualization System for Finalverse Service Networks
// ============================================================================

#pragma once

#include "Scene/SceneNode.h"
#include "Core/Math/MathTypes.h"
#include "Rendering/Material.h"
#include "Rendering/Mesh.h"
#include <memory>
#include <vector>

namespace FinalStorm {

// Forward declarations
class ParticleEmitter;
class RenderContext;

// ============================================================================
// ConnectionBeam - Dynamic 3D connection visualization between services
// ============================================================================

class ConnectionBeam : public SceneNode {
public:
    // Connection types for different visualization styles
    enum class ConnectionType {
        DATA_FLOW,      // Standard data transfer
        QUERY,          // Database query
        API_CALL,       // REST/GraphQL API call
        STREAM,         // Real-time streaming
        BLOCKCHAIN,     // Blockchain transaction
        AI_INFERENCE,   // AI model inference
        AUDIO_SIGNAL    // Audio/music data flow
    };
    
    // Connection states affecting visual appearance
    enum class ConnectionState {
        ESTABLISHING,   // Connection being established
        ACTIVE,         // Normal active connection
        HIGH_LOAD,      // Heavy traffic
        ERROR,          // Connection error
        TIMEOUT,        // Connection timeout
        CLOSING         // Connection closing
    };

    ConnectionBeam();
    virtual ~ConnectionBeam();

    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

    // Connection setup
    void setStartPosition(const vec3& pos);
    void setEndPosition(const vec3& pos);
    void setConnectionType(ConnectionType type);
    void setConnectionState(ConnectionState state);
    
    // Visual properties
    void setColor(const vec3& color);
    void setIntensity(float intensity);
    void setThickness(float thickness);
    void setFlowSpeed(float speed);
    void setFlowDirection(float direction); // 1.0 = start to end, -1.0 = end to start
    
    // Animation and effects
    void setDuration(float duration); // 0.0 = permanent
    void setConnectionId(uint32_t id);
    void pulse(float intensity = 1.5f);
    void showDataPacket(const vec3& color = vec3_one(), float size = 0.1f);
    void setLatency(float milliseconds); // Affects visual delay
    void setBandwidth(float mbps); // Affects thickness and particle density
    
    // State queries
    bool isExpired() const;
    float getLength() const;
    uint32_t getConnectionId() const { return m_connectionId; }
    ConnectionType getType() const { return m_connectionType; }
    ConnectionState getState() const { return m_connectionState; }
    
    // Advanced effects
    void setTurbulence(float amount);
    void setGlowFalloff(float falloff);
    void enableQuantumFlicker(bool enable); // For quantum-themed connections
    void setHolographicNoise(float amount); // Holographic interference effect

private:
    // Core properties
    vec3 m_startPosition;
    vec3 m_endPosition;
    vec3 m_color;
    float m_intensity;
    float m_thickness;
    float m_flowSpeed;
    float m_flowDirection;
    
    // Connection metadata
    ConnectionType m_connectionType;
    ConnectionState m_connectionState;
    uint32_t m_connectionId;
    float m_latency;
    float m_bandwidth;
    
    // Animation state
    float m_duration;
    float m_maxDuration;
    bool m_isExpired;
    float m_flowPhase;
    float m_pulsePhase;
    float m_pulseIntensity;
    
    // Visual effects
    float m_turbulence;
    float m_glowFalloff;
    bool m_quantumFlicker;
    float m_holographicNoise;
    float m_quantumPhase;
    float m_hologramPhase;
    
    // Geometry and rendering
    std::shared_ptr<class BeamMesh> m_beamMesh;
    std::shared_ptr<Material> m_beamMaterial;
    std::shared_ptr<ParticleEmitter> m_flowParticles;
    std::shared_ptr<ParticleEmitter> m_glowParticles;
    bool m_geometryDirty;
    int m_segments;
    
    // Data packet system
    struct DataPacket {
        vec3 position;
        vec3 color;
        float size;
        float life;
        float maxLife;
        float speed;
    };
    std::vector<DataPacket> m_dataPackets;
    
    // Private methods
    void createBeamGeometry();
    void createFlowEffects();
    void updateFlow(float deltaTime);
    void updatePulse(float deltaTime);
    void updateDuration(float deltaTime);
    void updateGeometry();
    void updateDataPackets(float deltaTime);
    void updateQuantumEffects(float deltaTime);
    void updateHolographicNoise(float deltaTime);
    
    void updateBeamMesh();
    void updateFlowLine();
    void updateMaterialProperties();
    
    void renderBeam(RenderContext& context);
    void renderFlowEffects(RenderContext& context);
    void renderFlowIndicators(RenderContext& context);
    void renderDataPackets(RenderContext& context);
    void renderGlowEffect(RenderContext& context);
    
    vec3 calculateBeamPosition(float t) const;
    vec3 calculateFlowTurbulence(const vec3& position, float time) const;
    vec4 getConnectionTypeColor() const;
    vec4 getConnectionStateColor() const;
    float calculateStateIntensityMultiplier() const;
};

// ============================================================================
// BeamMesh - Specialized mesh for rendering connection beams
// ============================================================================

class BeamMesh {
public:
    BeamMesh();
    ~BeamMesh();
    
    void setSegmentCount(int segments);
    void updateCenterLine(const std::vector<vec3>& centerLine, float thickness);
    void generateTube(const std::vector<vec3>& centerLine, float thickness);
    void generateRibbon(const std::vector<vec3>& centerLine, float width);
    
    void render(RenderContext& context);
    
    // Mesh data access
    const std::vector<vec3>& getVertices() const { return m_vertices; }
    const std::vector<vec3>& getNormals() const { return m_normals; }
    const std::vector<vec2>& getUVs() const { return m_uvs; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    
private:
    std::vector<vec3> m_vertices;
    std::vector<vec3> m_normals;
    std::vector<vec2> m_uvs;
    std::vector<uint32_t> m_indices;
    
    int m_segmentCount;
    bool m_isDirty;
    
    // Metal buffer objects
    id<MTLBuffer> m_vertexBuffer;
    id<MTLBuffer> m_indexBuffer;
    
    void updateBuffers();
    vec3 calculateTangent(const std::vector<vec3>& centerLine, int index) const;
    vec3 calculateBinormal(const vec3& tangent, const vec3& up = make_vec3(0, 1, 0)) const;
};

// ============================================================================
// ConnectionManager - Manages multiple connection beams
// ============================================================================

class ConnectionManager : public SceneNode {
public:
    ConnectionManager();
    virtual ~ConnectionManager();
    
    // Connection management
    std::shared_ptr<ConnectionBeam> createConnection(
        const vec3& startPos,
        const vec3& endPos,
        ConnectionBeam::ConnectionType type = ConnectionBeam::ConnectionType::DATA_FLOW
    );
    
    void removeConnection(uint32_t connectionId);
    void removeConnection(std::shared_ptr<ConnectionBeam> beam);
    void removeAllConnections();
    
    // Bulk operations
    void updateAllConnections(float deltaTime);
    void setGlobalIntensity(float intensity);
    void setGlobalFlowSpeed(float speed);
    void pulseAllConnections(float intensity = 1.2f);
    
    // Connection queries
    std::shared_ptr<ConnectionBeam> findConnection(uint32_t connectionId);
    std::vector<std::shared_ptr<ConnectionBeam>> getConnectionsByType(ConnectionBeam::ConnectionType type);
    std::vector<std::shared_ptr<ConnectionBeam>> getActiveConnections();
    int getConnectionCount() const { return static_cast<int>(m_connections.size()); }
    
    // Service-to-service connections
    void connectServices(const std::string& serviceA, const std::string& serviceB, 
                        ConnectionBeam::ConnectionType type);
    void disconnectServices(const std::string& serviceA, const std::string& serviceB);
    
    // Event handling
    void onServiceActivity(const std::string& serviceName, float intensity);
    void onDataTransfer(const std::string& from, const std::string& to, float amount);
    void onConnectionError(const std::string& from, const std::string& to);
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    std::vector<std::shared_ptr<ConnectionBeam>> m_connections;
    std::map<std::string, vec3> m_servicePositions;
    std::map<std::pair<std::string, std::string>, std::shared_ptr<ConnectionBeam>> m_serviceConnections;
    
    uint32_t m_nextConnectionId;
    float m_globalIntensity;
    float m_globalFlowSpeed;
    
    void cleanupExpiredConnections();
    uint32_t generateConnectionId();
    vec3 getServicePosition(const std::string& serviceName);
    void registerServicePosition(const std::string& serviceName, const vec3& position);
};

// ============================================================================
// ConnectionNetwork - High-level network visualization
// ============================================================================

class ConnectionNetwork : public SceneNode {
public:
    struct NetworkNode {
        std::string id;
        vec3 position;
        vec3 color;
        float activity;
        float importance; // Affects visual size/prominence
        std::map<std::string, float> connections; // target -> strength
    };
    
    ConnectionNetwork();
    virtual ~ConnectionNetwork();
    
    // Network management
    void addNode(const NetworkNode& node);
    void removeNode(const std::string& nodeId);
    void updateNode(const std::string& nodeId, const NetworkNode& node);
    
    void addConnection(const std::string& from, const std::string& to, float strength = 1.0f);
    void removeConnection(const std::string& from, const std::string& to);
    void updateConnectionStrength(const std::string& from, const std::string& to, float strength);
    
    // Network analysis
    void calculateNetworkLayout(); // Auto-arrange nodes
    void setLayoutAlgorithm(const std::string& algorithm); // "force", "circular", "hierarchical"
    void animateToLayout(float duration = 2.0f);
    
    // Visualization
    void setNetworkTheme(const std::string& theme); // "cyber", "organic", "quantum"
    void enableDataFlowVisualization(bool enable);
    void setFlowVisualizationSpeed(float speed);
    
    // Events
    void onNodeActivity(const std::string& nodeId, float intensity);
    void onDataFlow(const std::string& from, const std::string& to, float amount);
    void onNetworkEvent(const std::string& eventType, const std::string& nodeId);
    
    // SceneNode interface
    void update(float deltaTime) override;
    void render(RenderContext& context) override;

private:
    std::map<std::string, NetworkNode> m_nodes;
    std::shared_ptr<ConnectionManager> m_connectionManager;
    
    std::string m_layoutAlgorithm;
    std::string m_networkTheme;
    bool m_dataFlowVisualization;
    float m_flowSpeed;
    
    // Layout animation
    bool m_isAnimatingLayout;
    float m_layoutAnimationTime;
    float m_layoutAnimationDuration;
    std::map<std::string, vec3> m_targetPositions;
    std::map<std::string, vec3> m_startPositions;
    
    void updateLayout(float deltaTime);
    void updateConnections();
    void renderNodes(RenderContext& context);
    
    // Layout algorithms
    void calculateForceDirectedLayout();
    void calculateCircularLayout();
    void calculateHierarchicalLayout();
    
    vec3 interpolatePosition(const std::string& nodeId, float t);
};

} // namespace FinalStorm