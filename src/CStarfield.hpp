#pragma once

#include "engine/Color.hpp"
#include "engine/Vec2.hpp"

#include <cstddef>
#include <vector>

namespace engine
{
class Window;
}

// The space behind every state: three parallax layers of twinkling
// stars drifting slowly down, generated at startup instead of the old
// static image.
class CStarfield
{
  public:
    CStarfield();

    void update(const float dt);
    void draw(engine::Window& target) const;

    std::size_t get_star_count() const;
    bool has_shooting_star() const;

  private:
    struct Star
    {
        engine::Vec2f pos;
        float size = 1;
        // Downward drift in pixels per second; nearer layers move faster.
        float speed = 0;
        // Twinkle wave: each star breathes on its own frequency and phase.
        float twinkle_hz = 0;
        float phase = 0;
        engine::Color color;
    };

    Star spawn_star(unsigned int layer) const;

    void spawn_shooting_star();

    std::vector<Star> stars;

    // Accumulated time driving the twinkle wave.
    float time = 0;

    // The occasional shooting star: a bright streak that crosses part
    // of the sky, burns out and rearms on a random cooldown.
    engine::Vec2f shot_pos;
    engine::Vec2f shot_vel;
    float shot_life = 0;
    float shot_cooldown = 8.f;

    static constexpr unsigned int LAYERS = 3;
    static constexpr unsigned int STARS_PER_LAYER = 36;
    static constexpr float SHOT_LIFE = 0.7f;
};
