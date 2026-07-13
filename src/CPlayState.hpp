#pragma once

#include "CBall.hpp"
#include "CBonus.hpp"
#include "CBrick.hpp"
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

    void render_balls();
    void render_bricks();
    void render_paddle();
    void render_bonus();

    void next_stage();

    void insert_bonus(CBrick* b);

    static unsigned int get_current_stage();

  private:
    std::unique_ptr<CPaddle> paddle;

    std::list<std::unique_ptr<CBall>> balls;
    std::list<std::unique_ptr<CBrick>> bricks;
    std::list<std::unique_ptr<CBonus>> bonus;

    static unsigned int current_stage;
    unsigned int total_bricks = 0;
};
