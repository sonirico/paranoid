#pragma once

#include "CEntity.hpp"
#include "assets.h"
#include "engine/Animation.hpp"
#include "engine/Color.hpp"

#include <vector>

class CBrick : public CEntity
{
  public:
    CBrick(CState* state, game::game_bricks::bricks t);

    void init() override;
    void update(const float dt) override;
    void reset() override;

    // Draws the sprite plus a fading white overlay while a hit flashes.
    void draw(engine::Window& target) const override;

    void quit_life();

    bool is_removable() const;
    bool is_flashing() const;

    unsigned int get_score() const;

    // The brick's dominant sprite color, e.g. for death particles.
    engine::Color get_color() const;

  public:
    game::game_bricks::bricks type;

  private:
    // Fills score, lifes and animation frames according to the type.
    void settings();

    engine::Animation animation;

    unsigned int lifes = 0;
    unsigned int score = 0;

    bool removable = false;

    // Time left on the white blink a hit triggers.
    float flash_time = 0;

    static constexpr float FLASH_DURATION = 0.1f;

    std::vector<engine::IntRect> rects;
};
