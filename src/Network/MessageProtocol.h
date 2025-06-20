// src/Network/MessageProtocol.h
// Message protocol definitions
// Defines message types and serialization

#pragma once
#include <vector>
#include <cstdint>

namespace FinalStorm {

enum class MessageType : uint16_t {
    Connect = 0,
    Disconnect = 1,
    Ping = 2,
    Pong = 3,
    PlayerUpdate = 10,
    EntityUpdate = 11,
    ServiceUpdate = 20,
    ServiceMetrics = 21,
    WorldData = 30,
    ChatMessage = 40
};

class Message {
public:
    explicit Message(MessageType type);
    
    MessageType getType() const { return type; }
    void setTimestamp(uint64_t ts) { timestamp = ts; }
    uint64_t getTimestamp() const { return timestamp; }
    
    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& buffer);
    
    std::vector<uint8_t> data;
    
private:
    MessageType type;
    uint64_t timestamp;
};

} // namespace FinalStorm