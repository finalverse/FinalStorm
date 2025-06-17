// ServiceVisualizations.h - Visual representations for different service types
#pragma once

#include "SceneNode.h"
#include "../Core/Math/Math.h"
#include <random>

namespace FinalStorm {

// Base class for service visualizations
class ServiceVisualization : public ServiceNode {
public:
    ServiceVisualization(const ServiceInfo& info) : 
        ServiceNode(info),
        baseColor(0.0f, 0.8f, 1.0f),
        glowColor(0.0f, 1.0f, 1.0f) {
        
        // Create base components
        createBaseStructure();
        createActivityIndicators();
        createInfoDisplay();
    }

protected:
    virtual void createBaseStructure() = 0;
    virtual void createActivityIndicators() = 0;
    
    void createInfoDisplay() {
        // Create holographic info panel
        auto infoPanel = std::make_shared<UI3DPanel>(2.0f, 1.0f);
        infoPanel->setPosition(make_float3(0, 2.5f, 0));
        infoPanel->setText(getServiceInfo().name);
        infoPanel->setBackgroundColor(make_float4(0.05f, 0.05f, 0.1f, 0.7f));
        addChild(infoPanel);
        
        this->infoPanel = infoPanel;
    }
    
    void updateVisualComponents() override {
        // Update glow based on activity
        float glow = getGlowIntensity();
        
        // Update particle emission rate
        if (activityParticles) {
            auto params = activityParticles->getParams();
            params.emitRate = 5.0f + getActivityLevel() * 50.0f;
            activityParticles->setParams(params);
        }
        
        // Update info display
        if (infoPanel) {
            auto& metrics = getMetrics();
            std::stringstream ss;
            ss << getServiceInfo().name << "\n"
               << "CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%\n"
               << "Mem: " << std::fixed << std::setprecision(1) << metrics.memoryUsage << "%\n"
               << "Connections: " << metrics.activeConnections;
            infoPanel->setText(ss.str());
        }
    }

protected:
    float3 baseColor;
    float3 glowColor;
    std::shared_ptr<ParticleSystemNode> activityParticles;
    std::shared_ptr<UI3DPanel> infoPanel;
    
    const ServiceMetrics& getMetrics() const { return metrics; }
    
private:
    ServiceMetrics metrics;
};

// Web Server Visualization - Crystalline structure
class WebServerVisualization : public ServiceVisualization {
public:
    WebServerVisualization(const ServiceInfo& info) : 
        ServiceVisualization(info),
        rotationSpeed(0.5f) {
        baseColor = make_float3(0.0f, 0.7f, 1.0f);
        glowColor = make_float3(0.0f, 1.0f, 1.0f);
    }

protected:
    void createBaseStructure() override {
        // Central crystal core
        auto core = std::make_shared<MeshNode>();
        core->setMesh(MeshLibrary::OCTAHEDRON);
        core->setScale(make_float3(1.0f));
        addChild(core);
        this->core = core;
        
        // Outer crystal shell
        auto shell = std::make_shared<MeshNode>();
        shell->setMesh(MeshLibrary::ICOSAHEDRON);
        shell->setScale(make_float3(1.5f));
        shell->setMaterial(MaterialLibrary::GLASS);
        addChild(shell);
        this->shell = shell;
        
        // Connection nodes
        for (int i = 0; i < 6; ++i) {
            auto connector = std::make_shared<MeshNode>();
            connector->setMesh(MeshLibrary::SPHERE);
            connector->setScale(make_float3(0.2f));
            
            float angle = (i / 6.0f) * 2.0f * M_PI;
            float radius = 2.0f;
            connector->setPosition(make_float3(
                cos(angle) * radius,
                sin(angle * 2.0f) * 0.5f,
                sin(angle) * radius
            ));
            
            addChild(connector);
            connectionNodes.push_back(connector);
        }
    }
    
    void createActivityIndicators() override {
        // Data flow particles
        ParticleSystemNode::ParticleParams params;
        params.emitPosition = make_float3(0, 0, 0);
        params.emitDirection = make_float3(0, 1, 0);
        params.emitAngle = 360.0f;
        params.emitRate = 10.0f;
        params.particleLife = 2.0f;
        params.startSize = 0.1f;
        params.endSize = 0.0f;
        params.startColor = make_float4(baseColor, 1.0f);
        params.endColor = make_float4(glowColor, 0.0f);
        params.speed = 1.0f;
        params.gravity = make_float3(0, 0.2f, 0);
        
        activityParticles = std::make_shared<ParticleSystemNode>(params);
        addChild(activityParticles);
        
        // Connection beams
        for (auto& node : connectionNodes) {
            createConnectionBeam(node->getPosition());
        }
    }
    
