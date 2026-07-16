#include "CStarfield.hpp"

#include "assets.h"
#include "engine/Window.hpp"

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

void CStarfield::update(const float dt)
{
    this->time += dt;

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
        // The twinkle wave breathes each star between 60% and 100%.
        const float wave =
            0.8f + 0.2f * std::sin(2.f * 3.14159265f * star.twinkle_hz * this->time + star.phase);

        engine::Color color = star.color;
        color.a = static_cast<std::uint8_t>(color.a * wave);

        target.drawRect(star.pos, {star.size, star.size}, color);
    }
}

std::size_t CStarfield::get_star_count() const
{
    return this->stars.size();
}
