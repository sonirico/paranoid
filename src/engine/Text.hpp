#pragma once

#include "engine/Color.hpp"
#include "engine/Drawable.hpp"
#include "engine/Rect.hpp"
#include "engine/Transformable.hpp"

#include <string>

namespace engine
{

class BitmapFont;

// A single line of bitmap-font text. The color tints the font's white
// glyphs; size comes from the transform's scale.
class Text : public Transformable, public Drawable
{
  public:
    Text() = default;

    void setFont(const BitmapFont& font);
    const BitmapFont* getFont() const;

    void setString(const std::string& string);
    const std::string& getString() const;

    void setColor(const Color& color);
    const Color& getColor() const;

    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;

    void draw(Window& target) const override;

  private:
    const BitmapFont* m_font = nullptr;
    std::string m_string;
    Color m_color = Color::White;
};

} // namespace engine