    void onUpdate(float deltaTime) override {
        ServiceVisualization::onUpdate(deltaTime);
        
        // Rotate core based on activity
        if (core) {
            float speed = rotationSpeed * (1.0f + getActivityLevel() * 2.0f);
            core->rotate(glm::angleAxis(speed * deltaTime, make_float3(0, 1, 0)));
        }
        
        // Counter-rotate shell
        if (shell) {
            float speed = rotationSpeed * 0.5f;
            shell->rotate(glm::angleAxis(-speed * deltaTime, make_float3(1, 0, 0)));
        }
        
        // Pulse connection nodes
        float pulse = 0.8f + 0.2f * sin(pulseTimer);
        for (auto& node : connectionNodes) {
            node->setScale(make_float3(0.2f * pulse));
        }
        pulseTimer += deltaTime * 3.0f;
        
        // Update connection beams based on network activity
        updateConnectionBeams();
    }
    
private:
    void createConnectionBeam(const float3& target) {
        // Create laser-like beam effect
        auto beam = std::make_shared<MeshNode>();
        beam->setMesh(MeshLibrary::CYLINDER);
        beam->setScale(make_float3(0.05f, 1.0f, 0.05f));
        beam->setMaterial(MaterialLibrary::EMISSIVE);
        addChild(beam);
        connectionBeams.push_back(beam);
    }
    
    void updateConnectionBeams() {
        // Animate beams based on network traffic
        for (size_t i = 0; i < connectionBeams.size(); ++i) {
            auto& beam = connectionBeams[i];
            float activity = getMetrics().networkIn / 100.0f;
            float scale = 0.02f + activity * 0.08f;
            beam->setScale(make_float3(scale, 1.0f, scale));
        }
    }
    
    std::shared_ptr<MeshNode> core;
    std::shared_ptr<MeshNode> shell;
    std::vector<std::shared_ptr<MeshNode>> connectionNodes;
    std::vector<std::shared_ptr<MeshNode>> connectionBeams;
    float rotationSpeed;
    float pulseTimer = 0.0f;
};

// Database Visualization - Layered disc structure
class DatabaseVisualization : public ServiceVisualization {
public:
    DatabaseVisualization(const ServiceInfo& info) : 
        ServiceVisualization(info),
        numLayers(5) {
        baseColor = make_float3(0.2f, 0.8f, 0.2f);
        glowColor = make_float3(0.4f, 1.0f, 0.4f);
    }

protected:
    void createBaseStructure() override {
        // Create stacked disc layers
        for (int i = 0; i < numLayers; ++i) {
            auto layer = std::make_shared<MeshNode>();
            layer->setMesh(MeshLibrary::CYLINDER);
            layer->setScale(make_float3(1.5f - i * 0.1f, 0.1f, 1.5f - i * 0.1f));
            layer->setPosition(make_float3(0, i * 0.3f, 0));
            
            addChild(layer);
            layers.push_back(layer);
        }
        
        // Central data core
        auto dataCore = std::make_shared<MeshNode>();
        dataCore->setMesh(MeshLibrary::SPHERE);
        dataCore->setScale(make_float3(0.3f));
        dataCore->setPosition(make_float3(0, numLayers * 0.3f + 0.5f, 0));
        dataCore->setMaterial(MaterialLibrary::EMISSIVE);
        addChild(dataCore);
        this->dataCore = dataCore;
        
        // Query visualization rings
        for (int i = 0; i < 3; ++i) {
            auto ring = std::make_shared<MeshNode>();
            ring->setMesh(MeshLibrary::TORUS);
            ring->setScale(make_float3(2.0f + i * 0.5f, 0.05f, 2.0f + i * 0.5f));
            ring->setPosition(make_float3(0, 0.5f, 0));
            addChild(ring);
            queryRings.push_back(ring);
        }
    }
    
