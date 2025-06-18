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
#include "Core/Math/Math.h"
#include "Core/Networking/MessageProtocol.h"
#include "Visualization/DataVisualizer.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

namespace FinalStorm {

class WorldManager;

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
    void connectToServer(const std::string& url);
    void subscribeToServices(const std::vector<std::string>& services);
    void disconnect();
    bool isConnected() const { return m_connected; }
    
    void sendPlayerPosition(const float3& position, const float4& rotation);  // Add rotation parameter
    void sendMessage(MessageType type, const void* data, size_t size);
    
    void setWorldManager(WorldManager* worldManager) { m_worldManager = worldManager; }
    void setDataVisualizer(DataVisualizer* viz) { m_dataVisualizer = viz; }
    void setDisconnectCallback(DisconnectCallback callback) { m_disconnectCallback = callback; }
    
    void update(); // Call from main thread to process received messages
    
private:
    void processMessage(const Message& msg);
    void handleEntityUpdate(const Message& msg) {}
    
    bool m_connected;
    std::string m_host;
    uint16_t m_port;

    std::atomic<bool> m_running;

    // Received messages
    std::queue<Message> m_receiveQueue;
    mutable std::mutex m_receiveMutex;

    using WSClient = websocketpp::client<websocketpp::config::asio_client>;
    WSClient m_client;
    websocketpp::connection_hdl m_connection;
    std::unique_ptr<websocketpp::lib::thread> m_wsThread;

    WorldManager* m_worldManager;
    DataVisualizer* m_dataVisualizer;
    ConnectCallback m_connectCallback;
    DisconnectCallback m_disconnectCallback;
};

} // namespace FinalStorm