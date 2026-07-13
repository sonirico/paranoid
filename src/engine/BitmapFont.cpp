#include "engine/BitmapFont.hpp"

#include "engine/Texture.hpp"

namespace engine
{

void BitmapFont::setTexture(const Texture& texture)
{
    m_texture = &texture;
}

const Texture* BitmapFont::getTexture() const
{
    return m_texture;
}

IntRect BitmapFont::glyphRect(char c) const
{
    if (c < FIRST || c > LAST)
    {
        c = '?';
    }

    const int index = c - FIRST;

    return {(index % COLUMNS) * GLYPH, (index / COLUMNS) * GLYPH, GLYPH, GLYPH};
}

int BitmapFont::getGlyphSize() const
{
    return GLYPH;
}

} // namespace engine
