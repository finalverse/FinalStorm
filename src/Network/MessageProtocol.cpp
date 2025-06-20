// src/Network/MessageProtocol.cpp
// Message protocol implementation
// Handles serialization and deserialization of network messages

#include "Network/MessageProtocol.h"
#include <cstring>

namespace FinalStorm {

Message::Message(MessageType t)
    : type(t)
    , timestamp(0) {
}

std::vector<uint8_t> Message::serialize() const {
    std::vector<uint8_t> buffer;
    
    // Simple serialization - in production use protobuf or similar
    buffer.resize(sizeof(MessageType) + sizeof(uint64_t) + data.size());
    
    size_t offset = 0;
    memcpy(buffer.data() + offset, &type, sizeof(MessageType));
    offset += sizeof(MessageType);
    
    memcpy(buffer.data() + offset, &timestamp, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    
    if (!data.empty()) {
        memcpy(buffer.data() + offset, data.data(), data.size());
    }
    
    return buffer;
}

bool Message::deserialize(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(MessageType) + sizeof(uint64_t)) {
        return false;
    }
    
    size_t offset = 0;
    memcpy(&type, buffer.data() + offset, sizeof(MessageType));
    offset += sizeof(MessageType);
    
    memcpy(&timestamp, buffer.data() + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);
    
    size_t dataSize = buffer.size() - offset;
    if (dataSize > 0) {
        data.resize(dataSize);
        memcpy(data.data(), buffer.data() + offset, dataSize);
    }
    
    return true;
}

} // namespace FinalStorm