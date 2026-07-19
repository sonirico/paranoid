#pragma once

#include "CBall.hpp"
#include "CBonus.hpp"
#include "CBrick.hpp"
#include "CLaser.hpp"
#include "CMenu.hpp"
#include "CMeteor.hpp"
#include "CPaddle.hpp"
#include "CState.hpp"
#include "assets.h"

#include <cstddef>
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <vector>

class CGameContainer;

// The gameplay screen: paddle, balls, brick wall and falling bonuses.
class CPlayState : public CState
{
  public:
    explicit CPlayState(CGameContainer* gc);

    void init() override;
    void events() override;
    int update(const float dt) override;
    void render() override;
    void clear() override;

    void load_bricks();

    void update_balls(const float dt);
    // Returns true when the last destructible brick fell.
    bool update_bricks(const float dt);
    void update_paddle(const float dt);
    void update_bonus(const float dt);
    // Ticks the mode capsule countdown, restoring the paddle on expiry.
    void update_active_bonus(const float dt);

    void render_balls();
    void render_bricks();
    void render_paddle();
    void render_bonus();

    void next_stage();

    void insert_bonus(CBrick* b);

    static unsigned int get_current_stage();

    unsigned int get_lives() const;
    unsigned int get_score() const;
    unsigned int get_high_score() const;
    std::list<std::unique_ptr<CBall>>& get_balls();
    std::list<std::unique_ptr<CBrick>>& get_bricks();
    std::list<std::unique_ptr<CLaser>>& get_lasers();
    CPaddle* get_paddle();
    CMeteor* get_meteor();

    // The paddle-mode capsule currently in effect (COUNT means none).
    game::game_bonus::bonus get_active_bonus() const;

    std::size_t get_particle_count() const;
    std::size_t get_floating_text_count() const;

    // Bricks killed since the ball last came back off the paddle.
    unsigned int get_combo() const;

    // How long a mode capsule stays in effect before wearing off.
    static constexpr float BONUS_DURATION = 10.f;

    // Fallback length of the round card when no jingle could load; with
    // one, the card lasts the jingle plus a quarter-second beat.
    static constexpr float ROUND_INTRO_DURATION = 4.f;

    // Applies a collected bonus capsule to the game.
    void apply_bonus(game::game_bonus::bonus type);

    // Spawns a pair of shots from the paddle edges (laser mode only).
    void fire_lasers();

  private:
    // The screen's coarse mode: a frozen intro card, live gameplay, or
    // the game-over card before returning to the menu.
    enum class Phase
    {
        Intro,
        Playing,
        GameOver
    };

    // Freezes the game behind the "ROUND N" + "READY" card; plain
    // respawns show only "READY".
    void enter_intro(bool show_round);

    void render_phase_cards();

    // Records every moving entity's position so render can interpolate.
    void snapshot_entities();

    // Puts a mode capsule in effect and starts its countdown.
    void arm_active_bonus(game::game_bonus::bonus type);

    // Drops every mode: paddle modes and per-ball megaball/net flags.
    void cancel_active_bonus();

    // A short-lived colored square from a dying brick.
    struct Particle
    {
        engine::Vec2f pos;
        engine::Vec2f vel;
        float life = 0;
        engine::Color color;
    };

    void spawn_brick_particles(CBrick* brick);
    // A small burst where the ball just bounced or a brick took a hit.
    void spawn_impact_sparks(const engine::Vec2f& center, const engine::Color& color);
    // A bigger celebration burst in the capsule's color (pickups).
    void spawn_pickup_burst(const engine::Vec2f& center, const engine::Color& color);
    void update_particles(const float dt);
    void render_particles();

    // A short label that rises and fades (capsule names, score pops).
    struct FloatingText
    {
        std::string text;
        engine::Vec2f pos;
        float life = 0;
        engine::Color color;
    };

    void spawn_floating_text(const std::string& text, const engine::Vec2f& pos,
                             const engine::Color& color);
    void update_floating_texts(const float dt);
    void render_floating_texts();

    // The capsule letter's display name and burst color.
    static std::string get_bonus_name(game::game_bonus::bonus type);
    static engine::Color get_bonus_color(game::game_bonus::bonus type);

    // Kicks the screen sideways for a moment (life loss, game over).
    void start_shake(float duration, float strength);

