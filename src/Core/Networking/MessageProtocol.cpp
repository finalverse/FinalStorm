//
//  MessageProtocol.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#include <stdio.h>
#include <cstring>
#include "Core/Networking/MessageProtocol.h"

namespace FinalStorm {

// Helper functions for serialization
template<typename T>
static void writeValue(std::vector<uint8_t>& buffer, const T& value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
}

template<typename T>
static bool readValue(const uint8_t*& data, size_t& remaining, T& value) {
    if (remaining < sizeof(T)) return false;
    std::memcpy(&value, data, sizeof(T));
    data += sizeof(T);
    remaining -= sizeof(T);
    return true;
}

static void writeString(std::vector<uint8_t>& buffer, const std::string& str) {
    uint32_t length = static_cast<uint32_t>(str.length());
    writeValue(buffer, length);
    buffer.insert(buffer.end(), str.begin(), str.end());
}

static bool readString(const uint8_t*& data, size_t& remaining, std::string& str) {
    uint32_t length;
    if (!readValue(data, remaining, length)) return false;
    if (remaining < length) return false;
    
    str.assign(reinterpret_cast<const char*>(data), length);
    data += length;
    remaining -= length;
    return true;
}

// MessageSerializer implementation
std::vector<uint8_t> MessageSerializer::serialize(const MessageHeader& header) {
    std::vector<uint8_t> buffer;
    writeValue(buffer, header.type);
    writeValue(buffer, header.length);
    writeValue(buffer, header.timestamp);
    return buffer;
}

std::vector<uint8_t> MessageSerializer::serialize(const ConnectRequest& request) {
    std::vector<uint8_t> buffer;
    writeString(buffer, request.clientVersion);
    writeString(buffer, request.platform);
    writeString(buffer, request.username);
    writeValue(buffer, request.protocolVersion);
    return buffer;
}

std::vector<uint8_t> MessageSerializer::serialize(const PositionUpdate& update) {
    std::vector<uint8_t> buffer;
    writeValue(buffer, update.position);
    writeValue(buffer, update.rotation);
    writeValue(buffer, update.velocity);
    writeValue(buffer, update.timestamp);
    return buffer;
}

std::vector<uint8_t> MessageSerializer::serialize(const PlayerAction& action) {
    std::vector<uint8_t> buffer;
    writeValue(buffer, action.type);
    writeValue(buffer, action.direction);
    writeValue(buffer, action.targetId);
    
    uint32_t paramSize = static_cast<uint32_t>(action.parameters.size());
    writeValue(buffer, paramSize);
    buffer.insert(buffer.end(), action.parameters.begin(), action.parameters.end());
    
    return buffer;
}

bool MessageSerializer::deserialize(const uint8_t* data, size_t size, MessageHeader& header) {
    size_t remaining = size;
    return readValue(data, remaining, header.type) &&
           readValue(data, remaining, header.length) &&
           readValue(data, remaining, header.timestamp);
}

bool MessageSerializer::deserialize(const uint8_t* data, size_t size, ConnectResponse& response) {
    size_t remaining = size;
    uint8_t success;
    
    if (!readValue(data, remaining, success)) return false;
    response.success = success != 0;
    
    return readString(data, remaining, response.sessionId) &&
           readString(data, remaining, response.worldId) &&
           readValue(data, remaining, response.spawnPosition) &&
           readString(data, remaining, response.message);
}

bool MessageSerializer::deserialize(const uint8_t* data, size_t size, WorldUpdate& update) {
    size_t remaining = size;
    
    if (!readString(data, remaining, update.worldId)) return false;
    if (!readValue(data, remaining, update.timestamp)) return false;
    
    uint32_t entityCount;
    if (!readValue(data, remaining, entityCount)) return false;
    
    update.entities.resize(entityCount);
    for (uint32_t i = 0; i < entityCount; i++) {
        if (!deserialize(data, remaining, update.entities[i])) return false;
    }
    
    uint32_t removedCount;
    if (!readValue(data, remaining, removedCount)) return false;
    
    update.removedEntities.resize(removedCount);
    for (uint32_t i = 0; i < removedCount; i++) {
        if (!readValue(data, remaining, update.removedEntities[i])) return false;
    }
    
    return true;
}

bool MessageSerializer::deserialize(const uint8_t* data, size_t size, EntityData& entity) {
    size_t remaining = size;
    
    if (!readValue(data, remaining, entity.entityId)) return false;
    if (!readValue(data, remaining, entity.entityType)) return false;
    if (!readValue(data, remaining, entity.position)) return false;
    if (!readValue(data, remaining, entity.rotation)) return false;
    if (!readString(data, remaining, entity.meshName)) return false;
    
    uint32_t customDataSize;
    if (!readValue(data, remaining, customDataSize)) return false;
    if (remaining < customDataSize) return false;
    
    entity.customData.resize(customDataSize);
    std::memcpy(entity.customData.data(), data, customDataSize);
    
    return true;
}

} // namespace FinalStorm
