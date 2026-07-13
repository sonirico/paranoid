#pragma once

#include "engine/Rect.hpp"

namespace engine
{

class Texture;

// A fixed-cell bitmap font: a texture holding the printable ASCII range
// as a grid of square glyphs, drawn white so text can tint it any color.
class BitmapFont
{
  public:
    BitmapFont() = default;

    void setTexture(const Texture& texture);
    const Texture* getTexture() const;

    // The sub-image of the sheet holding the given character's glyph;
    // characters outside the printable range map to '?'.
    IntRect glyphRect(char c) const;

    int getGlyphSize() const;

  private:
    const Texture* m_texture = nullptr;

    static constexpr int GLYPH = 8;
    static constexpr int COLUMNS = 16;
    static constexpr char FIRST = ' ';
    static constexpr char LAST = '~';
};

} // namespace engine