    void createActivityIndicators() override {
        // Data access particles - vertical streams
        ParticleSystemNode::ParticleParams params;
        params.emitPosition = make_float3(0, 0, 0);
        params.emitDirection = make_float3(0, 1, 0);
        params.emitAngle = 10.0f;
        params.emitRate = 20.0f;
        params.particleLife = 1.5f;
        params.startSize = 0.05f;
        params.endSize = 0.02f;
        params.startColor = make_float4(baseColor, 1.0f);
        params.endColor = make_float4(glowColor, 0.0f);
        params.speed = 2.0f;
        params.gravity = make_float3(0, 0, 0);
        
        activityParticles = std::make_shared<ParticleSystemNode>(params);
        addChild(activityParticles);
        
        // Index structure visualizations
        createIndexStructures();
    }
    
    void onUpdate(float deltaTime) override {
        ServiceVisualization::onUpdate(deltaTime);
        
        // Rotate layers at different speeds
        for (size_t i = 0; i < layers.size(); ++i) {
            float speed = 0.2f + i * 0.1f;
            layers[i]->rotate(glm::angleAxis(speed * deltaTime, make_float3(0, 1, 0)));
        }
        
        // Pulse data core based on query activity
        if (dataCore) {
            float pulse = 0.3f + 0.1f * sin(queryTimer * 5.0f) * getActivityLevel();
            dataCore->setScale(make_float3(pulse));
        }
        
        // Animate query rings
        for (size_t i = 0; i < queryRings.size(); ++i) {
            float offset = i * 2.0f;
            float y = 0.5f + 0.2f * sin(queryTimer + offset);
            queryRings[i]->setPosition(make_float3(0, y, 0));
            queryRings[i]->rotate(glm::angleAxis(0.5f * deltaTime, make_float3(0, 1, 0)));
        }
        
        queryTimer += deltaTime;
    }
    
private:
    void createIndexStructures() {
        // Create geometric patterns representing indices
        int numIndices = 8;
        for (int i = 0; i < numIndices; ++i) {
            auto index = std::make_shared<MeshNode>();
            index->setMesh(MeshLibrary::PYRAMID);
            index->setScale(make_float3(0.1f, 0.3f, 0.1f));
            
            float angle = (i / float(numIndices)) * 2.0f * M_PI;
            float radius = 1.8f;
            index->setPosition(make_float3(
                cos(angle) * radius,
                0.2f,
                sin(angle) * radius
            ));
            index->setRotation(glm::angleAxis(angle, make_float3(0, 1, 0)));
            
            addChild(index);
            indexStructures.push_back(index);
        }
    }
    
    std::vector<std::shared_ptr<MeshNode>> layers;
    std::vector<std::shared_ptr<MeshNode>> queryRings;
    std::vector<std::shared_ptr<MeshNode>> indexStructures;
    std::shared_ptr<MeshNode> dataCore;
    int numLayers;
    float queryTimer = 0.0f;
};

// Blockchain Node Visualization - Chain of interconnected blocks
class BlockchainVisualization : public ServiceVisualization {
public:
    BlockchainVisualization(const ServiceInfo& info) : 
        ServiceVisualization(info),
        numBlocks(7),
        blockSpacing(1.5f) {
        baseColor = make_float3(0.8f, 0.6f, 0.0f);
        glowColor = make_float3(1.0f, 0.8f, 0.0f);
    }

protected:
    void createBaseStructure() override {
        // Create chain of blocks
        for (int i = 0; i < numBlocks; ++i) {
            auto block = std::make_shared<MeshNode>();
            block->setMesh(MeshLibrary::CUBE);
            block->setScale(make_float3(0.8f));
            
            // Arrange in a spiral
            float angle = i * 0.8f;
            float radius = 2.0f;
            float height = i * 0.3f;
            block->setPosition(make_float3(
                cos(angle) * radius,
                height,
                sin(angle) * radius
            ));
            
            addChild(block);
            blocks.push_back(block);
            
            // Create chain links
            if (i > 0) {
                createChainLink(blocks[i-1], block);
            }
        }
        
        // Mining visualization node
        auto miningNode = std::make_shared<MeshNode>();
        miningNode->setMesh(MeshLibrary::ICOSAHEDRON);
        miningNode->setScale(make_float3(0.5f));
        miningNode->setPosition(make_float3(0, numBlocks * 0.3f + 1.0f, 0));
        miningNode->setMaterial(MaterialLibrary::EMISSIVE);
        addChild(miningNode);
        this->miningNode = miningNode;
    }
    
