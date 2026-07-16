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

    std::vector<Star> stars;

    // Accumulated time driving the twinkle wave.
    float time = 0;

    static constexpr unsigned int LAYERS = 3;
    static constexpr unsigned int STARS_PER_LAYER = 36;
};
