#include "CBrick.hpp"

#include "CPlayState.hpp"
#include "CResourceHolder.hpp"

using namespace game::game_bricks;

CBrick::CBrick(CState* st, bricks t) : type(t)
{
    this->state = st;

    this->init();
}

void CBrick::init()
{
    this->settings();

    this->scale(this->scalation);

    this->bounds.x = this->animated_sprite.getGlobalBounds().width;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height;

    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;
}

void CBrick::update(const float dt)
{
    this->animated_sprite.update(dt);
}

void CBrick::reset() {}

void CBrick::settings()
{
    unsigned int w = BRICK_BOUND.x;
    unsigned int h = BRICK_BOUND.y;

    switch (this->type)
    {
    case RED:
        this->score = 90;
        this->lifes = 1;
        this->rects.push_back({64, 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case AQUA:
        this->score = 70;
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w * 2), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case BLUE:
        this->score = 100;
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w * 2), static_cast<int>(h),
                               static_cast<int>(w), static_cast<int>(h)});
        break;
    case GREEN:
        this->score = 80;
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w), static_cast<int>(h), static_cast<int>(w),
                               static_cast<int>(h)});
        break;
    case ORANGE:
        this->score = 60;
        this->lifes = 1;
        this->rects.push_back({64, static_cast<int>(h), static_cast<int>(w), static_cast<int>(h)});
        break;
    case PURPPLE:
        this->score = 110;
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w * 3), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case WHITE:
        this->score = 50;
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w * 3), static_cast<int>(h),
                               static_cast<int>(w), static_cast<int>(h)});
        break;
    case YELLOW:
        this->score = 120;
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case SILVER:
        this->score = 50 * CPlayState::get_current_stage();
        this->lifes = 2;
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 4), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    case GOLD:
        this->score = 100 * CPlayState::get_current_stage();
        this->lifes = 3;
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 5), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    case UNDESTROYABLE:
        this->score = 0;
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 6), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    default:
        break;
    }

    this->animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));

    for (unsigned int i = 0; i < this->rects.size(); i++)
    {
        this->animation.addFrame(this->rects[i]);
    }
    this->current_animation = &this->animation;

    this->animated_sprite = engine::AnimatedSprite(0.05f, true, false);
    this->animated_sprite.play(this->animation);
}

void CBrick::quit_life()
{
    if (this->lifes > 0)
    {
        this->lifes--;
        this->removable = this->lifes <= 0;
    }
}

bool CBrick::is_removable() const
{
    return this->removable;
}
