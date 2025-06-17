//
//  MessageProtocol.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "../Math/Math.h"

namespace FinalStorm {

// Message types matching Finalverse server protocol
enum class MessageType : uint32_t {
    Unknown = 0,
    Connect = 1,
    Disconnect = 2,
    WorldUpdate = 3,
    EntityUpdate = 4,
    PositionUpdate = 5,
    ChatMessage = 6,
    AudioStream = 7,
    TerrainData = 8,
    PlayerAction = 9,
    ServerInfo = 10
};

// Base message header
struct MessageHeader {
    MessageType type;
    uint32_t length;
    uint64_t timestamp;
};

// Connection messages
struct ConnectRequest {
    std::string clientVersion;
    std::string platform;
    std::string username;
    uint32_t protocolVersion;
};

struct ConnectResponse {
    bool success;
    std::string sessionId;
    std::string worldId;
    float3 spawnPosition;
    std::string message;
};

// World update messages
struct EntityData {
    uint64_t entityId;
    uint32_t entityType;
    float3 position;
    float4 rotation;
    std::string meshName;
    std::vector<uint8_t> customData;
};

struct WorldUpdate {
    std::string worldId;
    std::vector<EntityData> entities;
    std::vector<uint64_t> removedEntities;
    double timestamp;
};

// Player messages
struct PositionUpdate {
    float3 position;
    float4 rotation;
    float3 velocity;
    double timestamp;
};

struct PlayerAction {
    enum ActionType : uint32_t {
        Move = 0,
        Jump = 1,
        Interact = 2,
        Attack = 3,
        UseAbility = 4
    };
    
    ActionType type;
    float3 direction;
    uint64_t targetId;
    std::vector<uint8_t> parameters;
};

// Serialization helpers
class MessageSerializer {
public:
    static std::vector<uint8_t> serialize(const MessageHeader& header);
    static std::vector<uint8_t> serialize(const ConnectRequest& request);
    static std::vector<uint8_t> serialize(const PositionUpdate& update);
    static std::vector<uint8_t> serialize(const PlayerAction& action);
    
    static bool deserialize(const uint8_t* data, size_t size, MessageHeader& header);
    static bool deserialize(const uint8_t* data, size_t size, ConnectResponse& response);
    static bool deserialize(const uint8_t* data, size_t size, WorldUpdate& update);
    static bool deserialize(const uint8_t* data, size_t size, EntityData& entity);
};

} // namespace FinalStorm
