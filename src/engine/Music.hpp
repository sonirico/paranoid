#pragma once

#include <SDL3/SDL_audio.h>

#include <cstdint>
#include <string>

struct SDL_AudioStream;

namespace engine
{

class AudioDevice;

// A looping music track, fully decoded from Ogg Vorbis into memory.
// Call update() once per frame to keep the loop queue topped up.
class Music
{
  public:
    explicit Music(AudioDevice& device);
    ~Music();

    Music(const Music&) = delete;
    Music& operator=(const Music&) = delete;

    bool loadFromFile(const std::string& path);

    // Volume in the 0..100 range.
    void setVolume(float volume);

    void play();
    void stop();
    void update();

    std::uint32_t getSampleCount() const;

  private:
    void release();

    AudioDevice& m_device;

    SDL_AudioSpec m_spec{};
    short* m_samples = nullptr;
    std::uint32_t m_frames = 0;

    SDL_AudioStream* m_stream = nullptr;
};

} // namespace engine
