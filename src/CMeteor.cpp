#include "CMeteor.hpp"

#include "assets.h"
#include "engine/Window.hpp"

#include <algorithm>
#include <cstdlib>

void CMeteor::launch(const engine::Vec2f& target)
{
    const float offset =
        static_cast<float>(std::rand() % static_cast<int>(2 * ENTRY_SPREAD)) - ENTRY_SPREAD;

    this->pos.x = std::clamp(target.x + offset, 0.f, game::WIDTH - SIZE);
    this->pos.y = -SIZE * 3;

    this->vel = (target - this->get_position()).normalized() * SPEED;

    this->trail.clear();
    this->active = true;
}

void CMeteor::update(const float dt, const engine::Vec2f& target, bool has_target)
{
    if (!this->active)
    {
        return;
    }

    if (has_target)
    {
        // Blend the velocity toward the intercept course a little each
        // tick: enough to track a bouncing ball, too lazy to look guided.
        const engine::Vec2f desired = (target - this->get_position()).normalized() * SPEED;

        this->vel += (desired - this->vel) * std::min(1.f, HOMING * dt);
        this->vel = this->vel.normalized() * SPEED;
    }

    this->pos += this->vel * dt;

    this->trail.push_back(this->pos);

    if (this->trail.size() > TRAIL_LENGTH)
    {
        this->trail.erase(this->trail.begin());
    }

    if (this->pos.y > game::HEIGHT + SIZE)
    {
        this->active = false;
    }
}

void CMeteor::draw(engine::Window& target) const
{
    if (!this->active)
    {
        return;
    }

    // The burning tail: older segments smaller, dimmer and redder,
    // with a yellow-hot streak inside the newest ones.
    for (unsigned int i = 0; i < this->trail.size(); ++i)
    {
        const float age = (i + 1.f) / (this->trail.size() + 1.f);

        engine::Color color{255, static_cast<std::uint8_t>(70 + age * 130),
                            static_cast<std::uint8_t>(15 + age * 35), 0};
        color.a = static_cast<std::uint8_t>(age * 220);

        const float size = SIZE * (0.35f + 0.75f * age);
        const float inset = (SIZE - size) / 2;

        target.drawRect(this->trail[i] + engine::Vec2f{inset, inset}, {size, size}, color);

        if (age > 0.6f)
        {
            engine::Color hot{255, 230, 120, static_cast<std::uint8_t>(age * 180)};

            target.drawRect(this->trail[i] +
                                engine::Vec2f{inset + size * 0.25f, inset + size * 0.25f},
                            {size * 0.5f, size * 0.5f}, hot);
        }
    }

    // The rocky core: a soft heat halo, a dark slab, a lit face and a
    // hot leading edge.
    target.drawRect(this->pos - engine::Vec2f{3.f, 3.f}, {SIZE + 6.f, SIZE + 6.f},
                    {255, 140, 40, 70});
    target.drawRect(this->pos, {SIZE, SIZE}, {90, 78, 66, 255});
    target.drawRect(this->pos + engine::Vec2f{2.f, 2.f}, {SIZE - 6.f, SIZE - 6.f},
                    {140, 126, 110, 255});
    target.drawRect(this->pos + engine::Vec2f{2.f, SIZE - 3.f}, {SIZE - 4.f, 3.f},
                    {255, 180, 60, 255});
}

bool CMeteor::is_active() const
{
    return this->active;
}

void CMeteor::expire()
{
    this->active = false;
}

bool CMeteor::overlaps(const engine::Vec2f& other_pos, const engine::Vec2f& size) const
{
    if (!this->active)
    {
        return false;
    }

    return this->pos.x < other_pos.x + size.x && other_pos.x < this->pos.x + SIZE &&
           this->pos.y < other_pos.y + size.y && other_pos.y < this->pos.y + SIZE;
}

engine::Vec2f CMeteor::get_position() const
{
    return this->pos + engine::Vec2f{SIZE / 2, SIZE / 2};
}