    void createActivityIndicators() override {
        // Mining particles
        ParticleSystemNode::ParticleParams params;
        params.emitPosition = make_float3(0, numBlocks * 0.3f + 1.0f, 0);
        params.emitDirection = make_float3(0, -1, 0);
        params.emitAngle = 45.0f;
        params.emitRate = 30.0f;
        params.particleLife = 1.0f;
        params.startSize = 0.08f;
        params.endSize = 0.02f;
        params.startColor = make_float4(baseColor, 1.0f);
        params.endColor = make_float4(glowColor, 0.0f);
        params.speed = 1.5f;
        params.gravity = make_float3(0, -0.5f, 0);
        
        activityParticles = std::make_shared<ParticleSystemNode>(params);
        addChild(activityParticles);
        
        // Network consensus pulses
        createConsensusPulses();
    }
    
    void onUpdate(float deltaTime) override {
        ServiceVisualization::onUpdate(deltaTime);
        
        // Rotate blocks
        for (auto& block : blocks) {
            block->rotate(glm::angleAxis(0.3f * deltaTime, make_float3(0, 1, 0)));
        }
        
        // Mining animation
        if (miningNode) {
            miningTimer += deltaTime * 2.0f;
            float scale = 0.5f + 0.1f * sin(miningTimer * 5.0f);
            miningNode->setScale(make_float3(scale));
            miningNode->rotate(glm::angleAxis(2.0f * deltaTime, make_float3(1, 1, 0)));
        }
        
        // Consensus pulse animation
        consensusTimer += deltaTime;
        if (consensusTimer > 3.0f) {
            triggerConsensusPulse();
            consensusTimer = 0.0f;
        }
        
        // Update chain links
        updateChainLinks(deltaTime);
    }
    
private:
    void createChainLink(std::shared_ptr<MeshNode> from, std::shared_ptr<MeshNode> to) {
        auto link = std::make_shared<MeshNode>();
        link->setMesh(MeshLibrary::CYLINDER);
        
        float3 fromPos = from->getPosition();
        float3 toPos = to->getPosition();
        float3 diff = toPos - fromPos;
        float length = glm::length(diff);
        
        link->setScale(make_float3(0.05f, length, 0.05f));
        link->setPosition((fromPos + toPos) * 0.5f);
        
        // Orient the link
        float3 up = glm::normalize(diff);
        float3 forward = make_float3(0, 0, 1);
        float3 right = glm::cross(up, forward);
        if (glm::length(right) < 0.001f) {
            forward = make_float3(1, 0, 0);
            right = glm::cross(up, forward);
        }
        right = glm::normalize(right);
        forward = glm::cross(right, up);
        
        float4x4 orientation = matrix_identity();
        orientation.columns[0] = make_float4(right.x, right.y, right.z, 0);
        orientation.columns[1] = make_float4(up.x, up.y, up.z, 0);
        orientation.columns[2] = make_float4(forward.x, forward.y, forward.z, 0);

        link->setRotation(glm::quat_cast(*(glm::mat4*)&orientation));
        link->setMaterial(MaterialLibrary::EMISSIVE);
        
        addChild(link);
        chainLinks.push_back(link);
    }
    
    void createConsensusPulses() {
        // Create pulse effect nodes
        for (int i = 0; i < 3; ++i) {
            auto pulse = std::make_shared<MeshNode>();
            pulse->setMesh(MeshLibrary::SPHERE);
            pulse->setScale(make_float3(0.0f));
            pulse->setMaterial(MaterialLibrary::EMISSIVE);
            addChild(pulse);
            consensusPulses.push_back(pulse);
        }
    }
    
    void triggerConsensusPulse() {
        // Find inactive pulse and activate it
        for (auto& pulse : consensusPulses) {
            if (pulse->getScale().x < 0.01f) {
                pulse->setPosition(blocks[0]->getPosition());
                pulse->setScale(make_float3(0.1f));
                // Animation will be handled in updateChainLinks
                break;
            }
        }
    }
    
    void updateChainLinks(float deltaTime) {
        // Pulse effect through chain
        for (auto& pulse : consensusPulses) {
            float3 scale = pulse->getScale();
            if (scale.x > 0.01f) {
                scale *= (1.0f + deltaTime * 3.0f);
                pulse->setScale(scale);
                
                if (scale.x > 5.0f) {
                    pulse->setScale(make_float3(0.0f));
                }
            }
        }
        
        // Glow chain links based on activity
        float glow = 0.5f + 0.5f * getActivityLevel();
        for (auto& link : chainLinks) {
            // Update material emission based on activity
        }
    }
    
