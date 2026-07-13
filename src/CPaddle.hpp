#pragma once

#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

#include <vector>

class CPaddle : public CEntity
{
  public:
    explicit CPaddle(CState* st);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    // -1 left, 0 idle, 1 right; consumed by sticky-ball logic.
    int dir = 0;

  private:
    void check_bounds();

    engine::Animation animation;

    std::vector<engine::IntRect> rects;
};
