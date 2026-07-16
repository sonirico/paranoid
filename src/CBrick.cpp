#include "CBrick.hpp"

#include "CGameContainer.hpp"
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
        this->lifes = 1;
        this->rects.push_back({64, 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case AQUA:
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w * 2), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case BLUE:
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w * 2), static_cast<int>(h),
                               static_cast<int>(w), static_cast<int>(h)});
        break;
    case GREEN:
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w), static_cast<int>(h), static_cast<int>(w),
                               static_cast<int>(h)});
        break;
    case ORANGE:
        this->lifes = 1;
        this->rects.push_back({64, static_cast<int>(h), static_cast<int>(w), static_cast<int>(h)});
        break;
    case PURPPLE:
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w * 3), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case WHITE:
        this->lifes = 1;
        this->rects.push_back({static_cast<int>(64 + w * 3), static_cast<int>(h),
                               static_cast<int>(w), static_cast<int>(h)});
        break;
    case YELLOW:
        this->lifes = 1;
        this->rects.push_back(
            {static_cast<int>(64 + w), 0, static_cast<int>(w), static_cast<int>(h)});
        break;
    case SILVER:
        this->lifes = 2;
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 4), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    case GOLD:
        this->lifes = 3;
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 5), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    case UNDESTROYABLE:
        for (unsigned int i = 0; i < 6; i++)
            this->rects.push_back({static_cast<int>(64 + w * 6), static_cast<int>(h * i),
                                   static_cast<int>(w), static_cast<int>(h)});
        break;
    default:
        break;
    }

    // A brick is worth 10 points per life it takes to kill; the points
    // are only awarded when it actually dies.
    this->score = 10 * this->lifes;

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
    // A dry metallic cling tells the player at the first touch that
    // this brick cannot be broken.
    if (this->type == UNDESTROYABLE)
    {
        this->state->gc->play_fx(game::game_fx::CLING);

        return;
    }

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

unsigned int CBrick::get_score() const
{
    return this->score;
}

engine::Color CBrick::get_color() const
{
    using namespace game::game_bricks;

    switch (this->type)
    {
    case RED:
        return {216, 40, 0, 255};
    case ORANGE:
        return {252, 152, 56, 255};
    case YELLOW:
        return {248, 216, 0, 255};
    case GREEN:
        return {0, 168, 0, 255};
    case AQUA:
        return {0, 232, 216, 255};
    case BLUE:
        return {0, 112, 236, 255};
    case PURPPLE:
        return {216, 40, 200, 255};
    case WHITE:
        return {252, 252, 252, 255};
    case SILVER:
        return {188, 188, 188, 255};
    case GOLD:
        return {240, 188, 60, 255};
    default:
        return engine::Color::White;
    }
}
