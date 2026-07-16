#include "CStarfield.hpp"

#include "assets.h"
#include "engine/Window.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

CStarfield::CStarfield()
{
    for (unsigned int layer = 0; layer < LAYERS; ++layer)
    {
        for (unsigned int i = 0; i < STARS_PER_LAYER; ++i)
        {
            this->stars.push_back(this->spawn_star(layer));
        }
    }
}

CStarfield::Star CStarfield::spawn_star(unsigned int layer) const
{
    Star star;

    star.pos = {static_cast<float>(std::rand() % game::WIDTH),
                static_cast<float>(std::rand() % game::HEIGHT)};

    // Nearer layers are bigger, faster and brighter.
    star.size = static_cast<float>(layer + 1);
    star.speed = 6.f + layer * 8.f;
    star.twinkle_hz = 0.3f + (std::rand() % 120) / 100.f;
    star.phase = (std::rand() % 628) / 100.f;

    // Mostly white with the odd tinted one, like the old backdrop.
    const int roll = std::rand() % 100;

    if (roll < 70)
    {
        star.color = {255, 255, 255, 255};
    }
    else if (roll < 90)
    {
        star.color = {170, 180, 220, 255};
    }
    else if (roll < 95)
    {
        star.color = {216, 40, 0, 255};
    }
    else
    {
        star.color = {0, 168, 0, 255};
    }

    star.color.a = static_cast<std::uint8_t>(90 + layer * 60);

    return star;
}

void CStarfield::spawn_shooting_star()
{
    // Streaks diagonally across the top third, left or right at random.
    const float direction = std::rand() % 2 == 0 ? 1.f : -1.f;

    this->shot_pos = {static_cast<float>(std::rand() % game::WIDTH),
                      static_cast<float>(std::rand() % (game::HEIGHT / 3))};
    this->shot_vel = {direction * (250.f + std::rand() % 150), 120.f + std::rand() % 80};
    this->shot_life = SHOT_LIFE;
    this->shot_cooldown = 8.f + std::rand() % 12;
}

void CStarfield::pulse(float strength)
{
    this->pulse_level = std::min(1.f, this->pulse_level + strength);
}

float CStarfield::get_pulse() const
{
    return this->pulse_level;
}

void CStarfield::update(const float dt)
{
    this->time += dt;

    this->pulse_level -= this->pulse_level * 3.f * dt;

    if (this->shot_life > 0)
    {
        this->shot_life -= dt;
        this->shot_pos += this->shot_vel * dt;
    }
    else
    {
        this->shot_cooldown -= dt;

        if (this->shot_cooldown <= 0)
        {
            this->spawn_shooting_star();
        }
    }

    for (Star& star : this->stars)
    {
        star.pos.y += star.speed * dt;

        // Wrap to the top on a fresh column.
        if (star.pos.y > game::HEIGHT)
        {
            star.pos.y -= game::HEIGHT + star.size;
            star.pos.x = static_cast<float>(std::rand() % game::WIDTH);
        }
    }
}

void CStarfield::draw(engine::Window& target) const
{
    for (const Star& star : this->stars)
    {
        // The twinkle wave breathes each star between 60% and 100%;
        // a pulse from the game lifts the whole field on top of it.
        const float wave =
            0.8f + 0.2f * std::sin(2.f * 3.14159265f * star.twinkle_hz * this->time + star.phase);
        const float boost = 1.f + this->pulse_level * 0.8f;

        engine::Color color = star.color;
        color.a = static_cast<std::uint8_t>(std::min(255.f, color.a * wave * boost));

        target.drawRect(star.pos, {star.size, star.size}, color);
    }

    if (this->shot_life > 0)
    {
        // A bright head trailing segments that fade along the path.
        const engine::Vec2f step = this->shot_vel * 0.06f;
        const float fade = std::min(1.f, this->shot_life / (SHOT_LIFE * 0.5f));

        for (int i = 0; i < 3; ++i)
        {
            engine::Color color = engine::Color::White;
            color.a = static_cast<std::uint8_t>(fade * (180 - i * 60));

            target.drawLine(this->shot_pos - step * static_cast<float>(i),
                            this->shot_pos - step * static_cast<float>(i + 1), color);
        }
    }
}

std::size_t CStarfield::get_star_count() const
{
    return this->stars.size();
}

bool CStarfield::has_shooting_star() const
{
    return this->shot_life > 0;
}
