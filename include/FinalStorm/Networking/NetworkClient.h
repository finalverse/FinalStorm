// Networking/NetworkClient.h
#pragma once

#include <string>
#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>

namespace FinalStorm {

class WebSocketClient;

enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected,
    Disconnecting,
    Error
};

struct ServiceInfo {
    std::string id;
    std::string name;
    std::string type;
    std::string url;
    bool isHealthy;
    float cpuUsage;
    float memoryUsage;
    int activeConnections;
};

struct NetworkMessage {
    std::string type;
    std::string payload;
    double timestamp;
};

class NetworkClient {
public:
    using ConnectionCallback = std::function<void(ConnectionState)>;
    using MessageCallback = std::function<void(const NetworkMessage&)>;
    using ServiceCallback = std::function<void(const ServiceInfo&)>;

    NetworkClient();
    ~NetworkClient();

    void connect(const std::string& serverUrl);
    void disconnect();
    bool isConnected() const { return m_state == ConnectionState::Connected; }
    ConnectionState getState() const { return m_state; }

    void sendMessage(const std::string& type, const std::string& payload);
    void setMessageCallback(MessageCallback callback) { m_messageCallback = callback; }

    void requestServiceList();
    void subscribeToService(const std::string& serviceId);
    void unsubscribeFromService(const std::string& serviceId);
    void setServiceCallback(ServiceCallback callback) { m_serviceCallback = callback; }

    void setConnectionCallback(ConnectionCallback callback) { m_connectionCallback = callback; }

    void update();

private:
    void processMessageQueue();
    void handleMessage(const std::string& message);
    void parseServiceUpdate(const std::string& payload);

    std::unique_ptr<WebSocketClient> m_webSocket;
    ConnectionState m_state = ConnectionState::Disconnected;

    ConnectionCallback m_connectionCallback;
    MessageCallback m_messageCallback;
    ServiceCallback m_serviceCallback;

    std::queue<NetworkMessage> m_messageQueue;
    mutable std::mutex m_queueMutex;

    std::unordered_map<std::string, ServiceInfo> m_services;

    std::thread m_networkThread;
    std::atomic<bool> m_running{false};
};

} // namespace FinalStorm
