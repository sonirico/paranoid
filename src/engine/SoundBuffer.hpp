#pragma once

#include <SDL3/SDL_audio.h>

#include <cstdint>
#include <string>

namespace engine
{

// Decoded WAV data plus its format, shared by any number of Sounds.
class SoundBuffer
{
  public:
    SoundBuffer() = default;
    ~SoundBuffer();

    SoundBuffer(const SoundBuffer&) = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;
    SoundBuffer(SoundBuffer&& other) noexcept;
    SoundBuffer& operator=(SoundBuffer&& other) noexcept;

    bool loadFromFile(const std::string& path);

    const SDL_AudioSpec& getSpec() const;
    const std::uint8_t* getData() const;
    std::uint32_t getSize() const;
    std::uint32_t getSampleCount() const;

  private:
    void release();

    SDL_AudioSpec m_spec{};
    std::uint8_t* m_data = nullptr;
    std::uint32_t m_size = 0;
};

} // namespace engine
