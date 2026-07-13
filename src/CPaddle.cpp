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
