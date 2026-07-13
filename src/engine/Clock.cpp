#include "engine/Clock.hpp"

#include <SDL3/SDL.h>

namespace engine
{

Clock::Clock() : m_last(SDL_GetTicksNS()) {}

float Clock::restart()
{
    const std::uint64_t now = SDL_GetTicksNS();
    const std::uint64_t elapsed = now - m_last;

    m_last = now;

    return static_cast<float>(elapsed) / 1e9f;
}

} // namespace engine
