#include "engine/Music.hpp"

#include "engine/AudioDevice.hpp"

#include <SDL3/SDL.h>

#include <cstdlib>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"
#undef STB_VORBIS_HEADER_ONLY

namespace engine
{

Music::Music(AudioDevice& device) : m_device(device) {}

Music::~Music()
{
    release();
}

bool Music::loadFromFile(const std::string& path)
{
    int channels = 0;
    int sample_rate = 0;
    short* samples = nullptr;

    const int frames = stb_vorbis_decode_filename(path.c_str(), &channels, &sample_rate, &samples);

    if (frames <= 0)
    {
        return false;
    }

    release();

    m_samples = samples;
    m_frames = static_cast<std::uint32_t>(frames);
    m_spec = {SDL_AUDIO_S16, channels, sample_rate};

    if (m_device.isOpen())
    {
        m_stream = SDL_CreateAudioStream(&m_spec, nullptr);

        if (!m_stream || !SDL_BindAudioStream(m_device.getId(), m_stream))
        {
            SDL_Log("Music setup failed: %s", SDL_GetError());
        }
    }

    return true;
}

void Music::setVolume(float volume)
{
    if (m_stream)
    {
        SDL_SetAudioStreamGain(m_stream, volume / 100.f);
    }
}

void Music::play()
{
    if (!m_stream || !m_samples)
    {
        return;
    }

    SDL_ClearAudioStream(m_stream);
    SDL_PutAudioStreamData(m_stream, m_samples,
                           static_cast<int>(m_frames * m_spec.channels * sizeof(short)));
}

void Music::stop()
{
    if (m_stream)
    {
        SDL_ClearAudioStream(m_stream);
    }
}

void Music::update()
{
    if (!m_stream || !m_samples)
    {
        return;
    }

    const int total = static_cast<int>(m_frames * m_spec.channels * sizeof(short));

    // Re-queue the track when the buffer runs low so it loops seamlessly.
    if (SDL_GetAudioStreamQueued(m_stream) < total / 4)
    {
        SDL_PutAudioStreamData(m_stream, m_samples, total);
    }
}

std::uint32_t Music::getSampleCount() const
{
    return m_frames;
}

void Music::release()
{
    if (m_stream)
    {
        SDL_DestroyAudioStream(m_stream);
        m_stream = nullptr;
    }
    if (m_samples)
    {
        // stb_vorbis hands out malloc'd memory.
        std::free(m_samples);
        m_samples = nullptr;
        m_frames = 0;
    }
}

} // namespace engine
