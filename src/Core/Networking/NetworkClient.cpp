#include "Core/Networking/NetworkClient.h"
#include <iostream>

namespace FinalStorm {

namespace {
// Simple placeholder WebSocket client used for compiling the sample project.
class WebSocketClient {
public:
    void connect(const std::string& url) {
        std::cout << "[Network] connect " << url << std::endl;
    }
    void disconnect() {
        std::cout << "[Network] disconnect" << std::endl;
    }
    void send(const std::string& msg) {
        std::cout << "[Network] send: " << msg << std::endl;
    }
};
} // namespace

NetworkClient::NetworkClient() = default;

NetworkClient::~NetworkClient() {
    disconnect();
}

void NetworkClient::connect(const std::string& serverUrl) {
    if (m_state != ConnectionState::Disconnected) return;
    m_state = ConnectionState::Connecting;
    m_webSocket = std::make_unique<WebSocketClient>();
    m_webSocket->connect(serverUrl);
    m_state = ConnectionState::Connected;
    m_running = true;
    if (m_connectionCallback) m_connectionCallback(m_state);
}

void NetworkClient::disconnect() {
    if (m_state == ConnectionState::Disconnected) return;
    m_state = ConnectionState::Disconnecting;
    if (m_webSocket) {
        m_webSocket->disconnect();
    }
    m_state = ConnectionState::Disconnected;
    m_running = false;
    if (m_connectionCallback) m_connectionCallback(m_state);
}

void NetworkClient::sendMessage(const std::string& type, const std::string& payload) {
    if (!m_webSocket) return;
    m_webSocket->send(type + ":" + payload);
}

void NetworkClient::requestServiceList() {
    sendMessage("requestServiceList", "");
}

void NetworkClient::subscribeToService(const std::string& serviceId) {
    sendMessage("subscribe", serviceId);
}

void NetworkClient::unsubscribeFromService(const std::string& serviceId) {
    sendMessage("unsubscribe", serviceId);
}

void NetworkClient::update() {
    if (!m_running) return;
    processMessageQueue();
}

void NetworkClient::processMessageQueue() {
    std::queue<NetworkMessage> local;
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        std::swap(local, m_messageQueue);
    }
    while (!local.empty()) {
        if (m_messageCallback) m_messageCallback(local.front());
        local.pop();
    }
}

void NetworkClient::handleMessage(const std::string& message) {
    NetworkMessage msg{ "generic", message, 0.0 };
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_messageQueue.push(msg);
}

void NetworkClient::parseServiceUpdate(const std::string& payload) {
    ServiceInfo info{};
    info.id = payload;
    m_services[info.id] = info;
    if (m_serviceCallback) m_serviceCallback(info);
}

} // namespace FinalStorm
