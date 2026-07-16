#pragma once

struct SDL_AudioStream;

namespace engine
{

class AudioDevice;
class SoundBuffer;

// A playable voice: one audio stream bound to the device. Playing
// again restarts the effect from the beginning.
class Sound
{
  public:
    explicit Sound(AudioDevice& device);
    ~Sound();

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound&) = delete;

    void setBuffer(const SoundBuffer& buffer);

    // Volume in the 0..100 range, matching what the game code expects.
    void setVolume(float volume);

    // Playback speed ratio: 1 is natural, higher is faster and sharper.
    void setPitch(float ratio);

    void play();

  private:
    AudioDevice& m_device;
    const SoundBuffer* m_buffer = nullptr;
    SDL_AudioStream* m_stream = nullptr;
};

} // namespace engine
