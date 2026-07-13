#pragma once

#include "CBrick.hpp"
#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

#include <list>
#include <memory>
#include <vector>

class CPaddle; // Breaks the CBall <--> CPaddle circular dependency.

class CBall : public CEntity
{
  public:
    CBall(CState* st, CPaddle* pd);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    // Parks the ball centered on top of the paddle (serve position).
    void set_in_paddle();
    void set_in_paddle(bool b);

    bool is_in_paddle() const;
    bool is_removable() const;

    // Draws the spin trail behind the sprite when the ball is curving.
    void draw(engine::Window& target) const override;

    // Sweeps this tick's flight path against every brick, bouncing off
    // each one it strikes in path order. Returns the bricks that were hit.
    std::vector<CBrick*> collision_ball_bricks(const std::list<std::unique_ptr<CBrick>>& bricks);

    void set_velocity(const engine::Vec2f& v);

    // Megaball (bonus M): the ball smashes through destructible bricks
    // without bouncing; only indestructible ones still deflect it.
    void set_pierce(bool b);

    // Net (bonus N): a barrier on the floor bounces the ball back up.
    void set_net(bool b);

    // Multiplies the current speed and the base speed used after
    // paddle bounces, so slow/fast bonuses persist.
    void scale_velocity(float factor);

  private:
    void collision_ball_paddle();

    // Parks the ball at the given x offset from the paddle's left edge.
    void park_in_paddle(float offset);

    // Repositions a parked ball, keeping its offset as the paddle moves.
    void follow_paddle();

    // Curves the flight path sideways proportionally to the current spin.
    void apply_magnus(const float dt);

    void check_bounds();

    // How a tick's flight path strikes one brick, as found by
    // sweep_ball_brick: when along the segment, which axes to mirror,
    // and how far to push out a ball that started the tick overlapping.
    struct SweptHit
    {
        float t = 0.f;
        bool flip_x = false;
        bool flip_y = false;
        engine::Vec2f eject;
    };

    // Sweeps the ball's centre along the segment start + delta against a
    // rectangle expanded by half the ball (Minkowski sum). True on impact.
    bool sweep_ball_rect(const engine::Vec2f& start, const engine::Vec2f& delta,
                         const engine::Vec2f& rect_pos, const engine::Vec2f& rect_size,
                         SweptHit& hit) const;

    engine::Animation animation;

    CPaddle* paddle;

    bool in_paddle = false;

    // X distance from the paddle's left edge while parked.
    float paddle_offset = 0;

    bool removable = false;

    bool pierce = false;
    bool net = false;

    // Target speed (the velocity's magnitude): every bounce re-normalizes
    // the velocity to this, so the ball never slows down by hitting at an
    // angle. Ramps up on each paddle bounce until MAX_VEL.
    float vel = 400;

    static constexpr float VEL_RAMP = 1.025f;
    static constexpr float MAX_VEL = 720.f;

    // Spin imparted by a moving spin-mode paddle (bonus T); positive
    // curves clockwise on screen.
    float spin = 0;

    static constexpr float SPIN_FROM_PADDLE = 0.02f; // Per unit of paddle speed.
    static constexpr float MAX_SPIN = 16.f;          // Accumulation cap.
    static constexpr float MAGNUS_COEF = 60.f;       // Spin to lateral acceleration.
    static constexpr float SPIN_DECAY = 1.2f;        // Fraction lost per second.
    static constexpr float SPIN_BOUNCE_DAMP = 0.6f;  // Kept after wall/brick hits.

    // The Magnus curve may never push the flight path closer than ~20
    // degrees to the horizontal: a near-parallel ball diving at the floor
    // gives the player no time to react.
    static constexpr float MIN_VERTICAL_RATIO = 0.342f; // sin(20 degrees)

    // Where the ball started the current physics tick; a ball whose bottom
    // was already past the paddle's save grace can no longer be saved.
    engine::Vec2f tick_start_pos;

    // How deep past the paddle's top edge (as a fraction of the paddle
    // height) the ball's bottom may start a tick and still be bounced: a
    // late paddle slide under a low ball counts as a save, while a ball
    // clearly past the paddle keeps falling with no side bounce.
    static constexpr float SAVE_GRACE = 0.5f;

    // A tick's flight path bounces off at most this many bricks.
    static constexpr unsigned int MAX_BRICK_BOUNCES = 3;

    // Recent tick positions, oldest first; rendered as a fading trail
    // while the ball carries enough spin to visibly curve.
    std::vector<engine::Vec2f> trail;

    static constexpr unsigned int TRAIL_LENGTH = 8;
    static constexpr float TRAIL_MIN_SPIN = 1.f;

    // Separation left after each bounce so float rounding cannot
    // re-detect the face the ball just left.
    static constexpr float SWEEP_SKIN = 0.05f;
};
