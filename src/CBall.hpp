#pragma once

#include "CBrick.hpp"
#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

class CPaddle; // Breaks the CBall <--> CPaddle circular dependency.

class CBall : public CEntity
{
  public:
    CBall(CState* st, CPaddle* pd);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    // Parks the ball on top of the paddle (serve position).
    void set_in_paddle();
    void set_in_paddle(bool b);

    bool is_in_paddle() const;
    bool is_removable() const;

    bool collision_ball_brick(CBrick* b, const float dt);

  private:
    void collision_ball_paddle();

    void check_bounds();

    bool check_point_rect(engine::Vec2f point, CBrick* b) const;
    bool check_collision(CBrick* b);

    engine::Animation animation;

    CPaddle* paddle;

    bool in_paddle = false;

    bool removable = false;

    int vel = 400;

    // Set by check_collision when the velocity flipped on that axis.
    bool dirx = false;
    bool diry = false;
};
