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

    // True once the ball has flown near-horizontally for STALL_TIME
    // without touching the paddle; the play state calls a meteor down
    // on it. Paddle bounces and external redirects reset the clock.
    bool is_stalled() const;

    // Meteor crash: the ball staggers slowly and erratically down
    // toward the paddle until the daze wears off (or the paddle bounces
    // it), then resumes at its pre-crash speed.
    void daze();
    bool is_dazed() const;

    // How long the flight may stay flat before counting as stalled.
    static constexpr float STALL_TIME = 6.f;

    // Draws the fading comet tail behind the sprite.
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
    static constexpr float BASE_VEL = 400.f;
    static constexpr float VEL_RAMP = 1.025f;
    static constexpr float MAX_VEL = 720.f;

    float vel = BASE_VEL;

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

    // Seconds the flight has stayed flatter than STALL_RATIO; wall and
    // brick bounces preserve the angle, so only the paddle (or a
    // meteor's redirect) can wind it back to zero.
    float stall_time = 0;

    // |vy| below this fraction of the speed counts as flying flat: just
    // above the widest paddle bounce (~0.39), so even an edge shot that
    // rallies too long eventually calls the meteor.
    static constexpr float STALL_RATIO = 0.45f;

    // A flat ball already dropping onto the field within this many
    // seconds is coming to the player, not stalled.
    static constexpr float DESCENT_GRACE = 4.f;

    // Whether the ball is sinking and reaches the floor within the
    // descent grace.
    bool on_final_approach() const;

    // Drives the dazed wobble and ends it when the timer runs out.
    void apply_daze(const float dt);

    // Seconds of daze left, and the sway wave's running angle.
    float daze_time = 0;
    float daze_phase = 0;

    static constexpr float DAZE_DURATION = 6.f;
    static constexpr float DAZE_SPEED = 120.f; // A crawl next to BASE_VEL.
    static constexpr float DAZE_SWAY = 0.8f;   // Lateral wobble amplitude.

    // Recent tick positions, oldest first; rendered as a fading comet
    // tail behind the flying ball, colored by the mode in effect.
    std::vector<engine::Vec2f> trail;

    static constexpr unsigned int TRAIL_LENGTH = 8;
    static constexpr float TRAIL_MIN_SPIN = 1.f;

    // Separation left after each bounce so float rounding cannot
    // re-detect the face the ball just left.
    static constexpr float SWEEP_SKIN = 0.05f;
};
