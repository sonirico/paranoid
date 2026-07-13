#pragma once

#include "CBrick.hpp"
#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"

// A laser shot fired from the paddle; flies straight up and takes a
// life off the first brick it touches.
class CLaser : public CEntity
{
  public:
    explicit CLaser(CState* st);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    bool collision_laser_brick(CBrick* b);
    bool is_removable() const;

  private:
    engine::Animation animation;

    bool removable = false;
};
