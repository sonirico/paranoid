#include "engine/Sound.hpp"

#include "engine/AudioDevice.hpp"
#include "engine/SoundBuffer.hpp"

#include <SDL3/SDL.h>

namespace engine
{

Sound::Sound(AudioDevice& device) : m_device(device) {}

Sound::~Sound()
{
    if (m_stream)
    {
        SDL_DestroyAudioStream(m_stream);
    }
}

void Sound::setBuffer(const SoundBuffer& buffer)
{
    m_buffer = &buffer;

    if (m_stream)
    {
        SDL_DestroyAudioStream(m_stream);
        m_stream = nullptr;
    }

    if (!m_device.isOpen())
    {
        return;
    }

    // The device end of the spec is null: SDL converts to whatever
    // format the device runs at.
    m_stream = SDL_CreateAudioStream(&buffer.getSpec(), nullptr);

    if (!m_stream || !SDL_BindAudioStream(m_device.getId(), m_stream))
    {
        SDL_Log("Sound setup failed: %s", SDL_GetError());
    }
}

void Sound::setVolume(float volume)
{
    if (m_stream)
    {
        SDL_SetAudioStreamGain(m_stream, volume / 100.f);
    }
}

void Sound::play()
{
    if (!m_stream || !m_buffer)
    {
        return;
    }

    // Restart from the beginning, dropping whatever was still queued.
    SDL_ClearAudioStream(m_stream);
    SDL_PutAudioStreamData(m_stream, m_buffer->getData(), static_cast<int>(m_buffer->getSize()));
}

} // namespace engine
