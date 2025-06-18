//
//  AudioEngine.h
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Core/Math/Math.h"

namespace FinalStorm {

class AudioBuffer;
class AudioSource;
class AudioListener;

enum class AudioFormat {
    Mono8,
    Mono16,
    Stereo8,
    Stereo16,
    Float32
};

struct AudioClip {
    std::string name;
    std::shared_ptr<AudioBuffer> buffer;
    AudioFormat format;
    uint32_t sampleRate;
    float duration;
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    // Initialize audio system
    bool initialize();
    void shutdown();
    
    // Audio clip management
    bool loadAudioClip(const std::string& filename, const std::string& name);
    std::shared_ptr<AudioClip> getAudioClip(const std::string& name) const;
    
    // 3D audio sources
    std::shared_ptr<AudioSource> createSource();
    void removeSource(std::shared_ptr<AudioSource> source);
    
    // Listener (camera)
    void setListenerPosition(const float3& position);
    void setListenerOrientation(const float3& forward, const float3& up);
    void setListenerVelocity(const float3& velocity);
    
    // Global settings
    void setMasterVolume(float volume);
    float getMasterVolume() const { return m_masterVolume; }
    
    // Update audio system
    void update(float deltaTime);
    
    // Play sounds
    void playSound2D(const std::string& clipName, float volume = 1.0f);
    void playSound3D(const std::string& clipName, const float3& position, float volume = 1.0f);
    
private:
    bool m_initialized;
    float m_masterVolume;
    
    std::unique_ptr<AudioListener> m_listener;
    std::vector<std::shared_ptr<AudioSource>> m_sources;
    std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_audioClips;
    
    // Platform-specific audio context
    void* m_audioContext;
};

class AudioSource {
public:
    AudioSource();
    ~AudioSource();
    
    void setClip(std::shared_ptr<AudioClip> clip);
    void setPosition(const float3& position);
    void setVelocity(const float3& velocity);
    void setVolume(float volume);
    void setPitch(float pitch);
    void setLooping(bool loop);
    void setReferenceDistance(float distance);
    void setMaxDistance(float distance);
    
    void play();
    void pause();
    void stop();
    bool isPlaying() const;
    
    const float3& getPosition() const { return m_position; }
    float getVolume() const { return m_volume; }
    
private:
    std::shared_ptr<AudioClip> m_clip;
    float3 m_position;
    float3 m_velocity;
    float m_volume;
    float m_pitch;
    bool m_looping;
    bool m_playing;
    float m_referenceDistance;
    float m_maxDistance;
    
    // Platform-specific source handle
    uint32_t m_sourceId;
};

} // namespace FinalStorm
