#include "CBonus.hpp"

#include "CPaddle.hpp"
#include "CResourceHolder.hpp"

CBonus::CBonus(CState* state, game::game_bonus::bonus t) : type(t)
{
    this->state = state;

    this->init();
}

void CBonus::init()
{
    this->settings();

    this->bounds.x = this->animated_sprite.getGlobalBounds().width * this->scalation.x;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height * this->scalation.y;

    this->velocity = engine::Vec2f{0, 100};

    this->scale(this->scalation);
}

void CBonus::update(const float dt)
{
    this->move(this->velocity * dt);
    this->animated_sprite.update(dt);
}

void CBonus::reset() {}

void CBonus::settings()
{
    using namespace game::game_bonus;

    const int w = 16;
    const int h = 8;

    // Each bonus letter is a 7-frame vertical strip on the sheet,
    // except X (extra life) which uses taller 16x16 frames.
    if (this->type == X)
    {
        for (int i = 0; i < 6; ++i)
        {
            this->rects.push_back({176 + w * 11, 16 * i, 16, 16});
        }
    }
    else
    {
        const int column = static_cast<int>(this->type);

        for (int i = 0; i < 7; ++i)
        {
            this->rects.push_back({176 + w * column, h * i, w, h});
        }
    }

    this->animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));

    for (unsigned int i = 0; i < this->rects.size(); ++i)
    {
        this->animation.addFrame(this->rects[i]);
    }
    this->current_animation = &this->animation;

    this->animated_sprite = engine::AnimatedSprite(0.1f, false, true);
    this->animated_sprite.play(this->animation);
}

bool CBonus::collision_bonus_paddle(CPaddle* p)
{
    float x1, y1, w1, h1;
    float x2, y2, w2, h2;

    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;

    x2 = p->getPosition().x;
    y2 = p->getPosition().y;
    w2 = p->get_bounds().width * 2;
    h2 = p->get_bounds().height * 2;

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

bool CBonus::is_removable() const
{
    return this->removable;
}