    std::vector<std::shared_ptr<MeshNode>> blocks;
    std::vector<std::shared_ptr<MeshNode>> chainLinks;
    std::vector<std::shared_ptr<MeshNode>> consensusPulses;
    std::shared_ptr<MeshNode> miningNode;
    int numBlocks;
    float blockSpacing;
    float miningTimer = 0.0f;
    float consensusTimer = 0.0f;
};

// AI Service Visualization - Neural network
class AIServiceVisualization : public ServiceVisualization {
public:
    AIServiceVisualization(const ServiceInfo& info) : 
        ServiceVisualization(info) {
        baseColor = make_float3(0.8f, 0.0f, 0.8f);
        glowColor = make_float3(1.0f, 0.2f, 1.0f);
    }

protected:
    void createBaseStructure() override {
        // Create neural network layers
        int layerSizes[] = {4, 6, 6, 3};
        int numLayers = 4;
        float layerSpacing = 1.5f;
        
        for (int layer = 0; layer < numLayers; ++layer) {
            std::vector<std::shared_ptr<MeshNode>> layerNodes;
            
            for (int n = 0; n < layerSizes[layer]; ++n) {
                auto neuron = std::make_shared<MeshNode>();
                neuron->setMesh(MeshLibrary::SPHERE);
                neuron->setScale(make_float3(0.2f));
                
                float y = (n - layerSizes[layer] / 2.0f) * 0.8f;
                float x = (layer - numLayers / 2.0f) * layerSpacing;
                neuron->setPosition(make_float3(x, y, 0));
                
                addChild(neuron);
                layerNodes.push_back(neuron);
            }
            
            neurons.push_back(layerNodes);
            
            // Create connections to previous layer
            if (layer > 0) {
                createLayerConnections(neurons[layer-1], layerNodes);
            }
        }
        
        // Central processing core
        auto core = std::make_shared<MeshNode>();
        core->setMesh(MeshLibrary::OCTAHEDRON);
        core->setScale(make_float3(0.5f));
        core->setPosition(make_float3(0, 0, -1.0f));
        core->setMaterial(MaterialLibrary::EMISSIVE);
        addChild(core);
        this->processingCore = core;
    }
    
    void createActivityIndicators() override {
        // Thought bubble particles
        ParticleSystemNode::ParticleParams params;
        params.emitPosition = make_float3(0, 2.0f, 0);
        params.emitDirection = make_float3(0, 1, 0);
        params.emitAngle = 30.0f;
        params.emitRate = 15.0f;
        params.particleLife = 2.0f;
        params.startSize = 0.15f;
        params.endSize = 0.3f;
        params.startColor = make_float4(baseColor, 0.8f);
        params.endColor = make_float4(glowColor, 0.0f);
        params.speed = 0.5f;
        params.gravity = make_float3(0, 0.1f, 0);
        
        activityParticles = std::make_shared<ParticleSystemNode>(params);
        addChild(activityParticles);
    }
    
    void onUpdate(float deltaTime) override {
        ServiceVisualization::onUpdate(deltaTime);
        
        // Neural activity simulation
        neuralTimer += deltaTime;
        
        // Activate neurons in waves
        for (size_t layer = 0; layer < neurons.size(); ++layer) {
            for (size_t n = 0; n < neurons[layer].size(); ++n) {
                float phase = neuralTimer * 2.0f - layer * 0.5f - n * 0.1f;
                float activation = (sin(phase) + 1.0f) * 0.5f;
                float scale = 0.15f + activation * 0.15f * getActivityLevel();
                neurons[layer][n]->setScale(make_float3(scale));
                
                // Glow effect based on activation
                // Would update material emission here
            }
        }
        
        // Rotate and pulse processing core
        if (processingCore) {
            processingCore->rotate(glm::angleAxis(deltaTime, make_float3(1, 1, 1)));
            float pulse = 0.5f + 0.2f * sin(neuralTimer * 3.0f);
            processingCore->setScale(make_float3(pulse));
        }
        
        // Update synapse connections
        updateSynapses(deltaTime);
    }
    
private:
    void createLayerConnections(
        const std::vector<std::shared_ptr<MeshNode>>& fromLayer,
        const std::vector<std::shared_ptr<MeshNode>>& toLayer) {
        
        std::vector<std::shared_ptr<MeshNode>> layerSynapses;
        
        // Create connections with some randomness
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        for (auto& from : fromLayer) {
            for (auto& to : toLayer) {
                if (dis(gen) < 0.7f) { // 70% connection probability
                    auto synapse = std::make_shared<MeshNode>();
                    synapse->setMesh(MeshLibrary::CYLINDER);
                    
                    float3 fromPos = from->getPosition();
                    float3 toPos = to->getPosition();
                    float3 diff = toPos - fromPos;
                    float length = glm::length(diff);
                    
                    synapse->setScale(make_float3(0.01f, length, 0.01f));
                    synapse->setPosition((fromPos + toPos) * 0.5f);
                    
                    // Orient the synapse
                    float3 up = glm::normalize(diff);
                    glm::quat rotation = glm::rotation(make_float3(0, 1, 0), up);
                    synapse->setRotation(rotation);
                    
                    synapse->setMaterial(MaterialLibrary::EMISSIVE);
                    addChild(synapse);
                    layerSynapses.push_back(synapse);
                }
            }
        }
        
        synapses.push_back(layerSynapses);
    }
    
