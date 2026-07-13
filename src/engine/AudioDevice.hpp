#pragma once

#include <SDL3/SDL_audio.h>

namespace engine
{

// Opens the default playback device. Sounds bind their streams to it;
// SDL mixes every bound stream automatically.
class AudioDevice
{
  public:
    AudioDevice();
    ~AudioDevice();

    AudioDevice(const AudioDevice&) = delete;
    AudioDevice& operator=(const AudioDevice&) = delete;

    bool isOpen() const;
    SDL_AudioDeviceID getId() const;

  private:
    SDL_AudioDeviceID m_device = 0;
};

} // namespace engine
