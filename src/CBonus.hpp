#pragma once

#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

#include <vector>

class CPaddle;

// A power-up capsule that falls from a destroyed brick.
class CBonus : public CEntity
{
  public:
    CBonus(CState* state, game::game_bonus::bonus t);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    bool collision_bonus_paddle(CPaddle* p);
    bool is_removable() const;

  private:
    // Picks the animation strip for the bonus letter.
    void settings();

    engine::Animation animation;

    game::game_bonus::bonus type;

    std::vector<engine::IntRect> rects;

    bool removable = false;
};
