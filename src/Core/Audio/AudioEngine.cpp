//
//  AudioEngine.cpp
//  FinalStorm
//
//  Created by Wenyan Qin on 2025-06-16.
//

#include "Core/Audio/AudioEngine.h"
#include <iostream>

namespace FinalStorm {

// AudioListener (internal class)
class AudioListener {
public:
    float3 position;
    float3 velocity;
    float3 forward;
    float3 up;
    
    AudioListener() 
        : position(simd_make_float3(0.0f, 0.0f, 0.0f))
        , velocity(simd_make_float3(0.0f, 0.0f, 0.0f))
        , forward(simd_make_float3(0.0f, 0.0f, -1.0f))
        , up(simd_make_float3(0.0f, 1.0f, 0.0f)) {}
};

// AudioEngine implementation
AudioEngine::AudioEngine()
    : m_initialized(false)
    , m_masterVolume(1.0f)
    , m_audioContext(nullptr)
{
}

AudioEngine::~AudioEngine() {
    if (m_initialized) {
        shutdown();
    }
}

bool AudioEngine::initialize() {
    // Platform-specific initialization would go here
    // For now, we'll just set up the basic structure
    
    m_listener = std::make_unique<AudioListener>();
    m_initialized = true;
    
    std::cout << "Audio Engine initialized" << std::endl;
    return true;
}

void AudioEngine::shutdown() {
    if (!m_initialized) return;
    
    // Clean up all sources
    m_sources.clear();
    m_audioClips.clear();
    
    m_initialized = false;
    std::cout << "Audio Engine shut down" << std::endl;
}

bool AudioEngine::loadAudioClip(const std::string& filename, const std::string& name) {
    // This would load actual audio data
    // For now, create a dummy clip
    auto clip = std::make_shared<AudioClip>();
    clip->name = name;
    clip->format = AudioFormat::Stereo16;
    clip->sampleRate = 44100;
    clip->duration = 1.0f;
    
    m_audioClips[name] = clip;
    return true;
}

std::shared_ptr<AudioClip> AudioEngine::getAudioClip(const std::string& name) const {
    auto it = m_audioClips.find(name);
    return (it != m_audioClips.end()) ? it->second : nullptr;
}

std::shared_ptr<AudioSource> AudioEngine::createSource() {
    auto source = std::make_shared<AudioSource>();
    m_sources.push_back(source);
    return source;
}

void AudioEngine::removeSource(std::shared_ptr<AudioSource> source) {
    m_sources.erase(
        std::remove(m_sources.begin(), m_sources.end(), source),
        m_sources.end()
    );
}

void AudioEngine::setListenerPosition(const float3& position) {
    if (m_listener) {
        m_listener->position = position;
    }
}

void AudioEngine::setListenerOrientation(const float3& forward, const float3& up) {
    if (m_listener) {
        m_listener->forward = forward;
        m_listener->up = up;
    }
}

void AudioEngine::setListenerVelocity(const float3& velocity) {
    if (m_listener) {
        m_listener->velocity = velocity;
    }
}

void AudioEngine::setMasterVolume(float volume) {
    m_masterVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioEngine::update(float deltaTime) {
    // Update all active sources
    for (auto& source : m_sources) {
        if (source->isPlaying()) {
            // Update 3D audio calculations
            // This would involve calculating attenuation, panning, etc.
        }
    }
}

void AudioEngine::playSound2D(const std::string& clipName, float volume) {
    auto clip = getAudioClip(clipName);
    if (!clip) return;
    
    auto source = createSource();
    source->setClip(clip);
    source->setVolume(volume * m_masterVolume);
    source->play();
}

void AudioEngine::playSound3D(const std::string& clipName, const float3& position, float volume) {
    auto clip = getAudioClip(clipName);
    if (!clip) return;
    
    auto source = createSource();
    source->setClip(clip);
    source->setPosition(position);
    source->setVolume(volume * m_masterVolume);
    source->play();
}

// AudioSource implementation
AudioSource::AudioSource()
    : m_position(simd_make_float3(0.0f, 0.0f, 0.0f))
    , m_velocity(simd_make_float3(0.0f, 0.0f, 0.0f))
    , m_volume(1.0f)
    , m_pitch(1.0f)
    , m_looping(false)
    , m_playing(false)
    , m_referenceDistance(1.0f)
    , m_maxDistance(100.0f)
    , m_sourceId(0)
{
}

AudioSource::~AudioSource() {
    stop();
}

void AudioSource::setClip(std::shared_ptr<AudioClip> clip) {
    m_clip = clip;
}

void AudioSource::setPosition(const float3& position) {
    m_position = position;
}

void AudioSource::setVelocity(const float3& velocity) {
    m_velocity = velocity;
}

void AudioSource::setVolume(float volume) {
    m_volume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioSource::setPitch(float pitch) {
    m_pitch = std::max(0.1f, std::min(2.0f, pitch));
}

void AudioSource::setLooping(bool loop) {
    m_looping = loop;
}

void AudioSource::setReferenceDistance(float distance) {
    m_referenceDistance = std::max(0.1f, distance);
}

void AudioSource::setMaxDistance(float distance) {
    m_maxDistance = std::max(m_referenceDistance, distance);
}

void AudioSource::play() {
    if (!m_clip) return;
    m_playing = true;
    // Platform-specific play implementation
}

void AudioSource::pause() {
    m_playing = false;
    // Platform-specific pause implementation
}

void AudioSource::stop() {
    m_playing = false;
    // Platform-specific stop implementation
}

bool AudioSource::isPlaying() const {
    return m_playing;
}

} // namespace FinalStorm
