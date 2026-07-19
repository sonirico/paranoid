#pragma once

#include "CBrick.hpp"
#include "CEntity.hpp"
#include "assets.h"

// A fire bolt shot from the paddle; flies straight up and takes a
// life off the first brick it touches. Drawn with primitives: a hot
// core in a red casing trailing a short wake.
class CLaser : public CEntity
{
  public:
    explicit CLaser(CState* st);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    void draw(engine::Window& target) const override;

    bool collision_laser_brick(CBrick* b);
    bool is_removable() const;

  private:
    bool removable = false;
};
