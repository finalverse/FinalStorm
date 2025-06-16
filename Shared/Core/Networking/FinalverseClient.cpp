//
//  FinalverseClient.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#include "FinalverseClient.h"
#include "../World/WorldManager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

namespace FinalStorm {

FinalverseClient::FinalverseClient()
    : m_connected(false)
    , m_port(0)
    , m_socket(-1)
    , m_running(false)
    , m_worldManager(nullptr)
{
}

FinalverseClient::~FinalverseClient()
{
    disconnect();
}

void FinalverseClient::connect(const std::string& host, uint16_t port, ConnectCallback callback)
{
    m_host = host;
    m_port = port;
    m_connectCallback = callback;
    
    m_running = true;
    m_networkThread = std::make_unique<std::thread>(&FinalverseClient::networkThread, this);
}

void FinalverseClient::disconnect()
{
    m_running = false;
    
    if (m_networkThread && m_networkThread->joinable()) {
        m_networkThread->join();
    }
    
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
    
    m_connected = false;
}

void FinalverseClient::networkThread()
{
    // Create socket
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0) {
        if (m_connectCallback) m_connectCallback(false);
        return;
    }
    
    // Set non-blocking
    int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    
    // Connect to server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_port);
    serverAddr.sin_addr.s_addr = inet_addr(m_host.c_str());
    
    if (::connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        if (errno != EINPROGRESS) {
            if (m_connectCallback) m_connectCallback(false);
            return;
        }
    }
    
    // Wait for connection
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(m_socket, &writefds);
    
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    if (select(m_socket + 1, nullptr, &writefds, nullptr, &timeout) > 0) {
        m_connected = true;
        if (m_connectCallback) m_connectCallback(true);
        
        // Main network loop
        while (m_running) {
            // Send queued messages
            {
                std::lock_guard<std::mutex> lock(m_sendMutex);
                while (!m_sendQueue.empty()) {
                    const auto& msg = m_sendQueue.front();
                    // Send message (implement protocol)
                    m_sendQueue.pop();
                }
            }
            
            // Receive messages
            uint8_t buffer[4096];
            ssize_t received = recv(m_socket, buffer, sizeof(buffer), 0);
            if (received > 0) {
                // Parse and queue received messages
                // (Implement based on your protocol)
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } else {
        if (m_connectCallback) m_connectCallback(false);
    }
    
    m_connected = false;
    if (m_disconnectCallback) m_disconnectCallback();
}

void FinalverseClient::sendPlayerPosition(const float3& position)
{
    // Create position update message
    struct PositionMessage {
        MessageType type = MessageType::PositionUpdate;
        float x, y, z;
    } msg;
    
    msg.x = position.x;
    msg.y = position.y;
    msg.z = position.z;
    
    sendMessage(MessageType::PositionUpdate, &msg, sizeof(msg));
}

void FinalverseClient::sendMessage(MessageType type, const void* data, size_t size)
{
    Message msg;
    msg.type = type;
    msg.data.assign((uint8_t*)data, (uint8_t*)data + size);
    
    std::lock_guard<std::mutex> lock(m_sendMutex);
    m_sendQueue.push(msg);
}

void FinalverseClient::update()
{
    std::lock_guard<std::mutex> lock(m_receiveMutex);
    while (!m_receiveQueue.empty()) {
        processMessage(m_receiveQueue.front());
        m_receiveQueue.pop();
    }
}

void FinalverseClient::processMessage(const Message& msg)
{
    switch (msg.type) {
        case MessageType::WorldUpdate:
            // Parse world update and forward to WorldManager
            if (m_worldManager) {
                // m_worldManager->processWorldUpdate(msg.data);
            }
            break;
            
        case MessageType::EntityUpdate:
            // Parse entity update
            if (m_worldManager) {
                // m_worldManager->processEntityUpdate(msg.data);
            }
            break;
            
        default:
            break;
    }
}

} // namespace FinalStorm
