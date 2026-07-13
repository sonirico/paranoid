#pragma once

#include "CState.hpp"
#include "engine/AnimatedSprite.hpp"
#include "engine/Drawable.hpp"
#include "engine/Rect.hpp"
#include "engine/Transformable.hpp"
#include "engine/Vec2.hpp"

// Base class for everything that moves on screen. The entity owns the
// transform; its sprite is drawn with that transform applied, so
// get_bounds() intentionally returns the UNSCALED frame size — the
// collision code compensates with the scale factor at each call site.
class CEntity : public engine::Transformable, public engine::Drawable
{
  public:
    virtual void init() = 0;
    virtual void update(const float dt) = 0;
    virtual void reset() = 0;

    // Renders the animated sprite at the entity's position and scale.
    void draw(engine::Window& target) const override;

    engine::FloatRect get_bounds() const;

    void stop_animation();
    void play_animation();

    void set_id(unsigned int id);
    int get_id() const;

    engine::Vec2f get_velocity() const;

  protected:
    // Non-owning; points at an animation owned by the concrete entity.
    const engine::Animation* current_animation = nullptr;
    engine::AnimatedSprite animated_sprite;

    unsigned int id = 0;

    engine::Vec2f velocity;

    engine::Vec2f scalation{2, 2};

    engine::Vec2f bounds;

    CState* state = nullptr;
};
