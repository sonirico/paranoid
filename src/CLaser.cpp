#include "CLaser.hpp"

#include "CResourceHolder.hpp"

CLaser::CLaser(CState* st)
{
    this->state = st;

    this->init();
}

void CLaser::init()
{
    this->velocity = engine::Vec2f{0, -600};

    this->scale(this->scalation);

    // Reuses the ball frame as the projectile sprite.
    this->animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));
    this->animation.addFrame({64, 16, 5, 4});
    this->current_animation = &this->animation;

    this->animated_sprite = engine::AnimatedSprite(-1.f, true, false);
    this->animated_sprite.play(this->animation);

    this->bounds.x = this->animated_sprite.getLocalBounds().width * this->scalation.x;
    this->bounds.y = this->animated_sprite.getLocalBounds().height * this->scalation.y;
}

void CLaser::update(const float dt)
{
    this->move(this->velocity * dt);

    if (this->getPosition().y + this->bounds.y < 0)
    {
        this->removable = true;
    }
}

void CLaser::reset() {}

bool CLaser::collision_laser_brick(CBrick* b)
{
    float x1, y1, w1, h1;
    float x2, y2, w2, h2;

    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;

    x2 = b->getPosition().x;
    y2 = b->getPosition().y;
    w2 = b->get_bounds().width * 2;
    h2 = b->get_bounds().height * 2;

    if ((x1 + w1) < x2)
        return false;
    if ((y1 + h1) < y2)
        return false;
    if ((x2 + w2) < x1)
        return false;
    if ((y2 + h2) < y1)
        return false;

    this->removable = true;

    return true;
}

bool CLaser::is_removable() const
{
    return this->removable;
}