    void updateSynapses(float deltaTime) {
        // Pulse synapses to show signal propagation
        for (size_t layer = 0; layer < synapses.size(); ++layer) {
            for (size_t s = 0; s < synapses[layer].size(); ++s) {
                float phase = neuralTimer * 3.0f - layer * 0.8f - s * 0.05f;
                float intensity = (sin(phase) + 1.0f) * 0.5f;
                float scale = 0.005f + intensity * 0.015f * getActivityLevel();
                
                auto& synapse = synapses[layer][s];
                float3 currentScale = synapse->getScale();
                currentScale.x = scale;
                currentScale.z = scale;
                synapse->setScale(currentScale);
            }
        }
    }
    
    std::vector<std::vector<std::shared_ptr<MeshNode>>> neurons;
    std::vector<std::vector<std::shared_ptr<MeshNode>>> synapses;
    std::shared_ptr<MeshNode> processingCore;
    float neuralTimer = 0.0f;
};

// Factory for creating service visualizations
class ServiceVisualizationFactory {
public:
    static std::shared_ptr<ServiceVisualization> create(const ServiceNode::ServiceInfo& info) {
        if (info.type == "web_server") {
            return std::make_shared<WebServerVisualization>(info);
        } else if (info.type == "database") {
            return std::make_shared<DatabaseVisualization>(info);
        } else if (info.type == "blockchain") {
            return std::make_shared<BlockchainVisualization>(info);
        } else if (info.type == "ai_service") {
            return std::make_shared<AIServiceVisualization>(info);
        } else {
            // Default visualization
            return std::make_shared<WebServerVisualization>(info);
        }
    }
};

// Service discovery interface
class ServiceDiscoveryUI : public SceneNode {
public:
    ServiceDiscoveryUI() : 
        expanded(false),
        animationTime(0.0f) {
        createDiscoveryOrb();
    }
    
    void setAvailableServices(const std::vector<ServiceNode::ServiceInfo>& services) {
        this->availableServices = services;
        if (expanded) {
            refreshServiceRing();
        }
    }
    
    std::function<void(const ServiceNode::ServiceInfo&)> onServiceSelected;
    
protected:
    void onUpdate(float deltaTime) override {
        animationTime += deltaTime;
        
        // Pulse discovery orb
        if (discoveryOrb) {
            float pulse = 0.8f + 0.2f * sin(animationTime * 2.0f);
            discoveryOrb->setScale(make_float3(pulse));
            discoveryOrb->rotate(glm::angleAxis(deltaTime * 0.5f, make_float3(0, 1, 0)));
        }
        
        // Animate service ring
        if (expanded && serviceRing) {
            serviceRing->rotate(glm::angleAxis(deltaTime * 0.2f, make_float3(0, 1, 0)));
        }
    }
    
private:
    void createDiscoveryOrb() {
        // Central interactive orb
        discoveryOrb = std::make_shared<MeshNode>();
        discoveryOrb->setMesh(MeshLibrary::SPHERE);
        discoveryOrb->setScale(make_float3(0.5f));
        discoveryOrb->setMaterial(MaterialLibrary::EMISSIVE);
        addChild(discoveryOrb);
        
        // Add glow effect
        auto glowRing = std::make_shared<MeshNode>();
        glowRing->setMesh(MeshLibrary::TORUS);
        glowRing->setScale(make_float3(0.7f, 0.1f, 0.7f));
        glowRing->setMaterial(MaterialLibrary::EMISSIVE);
        discoveryOrb->addChild(glowRing);
    }
    
