#include "CBall.hpp"

#include "CGameContainer.hpp"
#include "CPaddle.hpp"
#include "CResourceHolder.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

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

    // Serve at 45 degrees with the full target speed.
    this->velocity = engine::Vec2f{1.f, -1.f}.normalized() * this->vel;

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
        this->follow_paddle();
        this->trail.clear();
    }
    else
    {
        this->tick_start_pos = this->getPosition();

        this->apply_magnus(dt);
        this->move(this->velocity * dt);
        this->collision_ball_paddle();

        this->trail.push_back(this->getPosition());

        if (this->trail.size() > TRAIL_LENGTH)
        {
            this->trail.erase(this->trail.begin());
        }
    }

    this->check_bounds();
}

void CBall::draw(engine::Window& target) const
{
    // A curving ball leaves a comet tail: orange for clockwise spin,
    // cyan for the other way, brighter the harder it spins.
    if (std::abs(this->spin) > TRAIL_MIN_SPIN)
    {
        const float intensity = std::min(1.f, std::abs(this->spin) / (MAX_SPIN / 2));
        const engine::Color base =
            this->spin > 0 ? engine::Color{255, 160, 40, 255} : engine::Color{80, 220, 255, 255};

        for (unsigned int i = 0; i < this->trail.size(); ++i)
        {
            // Older segments are smaller and fainter.
            const float age = (i + 1.f) / (this->trail.size() + 1.f);

            engine::Color color = base;
            color.a = static_cast<std::uint8_t>(age * intensity * 150);

            const engine::Vec2f size = this->bounds * (0.3f + 0.6f * age);
            const engine::Vec2f offset = (this->bounds - size) * 0.5f;

            target.drawRect(this->trail[i] + offset, size, color);
        }
    }

    CEntity::draw(target);
}

void CBall::apply_magnus(const float dt)
{
    if (this->spin == 0.f)
    {
        return;
    }

    // The curve may steepen a trajectory but never rotate it through the
    // horizontal, so remember which way the ball was flying.
    const float sign_y = this->velocity.y < 0 ? -1.f : 1.f;

    const engine::Vec2f accel =
        this->velocity.normalized().perpendicular() * (MAGNUS_COEF * this->spin);

    this->velocity += accel * dt;

    // Magnus curves the path but never changes the speed: renormalize so
    // the lateral push cannot fight the ramp.
    this->velocity = this->velocity.normalized() * this->vel;

    const float min_vy = MIN_VERTICAL_RATIO * this->vel;

    if (std::abs(this->velocity.y) < min_vy)
    {
        const float sign_x = this->velocity.x < 0 ? -1.f : 1.f;
        const float vx = std::sqrt(this->vel * this->vel - min_vy * min_vy);

        this->velocity = {vx * sign_x, min_vy * sign_y};
    }

    this->spin -= this->spin * SPIN_DECAY * dt;
}

void CBall::reset() {}

void CBall::set_in_paddle()
{
    this->park_in_paddle((this->paddle->get_size().x - this->bounds.x) / 2);
}

void CBall::park_in_paddle(float offset)
{
    this->paddle_offset = offset;
    this->spin = 0.f;

    this->set_in_paddle(true);
    this->follow_paddle();
}

