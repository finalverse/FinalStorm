// Core/ResourceManager.h
#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <typeindex>
#include <mutex>

namespace FinalStorm {

class Resource {
public:
    virtual ~Resource() = default;
    virtual bool load(const std::string& path) = 0;
    virtual void unload() = 0;
    virtual bool isLoaded() const = 0;
    
    const std::string& getPath() const { return m_path; }
    
protected:
    std::string m_path;
    bool m_loaded = false;
};

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    template<typename T>
    std::shared_ptr<T> load(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // Check if already loaded
        auto it = m_resources.find(path);
        if (it != m_resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        
        // Create and load new resource
        auto resource = std::make_shared<T>();
        if (resource->load(path)) {
            m_resources[path] = resource;
            return resource;
        }
        
        return nullptr;
    }
    
    template<typename T>
    std::shared_ptr<T> get(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_resources.find(path);
        if (it != m_resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        
        return nullptr;
    }
    
    void unload(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_resources.find(path);
        if (it != m_resources.end()) {
            it->second->unload();
            m_resources.erase(it);
        }
    }
    
    void unloadAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pair : m_resources) {
            pair.second->unload();
        }
        m_resources.clear();
    }
    
    size_t getResourceCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_resources.size();
    }
    
private:
    ResourceManager() = default;
    ~ResourceManager() {
        unloadAll();
    }
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
};

} // namespace FinalStorm