    void toggleExpanded() {
        expanded = !expanded;
        if (expanded) {
            createServiceRing();
        } else {
            if (serviceRing) {
                removeChild(serviceRing);
                serviceRing = nullptr;
            }
        }
    }
    
    void createServiceRing() {
        serviceRing = std::make_shared<SceneNode>();
        
        float angleStep = 2.0f * M_PI / availableServices.size();
        float radius = 3.0f;
        
        for (size_t i = 0; i < availableServices.size(); ++i) {
            auto serviceIcon = createServiceIcon(availableServices[i]);
            
            float angle = i * angleStep;
            serviceIcon->setPosition(make_float3(
                cos(angle) * radius,
                0,
                sin(angle) * radius
            ));
            
            serviceRing->addChild(serviceIcon);
        }
        
        addChild(serviceRing);
    }
    
    void refreshServiceRing() {
        if (serviceRing) {
            removeChild(serviceRing);
        }
        createServiceRing();
    }
    
    std::shared_ptr<SceneNode> createServiceIcon(const ServiceNode::ServiceInfo& info) {
        auto icon = std::make_shared<SceneNode>();
        
        // Icon mesh based on service type
        auto mesh = std::make_shared<MeshNode>();
        if (info.type == "web_server") {
            mesh->setMesh(MeshLibrary::OCTAHEDRON);
        } else if (info.type == "database") {
            mesh->setMesh(MeshLibrary::CYLINDER);
        } else if (info.type == "blockchain") {
            mesh->setMesh(MeshLibrary::CUBE);
        } else if (info.type == "ai_service") {
            mesh->setMesh(MeshLibrary::ICOSAHEDRON);
        } else {
            mesh->setMesh(MeshLibrary::SPHERE);
        }
        
        mesh->setScale(make_float3(0.3f));
        icon->addChild(mesh);
        
        // Label
        auto label = std::make_shared<UI3DPanel>(1.5f, 0.5f);
        label->setText(info.name);
        label->setPosition(make_float3(0, 0.5f, 0));
        label->setBackgroundColor(make_float4(0.1f, 0.1f, 0.2f, 0.8f));
        icon->addChild(label);
        
        // Set up interaction
        label->onPress = [this, info]() {
            if (onServiceSelected) {
                onServiceSelected(info);
            }
        };
        
        return icon;
    }
    
    std::shared_ptr<MeshNode> discoveryOrb;
    std::shared_ptr<SceneNode> serviceRing;
    std::vector<ServiceNode::ServiceInfo> availableServices;
    bool expanded;
    float animationTime;
};

// Environment controller for the scene
class FinalverseEnvironment : public SceneNode {
public:
    FinalverseEnvironment() {
        createSkybox();
        createGroundGrid();
        createAmbientEffects();
    }
    
    void updateSystemHealth(float healthScore) {
        // Adjust environment based on system health (0-1)
        systemHealth = healthScore;
        
        // Update skybox tint
        float3 healthyColor(0.0f, 0.1f, 0.2f);
        float3 warningColor(0.2f, 0.1f, 0.0f);
        float3 criticalColor(0.2f, 0.0f, 0.0f);
        
        float3 skyTint;
        if (healthScore > 0.7f) {
            skyTint = healthyColor;
        } else if (healthScore > 0.3f) {
            float t = (healthScore - 0.3f) / 0.4f;
            skyTint = glm::mix(warningColor, healthyColor, t);
        } else {
            float t = healthScore / 0.3f;
            skyTint = glm::mix(criticalColor, warningColor, t);
        }
        
        // Apply tint to skybox material
        if (skybox) {
            // Update skybox material color
        }
    }
    
    void setNetworkActivity(float activity) {
        // Update particle effects based on network activity
        if (networkParticles) {
            auto params = networkParticles->getParams();
            params.emitRate = 10.0f + activity * 90.0f;
            networkParticles->setParams(params);
        }
    }
    
protected:
    void onUpdate(float deltaTime) override {
        environmentTime += deltaTime;
        
        // Pulse ground grid
        if (groundGrid) {
            float pulse = 0.5f + 0.5f * sin(environmentTime * 0.5f) * systemHealth;
            // Update grid material emission
        }
        
        // Update ambient effects
        updateAmbientEffects(deltaTime);
    }
    
private:
    void createSkybox() {
        skybox = std::make_shared<MeshNode>();
        skybox->setMesh(MeshLibrary::SKYBOX);
        skybox->setScale(make_float3(500.0f));
        skybox->setMaterial(MaterialLibrary::SKYBOX);
        addChild(skybox);
    }
    
