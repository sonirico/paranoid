#include "CPaddle.hpp"

#include "CResourceHolder.hpp"

#include <SDL3/SDL.h>

CPaddle::CPaddle(CState* st)
{
    this->state = st;

    this->init();
}

void CPaddle::init()
{
    this->velocity.x = 400;
    this->velocity.y = 0;

    this->scale(this->scalation);

    this->rects.push_back({128, 72, 32, 8});
    this->rects.push_back({128, 80, 32, 8});
    this->rects.push_back({128, 88, 32, 8});
    this->rects.push_back({128, 96, 32, 8});
    this->rects.push_back({128, 88, 32, 8});
    this->rects.push_back({128, 80, 32, 8});

    this->animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));

    for (unsigned int i = 0; i < this->rects.size(); ++i)
    {
        this->animation.addFrame(this->rects[i]);
    }

    this->laser_animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));

    for (int i = 0; i < 4; ++i)
    {
        this->laser_animation.addFrame({128, 192 + 8 * i, 32, 8});
    }

    this->current_animation = &this->animation;

    this->animated_sprite = engine::AnimatedSprite(0.2f, true, false);
    this->animated_sprite.play(this->animation);

    this->bounds.x = this->animated_sprite.getLocalBounds().width;
    this->bounds.y = this->animated_sprite.getLocalBounds().height;

    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;

    this->setPosition((game::WIDTH - this->bounds.x) / 2, game::HEIGHT - 100);
}

void CPaddle::update(const float dt)
{
    this->dir = 0;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_RIGHT])
    {
        this->move(this->velocity * dt);
        this->dir = 1;
    }
    if (keys[SDL_SCANCODE_LEFT])
    {
        this->move(this->velocity * -dt);
        this->dir = -1;
    }

    this->animated_sprite.update(dt);
    this->animated_sprite.play();

    this->check_bounds();
}

void CPaddle::reset()
{
    this->setPosition((game::WIDTH - this->bounds.x) / 2, game::HEIGHT - 100);
}

void CPaddle::reset_modes()
{
    this->apply_width_factor(1.f);
    this->set_sticky(false);
    this->set_laser(false);
    this->set_spin(false);
}

void CPaddle::expand()
{
    this->apply_width_factor(1.5f);
}

void CPaddle::shrink()
{
    this->apply_width_factor(0.65f);
}

void CPaddle::set_sticky(bool b)
{
    this->sticky = b;
}

bool CPaddle::is_sticky() const
{
    return this->sticky;
}

void CPaddle::set_laser(bool b)
{
    this->laser = b;

    // The paddle transforms: blue cannons while armed, normal otherwise.
    this->current_animation = b ? &this->laser_animation : &this->animation;
    this->animated_sprite.play(*this->current_animation);
}

bool CPaddle::has_laser() const
{
    return this->laser;
}

void CPaddle::set_spin(bool b)
{
    this->spin = b;
}

bool CPaddle::has_spin() const
{
    return this->spin;
}

void CPaddle::apply_width_factor(float factor)
{
    this->width_factor = factor;

    this->setScale({this->scalation.x * factor, this->scalation.y});

    this->bounds.x = this->animated_sprite.getLocalBounds().width * this->scalation.x * factor;

    this->check_bounds();
}

void CPaddle::check_bounds()
{
    engine::Vec2f pos = this->getPosition();

    if (pos.x <= 0)
    {
        pos.x = 0;
    }
    else if (pos.x + this->bounds.x >= game::WIDTH)
    {
        pos.x = game::WIDTH - this->bounds.x;
    }

    this->setPosition(pos);
}
