#include "CBall.hpp"

#include "CGameContainer.hpp"
#include "CPaddle.hpp"
#include "CResourceHolder.hpp"

#include <cmath>

CBall::CBall(CState* st, CPaddle* pd)
{
    this->state = st;
    this->paddle = pd;

    this->init();
}

void CBall::init()
{
    this->removable = false;
    this->in_paddle = false;

    this->velocity.x = this->vel * 0.5f;
    this->velocity.y = this->vel * -0.5f;

    this->scale(this->scalation);

    this->animation = engine::Animation();
    this->animation.setSpriteSheet(this->state->rh->get(game::game_textures::MAIN));
    this->animation.addFrame({64, 16, 5, 4});
    this->current_animation = &this->animation;

    // A single static frame: negative frame time keeps it from advancing.
    this->animated_sprite = engine::AnimatedSprite(-1.f, true, false);
    this->animated_sprite.play(this->animation);

    this->bounds.x = this->animated_sprite.getGlobalBounds().width;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height;

    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;
}

void CBall::update(const float dt)
{
    if (this->in_paddle)
    {
        this->set_in_paddle();
    }
    else
    {
        this->move(this->velocity * dt);
        this->collision_ball_paddle();
    }

    this->check_bounds();
}

void CBall::reset() {}

void CBall::set_in_paddle()
{
    engine::Vec2f pos;

    engine::Vec2f p_pos = this->paddle->getPosition();

    pos.x = p_pos.x + (this->paddle->get_size().x - this->bounds.x) / 2;
    pos.y = p_pos.y - this->bounds.y;

    this->setPosition(pos);

    this->set_in_paddle(true);
}

void CBall::set_in_paddle(bool b)
{
    this->in_paddle = b;
}

void CBall::check_bounds()
{
    engine::Vec2f pos = this->getPosition();

    unsigned int w = static_cast<int>(this->bounds.x);
    unsigned int h = static_cast<int>(this->bounds.y);

    if (pos.x < 0)
    {
        pos.x = 0;
        this->velocity.x *= -1;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    else if (pos.x + w > game::WIDTH)
    {
        pos.x = game::WIDTH - w;
        this->velocity.x *= -1;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    if (pos.y < 0)
    {
        pos.y = 0;
        this->velocity.y *= -1;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    else if (pos.y + h > game::HEIGHT)
    {
        // Fell below the paddle: the ball is lost.
        this->removable = true;
    }

    this->setPosition(pos);
}

bool CBall::is_in_paddle() const
{
    return this->in_paddle;
}

void CBall::collision_ball_paddle()
{
    if (this->paddle == nullptr)
    {
        return;
    }

    float x1, w1, h1, y1, x2, y2, w2, h2;

    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;

    x2 = this->paddle->getPosition().x;
    y2 = this->paddle->getPosition().y;
    w2 = this->paddle->get_size().x;
    h2 = this->paddle->get_size().y;

    if ((x1 + w1) < x2)
        return;
    if ((y1 + h1) < y2)
        return;
    if ((x2 + w2) < x1)
        return;
    if ((y2 + h2) < y1)
        return;

    this->state->gc->play_fx(game::game_fx::REBOTE2);

    // The ball hit the paddle: reposition it and recompute the velocity
    // depending on whether it struck the sides or the top.
    engine::Vec2f new_pos;

    // Hitting the sides lets the ball keep falling, losing the life.
    if (y1 > y2)
    {
        new_pos.y = y1;

        float overlap_right = std::abs(x2 + w2 - x1);
        float overlap_left = std::abs(x1 + w1 - x2);

        // Coming from the right.
        if (overlap_left > overlap_right)
        {
            new_pos.x = x2 + w2;
        }
        // From the left.
        else
        {
            new_pos.x = x2 - w1;
        }

        this->setPosition(new_pos);
        this->velocity.x *= -1;
    }
    else
    {
        new_pos.y = y2 - w1 - 1;
        new_pos.x = x1;
        this->setPosition(new_pos);

        // A sticky paddle catches the ball instead of bouncing it.
        if (this->paddle->is_sticky())
        {
            this->set_in_paddle();
            return;
        }

        float cp = (x2 + (w2 / 2)); // Paddle centre.
        float cb = (x1 + (w1 / 2)); // Ball centre.

        // Distance between centres over half the paddle width gives a
        // bounce angle factor between 0 and 1...
        float radians = std::abs((cp - cb) / (w2 / 2));

        // ...clamped so the ball never leaves near-parallel to the floor.
        if (radians > 0.7f)
        {
            radians = 0.7f;
        }

        // New horizontal direction.
        int dir = cp > cb ? -1 : 1;

        this->velocity.x = this->vel * radians * dir;
        // The vertical component always leaves upwards.
        this->velocity.y = -(this->vel * (1 - radians));
    }
}

bool CBall::check_point_rect(engine::Vec2f point, CBrick* b) const
{
    float x, y, w, h;

    x = b->getPosition().x;
    y = b->getPosition().y;
    w = b->get_bounds().width * 2;
    h = b->get_bounds().height * 2;

    float xx = point.x;
    float yy = point.y;

    if (xx < x || xx > x + w)
        return false;
    if (yy < y || yy > y + h)
        return false;

    return true;
}

bool CBall::collision_ball_brick(CBrick* b, const float dt)
{
    bool collided = false;

    // Step the ball backwards until its cardinal points no longer
    // touch the brick...
    while (this->check_collision(b))
    {
        this->move(this->velocity * -dt);
        collided = true;
    }

    if (collided)
    {
        // ...but corner hits need an extra nudge to fully separate.
        float m_w = this->bounds.x / 2;
        float m_h = this->bounds.y / 2;

        if (this->dirx)
        {
            this->velocity.x *= -1;

            if (this->getPosition().x < b->getPosition().x)
                this->move(-m_w, 0);
            else
                this->move(m_w, 0);
        }
        if (this->diry)
        {
            this->velocity.y *= -1;

            if (this->getPosition().y < b->getPosition().y)
                this->move(0, -m_h);
            else
                this->move(0, m_h);
        }

        this->dirx = false;
        this->diry = false;
    }

    return collided;
}

bool CBall::check_collision(CBrick* b)
{
    engine::Vec2f left{this->getPosition().x, this->getPosition().y + this->bounds.y / 2};
    engine::Vec2f right{this->getPosition().x + this->bounds.x,
                        this->getPosition().y + this->bounds.y / 2};
    engine::Vec2f top{this->getPosition().x + this->bounds.x / 2, this->getPosition().y};
    engine::Vec2f bottom{this->getPosition().x + this->bounds.x / 2,
                         this->getPosition().y + this->bounds.y};

    bool col = false;

    // X axis.
    if (this->check_point_rect(left, b))
    {
        dirx = true;
        col = true;
    }
    else if (this->check_point_rect(right, b))
    {
        dirx = true;
        col = true;
    }
    // Y axis.
    if (this->check_point_rect(top, b))
    {
        diry = true;
        col = true;
    }
    else if (this->check_point_rect(bottom, b))
    {
        diry = true;
        col = true;
    }

    return col;
}

bool CBall::is_removable() const
{
    return this->removable;
}

void CBall::set_velocity(const engine::Vec2f& v)
{
    this->velocity = v;
}

void CBall::scale_velocity(float factor)
{
    this->velocity *= factor;
    this->vel *= factor;
}