void CBall::follow_paddle()
{
    const engine::Vec2f p_pos = this->paddle->getPosition();

    // The paddle may have shrunk since parking; keep the ball on it.
    const float max_offset = this->paddle->get_size().x - this->bounds.x;
    this->paddle_offset = std::clamp(this->paddle_offset, 0.f, max_offset);

    this->setPosition(p_pos.x + this->paddle_offset, p_pos.y - this->bounds.y);
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
        this->spin *= SPIN_BOUNCE_DAMP;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    else if (pos.x + w > game::WIDTH)
    {
        pos.x = game::WIDTH - w;
        this->velocity.x *= -1;
        this->spin *= SPIN_BOUNCE_DAMP;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    if (pos.y < 0)
    {
        pos.y = 0;
        this->velocity.y *= -1;
        this->spin *= SPIN_BOUNCE_DAMP;
        this->state->gc->play_fx(game::game_fx::REBOTE1);
    }
    else if (pos.y + h > game::HEIGHT)
    {
        if (this->net)
        {
            // The net barrier (bonus N) bounces the ball back up.
            pos.y = game::HEIGHT - h;
            this->velocity.y *= -1;
            this->spin *= SPIN_BOUNCE_DAMP;
            this->state->gc->play_fx(game::game_fx::REBOTE1);
        }
        else
        {
            // Fell below the paddle: the ball is lost.
            this->removable = true;
        }
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

    const engine::Vec2f p_pos = this->paddle->getPosition();
    const engine::Vec2f p_size = this->paddle->get_size();

    // A ball whose bottom had already sunk deeper than the save grace at
    // the start of the tick is beyond saving: no side bounces, but a
    // paddle sliding under a low ball at the last instant still saves it.
    if (this->tick_start_pos.y + this->bounds.y > p_pos.y + p_size.y * SAVE_GRACE)
    {
        return;
    }

    // Sweeping this tick's flight path instead of testing the final
    // position keeps a fast ball from slipping through the paddle.
    const engine::Vec2f half{this->bounds.x / 2, this->bounds.y / 2};
    const engine::Vec2f start = this->tick_start_pos + half;
    const engine::Vec2f delta = this->getPosition() + half - start;

    SweptHit hit;

    if (!this->sweep_ball_rect(start, delta, p_pos, p_size, hit))
    {
        return;
    }

    this->state->gc->play_fx(game::game_fx::REBOTE2);

    // The ball hit the paddle from above: park it on top and recompute
    // the bounce direction from the impact point.
    engine::Vec2f new_pos;
    new_pos.y = p_pos.y - this->bounds.y - 1;
    new_pos.x = start.x + delta.x * hit.t - half.x;
    this->setPosition(new_pos);

    float cp = (p_pos.x + (p_size.x / 2));         // Paddle centre.
    float cb = (new_pos.x + (this->bounds.x / 2)); // Ball centre.

    // Distance between centres over half the paddle width gives a
    // bounce angle factor between 0 and 1...
    float radians = std::abs((cp - cb) / (p_size.x / 2));

    // ...clamped so the ball never leaves near-parallel to the floor.
    if (radians > 0.7f)
    {
        radians = 0.7f;
    }

    // New horizontal direction.
    int dir = cp > cb ? -1 : 1;

    // Each paddle bounce speeds the game up a little, capped.
    this->vel = std::min(this->vel * VEL_RAMP, MAX_VEL);

    // The vertical component always leaves upwards; normalizing keeps
    // the speed constant regardless of the bounce angle.
    engine::Vec2f direction{radians * dir, -(1 - radians)};
    this->velocity = direction.normalized() * this->vel;

    // A moving spin-mode paddle (bonus T) cuts the ball (Magnus effect).
    if (this->paddle->has_spin() && this->paddle->dir != 0)
    {
        const float imparted =
            SPIN_FROM_PADDLE * this->paddle->dir * std::abs(this->paddle->get_velocity().x);

        this->spin = std::clamp(this->spin + imparted, -MAX_SPIN, MAX_SPIN);
    }

    // A sticky paddle catches the ball right where it landed; the launch
    // velocity computed above is kept for when SPACE releases it.
    if (this->paddle->is_sticky())
    {
        this->park_in_paddle(new_pos.x - p_pos.x);
    }
}

std::vector<CBrick*> CBall::collision_ball_bricks(const std::list<std::unique_ptr<CBrick>>& bricks)
{
    std::vector<CBrick*> hit_bricks;

    if (this->in_paddle)
    {
        return hit_bricks;
    }

    // Working with the ball's centre turns the flight path into a segment
    // and each brick into a rectangle expanded by half the ball.
    const engine::Vec2f half{this->bounds.x / 2, this->bounds.y / 2};

    engine::Vec2f start = this->tick_start_pos + half;
    engine::Vec2f end = this->getPosition() + half;

    for (unsigned int i = 0; i < MAX_BRICK_BOUNCES; ++i)
    {
        const engine::Vec2f delta = end - start;

        CBrick* nearest = nullptr;
        SweptHit best;

        for (const auto& brick : bricks)
        {
            SweptHit hit;

            if (!this->sweep_ball_rect(start, delta, brick->getPosition(), brick->get_size(), hit))
            {
                continue;
            }

            // A megaball ploughs straight through destructible bricks:
            // they take the hit, but only indestructible ones deflect.
            if (this->pierce && brick->type != game::game_bricks::UNDESTROYABLE)
            {
                if (std::find(hit_bricks.begin(), hit_bricks.end(), brick.get()) ==
                    hit_bricks.end())
                {
                    hit_bricks.push_back(brick.get());
                }

                continue;
            }

            if (nearest == nullptr || hit.t < best.t)
            {
                best = hit;
                nearest = brick.get();
            }
        }

        if (nearest == nullptr)
        {
            break;
        }

        hit_bricks.push_back(nearest);

        // Advance to the impact point, then mirror the rest of the path
        // and the velocity across the struck face only.
        start += delta * best.t + best.eject;

        engine::Vec2f rest = delta * (1.f - best.t);

        if (best.flip_x)
        {
            rest.x = -rest.x;
            this->velocity.x = -this->velocity.x;
            start.x += this->velocity.x > 0 ? SWEEP_SKIN : -SWEEP_SKIN;
        }
        if (best.flip_y)
        {
            rest.y = -rest.y;
            this->velocity.y = -this->velocity.y;
            start.y += this->velocity.y > 0 ? SWEEP_SKIN : -SWEEP_SKIN;
        }

        end = start + rest;

        this->spin *= SPIN_BOUNCE_DAMP;
    }

    if (!hit_bricks.empty())
    {
        this->setPosition(end - half);
    }

    return hit_bricks;
}

bool CBall::sweep_ball_rect(const engine::Vec2f& start, const engine::Vec2f& delta,
                            const engine::Vec2f& rect_pos, const engine::Vec2f& rect_size,
                            SweptHit& hit) const
{
    const engine::Vec2f half{this->bounds.x / 2, this->bounds.y / 2};

    const float left = rect_pos.x - half.x;
    const float top = rect_pos.y - half.y;
    const float right = rect_pos.x + rect_size.x + half.x;
    const float bottom = rect_pos.y + rect_size.y + half.y;

    // Slab method: entry/exit times of the segment through each axis' band.
    constexpr float INF = std::numeric_limits<float>::infinity();

    float near_x = -INF, far_x = INF;
    float near_y = -INF, far_y = INF;

    if (delta.x != 0)
    {
        near_x = (left - start.x) / delta.x;
        far_x = (right - start.x) / delta.x;

        if (near_x > far_x)
        {
            std::swap(near_x, far_x);
        }
    }
    else if (start.x <= left || start.x >= right)
    {
        return false;
    }

    if (delta.y != 0)
    {
        near_y = (top - start.y) / delta.y;
        far_y = (bottom - start.y) / delta.y;

        if (near_y > far_y)
        {
            std::swap(near_y, far_y);
        }
    }
    else if (start.y <= top || start.y >= bottom)
    {
        return false;
    }

    const float entry = std::max(near_x, near_y);
    const float exit = std::min(far_x, far_y);

    if (entry >= exit || entry >= 1 || exit <= 0)
    {
        return false;
    }

    if (entry < 0)
    {
        // Already overlapping when the tick began (e.g. squeezed in by a
        // wall bounce): eject through the closest face right away, and only
        // turn the velocity around if it still points into the brick.
        hit.t = 0;

        const float pen_left = start.x - left;
        const float pen_right = right - start.x;
        const float pen_top = start.y - top;
        const float pen_bottom = bottom - start.y;

        if (std::min(pen_left, pen_right) <= std::min(pen_top, pen_bottom))
        {
            const bool out_left = pen_left <= pen_right;

            hit.eject.x = out_left ? -(pen_left + SWEEP_SKIN) : pen_right + SWEEP_SKIN;
            hit.flip_x = out_left ? this->velocity.x > 0 : this->velocity.x < 0;
        }
        else
        {
            const bool out_top = pen_top <= pen_bottom;

            hit.eject.y = out_top ? -(pen_top + SWEEP_SKIN) : pen_bottom + SWEEP_SKIN;
            hit.flip_y = out_top ? this->velocity.y > 0 : this->velocity.y < 0;
        }

        return true;
    }

    hit.t = entry;

    // The axis that entered last names the struck face; an exact corner
    // tie mirrors both, as if hitting the corner point head on.
    hit.flip_x = near_x >= near_y;
    hit.flip_y = near_y >= near_x;

    return true;
}

bool CBall::is_removable() const
{
    return this->removable;
}

void CBall::set_pierce(bool b)
{
    this->pierce = b;
}

void CBall::set_net(bool b)
{
    this->net = b;
}

void CBall::set_velocity(const engine::Vec2f& v)
{
    this->velocity = v;
    // Keep the target speed in sync so the next bounce or Magnus
    // renormalization doesn't snap the ball back to its previous speed.
    this->vel = v.length();
}

void CBall::scale_velocity(float factor)
{
    this->velocity *= factor;
    this->vel *= factor;
}