    // Blows the paddle up where it stands (life loss); rendered in
    // place of the paddle while it runs.
    void start_paddle_death();

    // Starts the track the current stage drew from the pool.
    void play_stage_music();

    // Draws a random track for the stage, never an immediate repeat.
    void pick_stage_track();

    // Called when the last ball is lost; restarts the run at 0 lives.
    void lose_life();
    void spawn_ball();

    void render_lives();
    void render_active_bonus();
    void render_pause_menu();
    void render_score();

    // The high score lives in gc->data_dir; no-ops when unset (tests).
    void load_high_score();
    void save_high_score();

    void update_lasers(const float dt);
    void render_lasers();

    // Launches a meteor at the first stalled ball, steers the one in
    // flight and knocks any ball it strikes downward.
    void update_meteor(const float dt);
    void render_meteor();

    std::unique_ptr<CPaddle> paddle;

    std::list<std::unique_ptr<CBall>> balls;
    std::list<std::unique_ptr<CBrick>> bricks;
    std::list<std::unique_ptr<CBonus>> bonus;
    std::list<std::unique_ptr<CLaser>> lasers;

    // The anti-stall rock, one at most in flight, and the ball it
    // falls toward; null once that ball is gone or was struck.
    CMeteor meteor;
    CBall* meteor_target = nullptr;

    bool fire_was_down = false;

    // Escape pauses the run and opens Resume / Main menu / Quit over it.
    bool paused = false;
    bool esc_was_down = false;
    std::unique_ptr<CMenu> pause_menu;

    static unsigned int current_stage;
    unsigned int total_bricks = 0;

    // Index into the stage track pool for the current stage; the
    // out-of-range start means the first draw excludes nothing.
    std::size_t stage_track = std::numeric_limits<std::size_t>::max();

    static const unsigned int STARTING_LIVES = 3;
    unsigned int lives = STARTING_LIVES;

    unsigned int score = 0;
    unsigned int high_score = 0;

    Phase phase = Phase::Intro;
    float phase_time = 0;
    bool intro_shows_round = true;

    static constexpr float READY_DURATION = 1.5f;
    static constexpr float GAME_OVER_DURATION = 3.f;

    // Hit-stop: gameplay freezes this long when a brick dies, so the
    // impact reads harder.
    static constexpr float HITSTOP_DURATION = 0.05f;
    float hitstop_time = 0;

    // A meteor strike freezes far longer than a brick pop: enough to
    // take the redirect in, short enough to leave the ball reachable.
    static constexpr float METEOR_HITSTOP_DURATION = 0.6f;

    // Kills in the current combo run; every COMBO_KILLS_PER_MULT of
    // them raise the score multiplier by one and sharpen the kill pop.
    unsigned int combo = 0;

    static constexpr unsigned int COMBO_KILLS_PER_MULT = 4;
    static constexpr float COMBO_PITCH_STEP = 0.05f;
    static constexpr float COMBO_PITCH_MAX = 1.6f;

    game::game_bonus::bonus active_bonus = game::game_bonus::COUNT;
    float bonus_time_left = 0;

    // Speed scaling from a slow/fast capsule (S/P), undone on cancel.
    float ball_speed_factor = 1.f;

    // A break capsule (B) was caught: jump stages once the bonus pass
    // finishes, so the capsule list is not cleared mid-iteration.
    bool break_pending = false;

    std::vector<Particle> particles;
    std::vector<FloatingText> floating_texts;

    float shake_time = 0;
    float shake_strength = 0;

    // Explosion frames left to show where the paddle just died.
    float paddle_death_time = 0;
    engine::Vec2f paddle_death_pos;

    static constexpr float PARTICLE_GRAVITY = 400.f;
    static constexpr unsigned int PARTICLES_PER_BRICK = 10;
    static constexpr unsigned int SPARKS_PER_IMPACT = 4;
    static constexpr unsigned int PARTICLES_PER_PICKUP = 12;

    static constexpr float FLOATING_TEXT_LIFE = 0.9f;
    static constexpr float FLOATING_TEXT_RISE = 45.f;

    static constexpr float PADDLE_DEATH_DURATION = 0.6f;
    static constexpr unsigned int PADDLE_DEATH_FRAMES = 5;
};
