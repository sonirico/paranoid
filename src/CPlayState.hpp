#pragma once

#include "CBall.hpp"
#include "CBonus.hpp"
#include "CBrick.hpp"
#include "CLaser.hpp"
#include "CMenu.hpp"
#include "CPaddle.hpp"
#include "CState.hpp"
#include "assets.h"

#include <list>
#include <memory>

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

    // How long a mode capsule stays in effect before wearing off.
    static constexpr float BONUS_DURATION = 10.f;

    // Applies a collected bonus capsule to the game.
    void apply_bonus(game::game_bonus::bonus type);

    // Spawns a pair of shots from the paddle edges (laser mode only).
    void fire_lasers();

  private:
    // Records every moving entity's position so render can interpolate.
    void snapshot_entities();

    // Puts a mode capsule in effect and starts its countdown.
    void arm_active_bonus(game::game_bonus::bonus type);

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

    game::game_bonus::bonus active_bonus = game::game_bonus::COUNT;
    float bonus_time_left = 0;
};
