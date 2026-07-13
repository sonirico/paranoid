#pragma once

#include "engine/Vec2.hpp"

#include <string>

// Game-wide constants: window, timing, stage layouts and asset ids.
namespace game
{
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const std::string TITLE = "PARANOID";

const unsigned int FRAMES = 60;
const float TIME_PER_FRAME = 1.f / FRAMES;

namespace game_states
{
enum states
{
    NULLSTATE = 0,
    MENU,
    PLAY,
    MAP
};
};

namespace game_textures
{
enum textures
{
    BACKGROUND,
    MAIN,
    BRICKS,
    FONT
};
};

namespace game_bonus
{
enum bonus
{
    B = 0,
    C,
    D,
    E,
    L,
    M,
    N,
    P,
    R,
    S,
    T,
    X,
    COUNT
};
};

namespace game_bricks
{
enum bricks
{
    NONE = 0,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    AQUA,
    BLUE,
    PURPPLE,
    WHITE,
    SILVER,
    GOLD,
    UNDESTROYABLE,
    COUNT
};

const unsigned int BRICKS_PER_LINE = 15;
const unsigned int LINES = 15;
const engine::Vec2f BRICK_BOUND = engine::Vec2f{16, 8};
const unsigned int MARGIN_LEFT =
    (game::WIDTH - (BRICKS_PER_LINE * static_cast<unsigned int>(BRICK_BOUND.x) * 2)) / 2;
const unsigned int MARGIN_TOP = 64;

}; // namespace game_bricks

namespace game_fx
{
enum fx
{
    REBOTE1 = 0,
    REBOTE2,
    MUERTE,
    OPTION,
    POINTS,
    LIFEUP,
    STICKY,
    SELECT,
    COUNT
};
};

}; // namespace game
