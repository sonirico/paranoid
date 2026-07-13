#pragma once

#include "engine/Drawable.hpp"
#include "engine/Rect.hpp"
#include "engine/Transformable.hpp"

namespace engine
{

class Texture;

// A textured quad. The texture rect selects the sub-image of the
// sheet to display; local bounds derive from it.
class Sprite : public Transformable, public Drawable
{
  public:
    Sprite() = default;

    void setTexture(const Texture& texture);
    const Texture* getTexture() const;

    void setTextureRect(const IntRect& rect);
    const IntRect& getTextureRect() const;

    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;

    void draw(Window& target) const override;

  private:
    const Texture* m_texture = nullptr;
    IntRect m_textureRect;
};

} // namespace engine
