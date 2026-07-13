#include "engine/Text.hpp"

#include "engine/BitmapFont.hpp"
#include "engine/Texture.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

namespace engine
{

void Text::setFont(const BitmapFont& font)
{
    m_font = &font;
}

const BitmapFont* Text::getFont() const
{
    return m_font;
}

void Text::setString(const std::string& string)
{
    m_string = string;
}

const std::string& Text::getString() const
{
    return m_string;
}

void Text::setColor(const Color& color)
{
    m_color = color;
}

const Color& Text::getColor() const
{
    return m_color;
}

FloatRect Text::getLocalBounds() const
{
    const float glyph = m_font ? static_cast<float>(m_font->getGlyphSize()) : 0.f;

    return {0.f, 0.f, m_string.size() * glyph, m_string.empty() ? 0.f : glyph};
}

FloatRect Text::getGlobalBounds() const
{
    const FloatRect local = getLocalBounds();
    const Vec2f position = getPosition();
    const Vec2f scale = getScale();
    const Vec2f origin = getOrigin();

    return {position.x - origin.x * scale.x, position.y - origin.y * scale.y, local.width * scale.x,
            local.height * scale.y};
}

void Text::draw(Window& target) const
{
    if (!m_font || !m_font->getTexture() || !m_font->getTexture()->get())
    {
        return;
    }

    SDL_Texture* texture = m_font->getTexture()->get();

    // The sheet's glyphs are white: tinting the texture colors the text.
    SDL_SetTextureColorMod(texture, m_color.r, m_color.g, m_color.b);
    SDL_SetTextureAlphaMod(texture, m_color.a);

    const FloatRect bounds = getGlobalBounds();
    const float glyph = static_cast<float>(m_font->getGlyphSize());
    const float advance = glyph * getScale().x;

    float x = bounds.left;

    for (const char c : m_string)
    {
        const IntRect rect = m_font->glyphRect(c);

        const SDL_FRect src{static_cast<float>(rect.left), static_cast<float>(rect.top),
                            static_cast<float>(rect.width), static_cast<float>(rect.height)};
        const SDL_FRect dst{x, bounds.top, advance, glyph * getScale().y};

        SDL_RenderTexture(target.getRenderer(), texture, &src, &dst);

        x += advance;
    }
}

} // namespace engine