    void createGroundGrid() {
        groundGrid = std::make_shared<MeshNode>();
        groundGrid->setMesh(MeshLibrary::GRID);
        groundGrid->setScale(make_float3(100.0f, 1.0f, 100.0f));
        groundGrid->setPosition(make_float3(0, -5.0f, 0));
        groundGrid->setMaterial(MaterialLibrary::GRID);
        addChild(groundGrid);
    }
    
    void createAmbientEffects() {
        // Network activity particles
        ParticleSystemNode::ParticleParams params;
        params.emitPosition = make_float3(0, -5.0f, 0);
        params.emitDirection = make_float3(0, 1, 0);
        params.emitAngle = 180.0f;
        params.emitRate = 20.0f;
        params.particleLife = 10.0f;
        params.startSize = 0.05f;
        params.endSize = 0.01f;
        params.startColor = make_float4(0.0f, 0.5f, 1.0f, 0.5f);
        params.endColor = make_float4(0.0f, 0.8f, 1.0f, 0.0f);
        params.speed = 2.0f;
        params.gravity = make_float3(0, 0.5f, 0);
        
        networkParticles = std::make_shared<ParticleSystemNode>(params);
        addChild(networkParticles);
        
        // Floating data motes
        createDataMotes();
    }
    
    void createDataMotes() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> posDist(-50.0, 50.0);
        std::uniform_real_distribution<> heightDist(0.0, 20.0);
        
        for (int i = 0; i < 50; ++i) {
            auto mote = std::make_shared<MeshNode>();
            mote->setMesh(MeshLibrary::SPHERE);
            mote->setScale(make_float3(0.02f));
            mote->setPosition(make_float3(
                posDist(gen),
                heightDist(gen),
                posDist(gen)
            ));
            mote->setMaterial(MaterialLibrary::EMISSIVE);
            
            addChild(mote);
            dataMotes.push_back(mote);
        }
    }
    
    void updateAmbientEffects(float deltaTime) {
        // Animate data motes
        for (size_t i = 0; i < dataMotes.size(); ++i) {
            auto& mote = dataMotes[i];
            float3 pos = mote->getPosition();
            
            // Floating motion
            float phase = environmentTime + i * 0.3f;
            pos.y += sin(phase) * deltaTime * 0.5f;
            
            // Drift
            pos.x += sin(phase * 0.7f) * deltaTime * 0.3f;
            pos.z += cos(phase * 0.8f) * deltaTime * 0.3f;
            
            // Wrap around
            if (pos.x > 50.0f) pos.x = -50.0f;
            if (pos.x < -50.0f) pos.x = 50.0f;
            if (pos.z > 50.0f) pos.z = -50.0f;
            if (pos.z < -50.0f) pos.z = 50.0f;
            if (pos.y > 20.0f) pos.y = 0.0f;
            if (pos.y < 0.0f) pos.y = 20.0f;
            
            mote->setPosition(pos);
        }
    }
    
    std::shared_ptr<MeshNode> skybox;
    std::shared_ptr<MeshNode> groundGrid;
    std::shared_ptr<ParticleSystemNode> networkParticles;
    std::vector<std::shared_ptr<MeshNode>> dataMotes;
    float systemHealth = 1.0f;
    float environmentTime = 0.0f;
};

// Mesh library IDs
struct MeshLibrary {
    static constexpr uint32_t CUBE = 1;
    static constexpr uint32_t SPHERE = 2;
    static constexpr uint32_t CYLINDER = 3;
    static constexpr uint32_t OCTAHEDRON = 4;
    static constexpr uint32_t ICOSAHEDRON = 5;
    static constexpr uint32_t TORUS = 6;
    static constexpr uint32_t PYRAMID = 7;
    static constexpr uint32_t GRID = 8;
    static constexpr uint32_t SKYBOX = 9;
};

// Material library IDs
struct MaterialLibrary {
    static constexpr uint32_t DEFAULT = 1;
    static constexpr uint32_t EMISSIVE = 2;
    static constexpr uint32_t GLASS = 3;
    static constexpr uint32_t GRID = 4;
    static constexpr uint32_t SKYBOX = 5;
};

} // namespace FinalStorm

