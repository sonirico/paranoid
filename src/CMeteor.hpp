#pragma once

#include "engine/Vec2.hpp"

#include <vector>

namespace engine
{
class Window;
}

// A rock called down on a ball stuck in a near-horizontal rally: it
// streaks in from above the sky, steers gently toward the ball and
// knocks it downward on impact. Drawn with primitives, like the
// starfield's shooting star.
class CMeteor
{
  public:
    // Enters above the screen, roughly over the target, flying at it.
    void launch(const engine::Vec2f& target);

    // Falls one tick, steering toward the target while there is one;
    // leaving the bottom of the screen deactivates it (a miss).
    void update(const float dt, const engine::Vec2f& target, bool has_target);

    void draw(engine::Window& target) const;

    bool is_active() const;

    // Ends the flight right away (it struck something).
    void expire();

    // Whether the rocky core touches the given rectangle.
    bool overlaps(const engine::Vec2f& pos, const engine::Vec2f& size) const;

    // The core's centre, where impact bursts belong.
    engine::Vec2f get_position() const;

  private:
    engine::Vec2f pos;
    engine::Vec2f vel;

    bool active = false;

    // Recent tick positions, oldest first; rendered as the burning tail.
    std::vector<engine::Vec2f> trail;

    static constexpr float SPEED = 520.f;
    static constexpr float SIZE = 14.f;

    // How fast the fall bends toward the target, per second.
    static constexpr float HOMING = 3.f;

    // Horizontal spread of the entry point over the target.
    static constexpr float ENTRY_SPREAD = 160.f;

    static constexpr unsigned int TRAIL_LENGTH = 16;
};
