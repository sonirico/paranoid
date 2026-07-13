#include "engine/Texture.hpp"

#include <SDL3/SDL.h>

#include <SDL3_image/SDL_image.h>
#include <utility>

namespace engine
{

Texture::~Texture()
{
    if (m_handle)
    {
        SDL_DestroyTexture(m_handle);
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_handle(std::exchange(other.m_handle, nullptr)), m_size(std::exchange(other.m_size, {}))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        if (m_handle)
        {
            SDL_DestroyTexture(m_handle);
        }
        m_handle = std::exchange(other.m_handle, nullptr);
        m_size = std::exchange(other.m_size, {});
    }
    return *this;
}

bool Texture::loadFromFile(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Texture* loaded = IMG_LoadTexture(renderer, path.c_str());

    if (!loaded)
    {
        return false;
    }

    SDL_SetTextureScaleMode(loaded, SDL_SCALEMODE_NEAREST);

    if (m_handle)
    {
        SDL_DestroyTexture(m_handle);
    }

    m_handle = loaded;
    m_size = {static_cast<int>(loaded->w), static_cast<int>(loaded->h)};

    return true;
}

Vec2i Texture::getSize() const
{
    return m_size;
}

SDL_Texture* Texture::get() const
{
    return m_handle;
}

} // namespace engine
