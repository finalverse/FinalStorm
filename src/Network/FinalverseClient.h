// src/Network/FinalverseClient.h
// Finalverse client interface
// Manages connection to Finalverse server

#pragma once
#include <string>
#include <vector>
#include <memory>

namespace FinalStorm {

class Message;

class FinalverseClient {
public:
    FinalverseClient();
    ~FinalverseClient();
    
    bool connect(const std::string& url);
    void disconnect();
    bool isConnected() const { return connected; }
    
    void update();
    void sendMessage(const Message& message);
    
private:
    std::string serverUrl;
    bool connected;
};

} // namespace FinalStorm