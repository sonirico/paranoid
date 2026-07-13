#include "CPlayState.hpp"

#include "CGameContainer.hpp"
#include "other_functions.hpp"

#include <SDL3/SDL.h>

#include <cstdlib>

using namespace game::game_states;
using namespace game::game_bricks;

unsigned int CPlayState::current_stage;

CPlayState::CPlayState(CGameContainer* gc)
{
    this->gc = gc;
    this->rh = gc->rh;
}

void CPlayState::init()
{
    current_stage = 0;

    this->paddle = std::make_unique<CPaddle>(this);

    this->load_bricks();

    auto b = std::make_unique<CBall>(this, this->paddle.get());
    b->set_in_paddle();
    this->balls.push_back(std::move(b));
}

void CPlayState::events()
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_SPACE])
    {
        // Release the first ball still parked on the paddle.
        for (auto& ball : this->balls)
        {
            if (ball->is_in_paddle())
            {
                ball->set_in_paddle(false);
                break;
            }
        }
    }
    if (keys[SDL_SCANCODE_A])
    {
        // Developer cheat: spawn an extra ball on the paddle.
        auto b = std::make_unique<CBall>(this, this->paddle.get());
        b->set_in_paddle();
        this->balls.push_back(std::move(b));
    }
}

int CPlayState::update(const float dt)
{
    this->update_paddle(dt);
    this->update_balls(dt);
    this->update_bonus(dt);

    if (this->update_bricks(dt))
    {
        this->next_stage();
    }

    return NULLSTATE;
}

void CPlayState::render()
{
    this->render_paddle();
    this->render_bricks();
    this->render_balls();
    this->render_bonus();
}

void CPlayState::clear()
{
    this->balls.clear();
    this->bonus.clear();
    this->bricks.clear();
}

void CPlayState::update_balls(const float dt)
{
    for (auto it = this->balls.begin(); it != this->balls.end();)
    {
        CBall* ball = it->get();

        if (ball->is_removable())
        {
            it = this->balls.erase(it);
        }
        else
        {
            ball->update(dt);
            ++it;
        }
    }
}

void CPlayState::update_bonus(const float dt)
{
    for (auto it = this->bonus.begin(); it != this->bonus.end();)
    {
        CBonus* b = it->get();

        if (b->is_removable())
        {
            /* TODO: apply the bonus effect to the paddle. */
            it = this->bonus.erase(it);
        }
        else
        {
            b->update(dt);
            b->collision_bonus_paddle(this->paddle.get());
            ++it;
        }
    }
}

bool CPlayState::update_bricks(const float dt)
{
    for (auto it = this->bricks.begin(); it != this->bricks.end();)
    {
        CBrick* brick = it->get();
        brick->update(dt);

        for (auto& ball : this->balls)
        {
            if (ball->collision_ball_brick(brick, dt))
            {
                brick->quit_life();
                brick->play_animation();
                break;
            }
        }

        if (brick->is_removable())
        {
            this->insert_bonus(brick);
            it = this->bricks.erase(it);

            this->total_bricks--;

            if (this->total_bricks <= 0)
            {
                return true;
            }
        }
        else
        {
            ++it;
        }
    }

    return false;
}

void CPlayState::update_paddle(const float dt)
{
    this->paddle->update(dt);
}

void CPlayState::render_balls()
{
    for (auto& ball : this->balls)
    {
        this->gc->window->draw(*ball);
    }
}

void CPlayState::render_bricks()
{
    for (auto& brick : this->bricks)
    {
        this->gc->window->draw(*brick);
    }
}

void CPlayState::render_bonus()
{
    for (auto& b : this->bonus)
    {
        this->gc->window->draw(*b);
    }
}

void CPlayState::render_paddle()
{
    this->gc->window->draw(*(this->paddle));
}

void CPlayState::load_bricks()
{
    this->bricks.clear();

    const unsigned int* current_map = get_stage_map(current_stage);

    if (current_map == nullptr)
    {
        return;
    }

    this->total_bricks = 0;

    for (unsigned int i = 0; i < (BRICKS_PER_LINE * LINES); ++i)
    {
        game::game_bricks::bricks type = static_cast<game::game_bricks::bricks>(current_map[i]);

        if (type != NONE)
        {
            auto b = std::make_unique<CBrick>(this, type);

            unsigned int row = i / LINES;
            unsigned int column = i % BRICKS_PER_LINE;

            unsigned int x = MARGIN_LEFT + BRICK_BOUND.x * 2 * row;
            unsigned int y = MARGIN_TOP + BRICK_BOUND.y * 2 * column;

            b->setPosition(x, y);

            this->bricks.push_back(std::move(b));

            // Only destructible bricks count towards clearing the stage.
            if (type != UNDESTROYABLE)
            {
                this->total_bricks++;
            }
        }
    }
}

void CPlayState::next_stage()
{
    current_stage++;

    this->balls.clear();
    this->bonus.clear();

    this->load_bricks();

    this->paddle->reset();

    auto b = std::make_unique<CBall>(this, this->paddle.get());
    b->set_in_paddle();
    this->balls.push_back(std::move(b));
}

void CPlayState::insert_bonus(CBrick* brick)
{
    // 5% chance of dropping a bonus at all.
    unsigned int percent = (std::rand() % 1000) + 1;

    if (percent > 50)
    {
        return;
    }

    // Then weight which bonus letter comes out.
    percent = (std::rand() % 100) + 1;

    game::game_bonus::bonus type;

    if (percent <= 5)
    { // Extra life
        type = game::game_bonus::X;
    }
    else if (percent <= 12)
    { // Laser
        type = game::game_bonus::L;
    }
    else if (percent <= 25)
    { // Expand
        type = game::game_bonus::E;
    }
    else if (percent <= 40)
    { // Shrink
        type = game::game_bonus::R;
    }
    else if (percent <= 55)
    { // Sticky paddle
        type = game::game_bonus::C;
    }
    else if (percent <= 70)
    { // Multi-ball
        type = game::game_bonus::D;
    }
    else if (percent <= 85)
    { // Slow balls down
        type = game::game_bonus::S;
    }
    else
    { // Speed up
        type = game::game_bonus::P;
    }

    auto b = std::make_unique<CBonus>(this, type);
    b->setPosition(brick->getPosition());

    this->bonus.push_back(std::move(b));
}

unsigned int CPlayState::get_current_stage()
{
    return current_stage;
}
