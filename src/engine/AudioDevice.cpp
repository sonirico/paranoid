#include "engine/AudioDevice.hpp"

#include <SDL3/SDL.h>

namespace engine
{

AudioDevice::AudioDevice()
{
    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

    if (m_device == 0)
    {
        SDL_Log("SDL_OpenAudioDevice failed: %s", SDL_GetError());
    }
}

AudioDevice::~AudioDevice()
{
    if (m_device != 0)
    {
        SDL_CloseAudioDevice(m_device);
    }
}

bool AudioDevice::isOpen() const
{
    return m_device != 0;
}

SDL_AudioDeviceID AudioDevice::getId() const
{
    return m_device;
}

} // namespace engine
