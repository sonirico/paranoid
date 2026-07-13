#pragma once

#include "engine/Vec2.hpp"

namespace engine
{

// Position/scale/origin holder for anything placed in the world.
// The game never rotates sprites, so no rotation support.
class Transformable
{
  public:
    virtual ~Transformable() = default;

    void setPosition(const Vec2f& position);
    void setPosition(float x, float y);
    const Vec2f& getPosition() const;
    void move(const Vec2f& offset);
    void move(float x, float y);

    void setScale(const Vec2f& factors);
    const Vec2f& getScale() const;

    // Multiplies the current scale, mirroring sf::Transformable::scale.
    void scale(const Vec2f& factors);
    void scale(float x, float y);

    void setOrigin(const Vec2f& origin);
    const Vec2f& getOrigin() const;

  private:
    Vec2f m_position;
    Vec2f m_scale{1.f, 1.f};
    Vec2f m_origin;
};

} // namespace engine
