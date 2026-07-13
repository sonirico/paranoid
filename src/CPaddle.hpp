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

    // Bonus-driven modes. Applying any mode clears the previous one.
    void reset_modes();
    void expand();
    void shrink();
    void set_sticky(bool b);
    bool is_sticky() const;
    void set_laser(bool b);
    bool has_laser() const;

    // -1 left, 0 idle, 1 right; consumed by sticky-ball logic.
    int dir = 0;

  private:
    void check_bounds();

    void apply_width_factor(float factor);

    engine::Animation animation;

    std::vector<engine::IntRect> rects;

    float width_factor = 1.f;
    bool sticky = false;
    bool laser = false;
};
