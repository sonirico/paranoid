#include "engine/Sprite.hpp"

#include "engine/Texture.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

namespace engine
{

void Sprite::setTexture(const Texture& texture)
{
    m_texture = &texture;

    // Mirror SFML: adopting a texture without an explicit rect shows all of it.
    if (m_textureRect == IntRect{})
    {
        m_textureRect = {0, 0, texture.getSize().x, texture.getSize().y};
    }
}

const Texture* Sprite::getTexture() const
{
    return m_texture;
}

void Sprite::setTextureRect(const IntRect& rect)
{
    m_textureRect = rect;
}

const IntRect& Sprite::getTextureRect() const
{
    return m_textureRect;
}

FloatRect Sprite::getLocalBounds() const
{
    return {0.f, 0.f, static_cast<float>(m_textureRect.width),
            static_cast<float>(m_textureRect.height)};
}

FloatRect Sprite::getGlobalBounds() const
{
    const FloatRect local = getLocalBounds();
    const Vec2f position = getPosition();
    const Vec2f scale = getScale();
    const Vec2f origin = getOrigin();

    return {position.x - origin.x * scale.x, position.y - origin.y * scale.y, local.width * scale.x,
            local.height * scale.y};
}

void Sprite::draw(Window& target) const
{
    if (!m_texture || !m_texture->get())
    {
        return;
    }

    const FloatRect bounds = getGlobalBounds();

    const SDL_FRect src{
        static_cast<float>(m_textureRect.left), static_cast<float>(m_textureRect.top),
        static_cast<float>(m_textureRect.width), static_cast<float>(m_textureRect.height)};
    // Snapped to whole pixels: a fractional destination makes GPU
    // renderers sample half a texel outside the source rect, bleeding
    // the atlas neighbor into the sprite's edge.
    const SDL_FRect dst{SDL_floorf(bounds.left + 0.5f), SDL_floorf(bounds.top + 0.5f), bounds.width,
                        bounds.height};

    SDL_RenderTexture(target.getRenderer(), m_texture->get(), &src, &dst);
}

} // namespace engine
