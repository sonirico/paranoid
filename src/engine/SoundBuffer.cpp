#include "engine/SoundBuffer.hpp"

#include <SDL3/SDL.h>

#include <utility>

namespace engine
{

SoundBuffer::~SoundBuffer()
{
    release();
}

SoundBuffer::SoundBuffer(SoundBuffer&& other) noexcept
    : m_spec(other.m_spec), m_data(std::exchange(other.m_data, nullptr)),
      m_size(std::exchange(other.m_size, 0))
{
}

SoundBuffer& SoundBuffer::operator=(SoundBuffer&& other) noexcept
{
    if (this != &other)
    {
        release();
        m_spec = other.m_spec;
        m_data = std::exchange(other.m_data, nullptr);
        m_size = std::exchange(other.m_size, 0);
    }
    return *this;
}

bool SoundBuffer::loadFromFile(const std::string& path)
{
    SDL_AudioSpec spec{};
    std::uint8_t* data = nullptr;
    std::uint32_t size = 0;

    if (!SDL_LoadWAV(path.c_str(), &spec, &data, &size))
    {
        return false;
    }

    release();
    m_spec = spec;
    m_data = data;
    m_size = size;

    return true;
}

const SDL_AudioSpec& SoundBuffer::getSpec() const
{
    return m_spec;
}

const std::uint8_t* SoundBuffer::getData() const
{
    return m_data;
}

std::uint32_t SoundBuffer::getSize() const
{
    return m_size;
}

std::uint32_t SoundBuffer::getSampleCount() const
{
    const std::uint32_t frameSize = SDL_AUDIO_FRAMESIZE(m_spec);

    return frameSize == 0 ? 0 : m_size / frameSize;
}

void SoundBuffer::release()
{
    if (m_data)
    {
        SDL_free(m_data);
        m_data = nullptr;
        m_size = 0;
    }
}

} // namespace engine
