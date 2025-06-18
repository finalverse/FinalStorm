//
//  FinalverseClient.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-15.
//

#include "Core/Networking/FinalverseClient.h"
#include "World/WorldManager.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <sstream>
#include <iostream>

namespace FinalStorm {

FinalverseClient::FinalverseClient()
    : m_connected(false)
    , m_port(0)
    , m_running(false)
    , m_worldManager(nullptr)
    , m_dataVisualizer(nullptr)
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
    std::stringstream url;
    url << "ws://" << host << ":" << port;
    connectToServer(url.str());
}

void FinalverseClient::connectToServer(const std::string& url)
{
    m_running = true;
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.init_asio();

    m_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
        m_connected = true;
        m_connection = hdl;
        if (m_connectCallback) m_connectCallback(true);
        subscribeToServices({});
    });

    m_client.set_fail_handler([this](websocketpp::connection_hdl) {
        m_connected = false;
        if (m_connectCallback) m_connectCallback(false);
    });

    m_client.set_close_handler([this](websocketpp::connection_hdl) {
        m_connected = false;
        if (m_disconnectCallback) m_disconnectCallback();
    });

    m_client.set_message_handler([this](websocketpp::connection_hdl, WSClient::message_ptr msg) {
        Message message;
        message.data.assign(msg->get_payload().begin(), msg->get_payload().end());

        if (msg->get_opcode() == websocketpp::frame::opcode::binary) {
            MessageHeader header;
            if (MessageSerializer::deserialize(message.data.data(), message.data.size(), header)) {
                message.type = header.type;
                message.data.erase(message.data.begin(), message.data.begin() + sizeof(MessageHeader));
            } else {
                message.type = MessageType::Unknown;
            }
        } else {
            message.type = MessageType::ServerInfo;
        }

        std::lock_guard<std::mutex> lock(m_receiveMutex);
        m_receiveQueue.push(std::move(message));
    });

    websocketpp::lib::error_code ec;
    auto con = m_client.get_connection(url, ec);
    if (ec) {
        if (m_connectCallback) m_connectCallback(false);
        return;
    }

    m_connection = con->get_handle();
    m_client.connect(con);
    m_wsThread.reset(new websocketpp::lib::thread([this]() { m_client.run(); }));
}

void FinalverseClient::subscribeToServices(const std::vector<std::string>& services)
{
    if (!m_connected) return;
    std::string payload = "subscribe"; // placeholder
    websocketpp::lib::error_code ec;
    m_client.send(m_connection, payload, websocketpp::frame::opcode::text, ec);
}

void FinalverseClient::disconnect()
{
    m_running = false;

    websocketpp::lib::error_code ec;
    m_client.close(m_connection, websocketpp::close::status::going_away, "", ec);
    if (m_wsThread && m_wsThread->joinable()) {
        m_wsThread->join();
    }

    m_connected = false;
}

void FinalverseClient::sendPlayerPosition(const float3& position, const float4& rotation)
{
    PositionUpdate update{};
    update.position = position;
    update.rotation = rotation;
    update.velocity = float3{0.f, 0.f, 0.f};
    update.timestamp = 0.0;

    auto payload = MessageSerializer::serialize(update);
    sendMessage(MessageType::PositionUpdate, payload.data(), payload.size());
}

void FinalverseClient::sendMessage(MessageType type, const void* data, size_t size)
{
    if (!m_connected) return;

    MessageHeader header{};
    header.type = type;
    header.length = static_cast<uint32_t>(size);
    header.timestamp = 0;

    auto headerBuf = MessageSerializer::serialize(header);
    std::vector<uint8_t> buffer;
    buffer.insert(buffer.end(), headerBuf.begin(), headerBuf.end());
    buffer.insert(buffer.end(), (const uint8_t*)data, (const uint8_t*)data + size);

    websocketpp::lib::error_code ec;
    m_client.send(m_connection, buffer.data(), buffer.size(), websocketpp::frame::opcode::binary, ec);
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
        case MessageType::WorldUpdate: {
            WorldUpdate update;
            if (MessageSerializer::deserialize(msg.data.data(), msg.data.size(), update)) {
                if (m_worldManager) {
                    // m_worldManager->processWorldUpdate(update);
                }
            }
            break;
        }
        case MessageType::EntityUpdate:
            if (m_worldManager) {
                handleEntityUpdate(msg);
            }
            break;
        case MessageType::ServerInfo: {
            if (m_dataVisualizer) {
                ServiceMetrics metrics{};
                std::stringstream ss(std::string(msg.data.begin(), msg.data.end()));
                ss >> metrics.cpuUsage >> metrics.memoryUsage >> metrics.activeConnections;
                float health = m_dataVisualizer->computeHealthScore(metrics);
                (void)health;
            }
            break;
        }
        default:
            break;
    }
}

} // namespace FinalStorm