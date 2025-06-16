//
//  FinalverseClient.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#pragma once

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include "../Math/Math.h"

namespace FinalStorm {

class WorldManager;

enum class MessageType : uint32_t {
    Unknown = 0,
    Connect = 1,
    WorldUpdate = 2,
    EntityUpdate = 3,
    PositionUpdate = 4,
    Disconnect = 5
};

struct Message {
    MessageType type;
    std::vector<uint8_t> data;
};

class FinalverseClient {
public:
    FinalverseClient();
    ~FinalverseClient();
    
    using ConnectCallback = std::function<void(bool success)>;
    using DisconnectCallback = std::function<void()>;
    
    void connect(const std::string& host, uint16_t port, ConnectCallback callback);
    void disconnect();
    bool isConnected() const { return m_connected; }
    
    void sendPlayerPosition(const float3& position, const float4& rotation);  // Add rotation parameter
    void sendMessage(MessageType type, const void* data, size_t size);
    
    void setWorldManager(WorldManager* worldManager) { m_worldManager = worldManager; }
    void setDisconnectCallback(DisconnectCallback callback) { m_disconnectCallback = callback; }
    
    void update(); // Call from main thread to process received messages
    
private:
    void networkThread();
    void processMessage(const Message& msg);
    void handleEntityUpdate(const Message& msg) {} // Add this
    
    bool m_connected;
    std::string m_host;
    uint16_t m_port;
    
    std::unique_ptr<std::thread> m_networkThread;
    std::atomic<bool> m_running;
    
    // Message queues
    std::queue<Message> m_sendQueue;
    std::queue<Message> m_receiveQueue;
    mutable std::mutex m_sendMutex;
    mutable std::mutex m_receiveMutex;
    
    // Socket handle (platform specific)
    int m_socket;
    
    // Callbacks
    WorldManager* m_worldManager;
    ConnectCallback m_connectCallback;
    DisconnectCallback m_disconnectCallback;
};

} // namespace FinalStorm