#pragma once

#include "CBall.hpp"
#include "CBonus.hpp"
#include "CBrick.hpp"
#include "CLaser.hpp"
#include "CMenu.hpp"
#include "CPaddle.hpp"
#include "CState.hpp"
#include "assets.h"

#include <cstddef>
#include <list>
#include <memory>
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

    // The paddle-mode capsule currently in effect (COUNT means none).
    game::game_bonus::bonus get_active_bonus() const;

    std::size_t get_particle_count() const;

    // How long a mode capsule stays in effect before wearing off.
    static constexpr float BONUS_DURATION = 10.f;

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
    void update_particles(const float dt);
    void render_particles();

    // Kicks the screen sideways for a moment (life loss, game over).
    void start_shake(float duration, float strength);

    // Blows the paddle up where it stands (life loss); rendered in
    // place of the paddle while it runs.
    void start_paddle_death();

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

    std::unique_ptr<CPaddle> paddle;

    std::list<std::unique_ptr<CBall>> balls;
    std::list<std::unique_ptr<CBrick>> bricks;
    std::list<std::unique_ptr<CBonus>> bonus;
    std::list<std::unique_ptr<CLaser>> lasers;

    bool fire_was_down = false;

    // Escape pauses the run and opens Resume / Main menu / Quit over it.
    bool paused = false;
    bool esc_was_down = false;
    std::unique_ptr<CMenu> pause_menu;

    static unsigned int current_stage;
    unsigned int total_bricks = 0;

    static const unsigned int STARTING_LIVES = 3;
    unsigned int lives = STARTING_LIVES;

    unsigned int score = 0;
    unsigned int high_score = 0;

    Phase phase = Phase::Intro;
    float phase_time = 0;
    bool intro_shows_round = true;

    static constexpr float ROUND_INTRO_DURATION = 2.f;
    static constexpr float READY_DURATION = 1.5f;
    static constexpr float GAME_OVER_DURATION = 3.f;

    game::game_bonus::bonus active_bonus = game::game_bonus::COUNT;
    float bonus_time_left = 0;

    // A break capsule (B) was caught: jump stages once the bonus pass
    // finishes, so the capsule list is not cleared mid-iteration.
    bool break_pending = false;

    std::vector<Particle> particles;

    float shake_time = 0;
    float shake_strength = 0;

    // Explosion frames left to show where the paddle just died.
    float paddle_death_time = 0;
    engine::Vec2f paddle_death_pos;

    static constexpr float PARTICLE_GRAVITY = 400.f;
    static constexpr unsigned int PARTICLES_PER_BRICK = 10;

    static constexpr float PADDLE_DEATH_DURATION = 0.6f;
    static constexpr unsigned int PADDLE_DEATH_FRAMES = 5;
};
