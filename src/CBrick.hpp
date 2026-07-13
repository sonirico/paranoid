#pragma once

#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

#include <vector>

class CBrick : public CEntity
{
  public:
    CBrick(CState* state, game::game_bricks::bricks t);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    void quit_life();

    bool is_removable() const;

    unsigned int get_score() const;

  public:
    game::game_bricks::bricks type;

  private:
    // Fills score, lifes and animation frames according to the type.
    void settings();

    engine::Animation animation;

    unsigned int lifes = 0;
    unsigned int score = 0;

    bool removable = false;

    std::vector<engine::IntRect> rects;
};
