#include "CPlayState.hpp"

#include "CGameContainer.hpp"
#include "other_functions.hpp"

#include <SDL3/SDL.h>

#include <cmath>
#include <cstdlib>
#include <string>

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
    this->lives = STARTING_LIVES;

    this->paddle = std::make_unique<CPaddle>(this);

    this->load_bricks();

    this->spawn_ball();
}

void CPlayState::events()
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    // Left click mirrors Space: release the sticky ball, fire the laser.
    const bool fire =
        keys[SDL_SCANCODE_SPACE] || (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK);

    if (fire)
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

        // Laser mode fires once per press.
        if (!this->fire_was_down && this->paddle->has_laser())
        {
            this->fire_lasers();
        }
    }
    this->fire_was_down = fire;
    if (keys[SDL_SCANCODE_A])
    {
        // Developer cheat: spawn an extra ball on the paddle.
        this->spawn_ball();
    }
}

int CPlayState::update(const float dt)
{
    this->snapshot_entities();

    this->update_paddle(dt);
    this->update_balls(dt);
    this->update_bonus(dt);
    this->update_lasers(dt);
    this->update_active_bonus(dt);

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
    this->render_lasers();
    this->render_lives();
    this->render_active_bonus();
}

void CPlayState::render_lives()
{
    // One small paddle icon per remaining life, bottom-left corner.
    engine::Sprite icon;
    icon.setTexture(this->rh->get(game::game_textures::MAIN));
    icon.setTextureRect({128, 72, 32, 8});

    for (unsigned int i = 0; i < this->lives; ++i)
    {
        icon.setPosition(10.f + i * 38.f, game::HEIGHT - 18.f);
        this->gc->window->draw(icon);
    }
}

void CPlayState::render_active_bonus()
{
    using namespace game::game_bonus;

    std::string name;

    switch (this->active_bonus)
    {
    case E:
        name = "EXPAND";
        break;
    case R:
        name = "SHRINK";
        break;
    case C:
        name = "STICKY";
        break;
    case L:
        name = "LASER";
        break;
    case T:
        name = "SPIN";
        break;
    default:
        return;
    }

    // Countdown in whole seconds, space-padded so the right-aligned text
    // doesn't shift when it drops below 10.
    const int seconds = static_cast<int>(std::ceil(this->bonus_time_left));
    name += seconds < 10 ? "  " : " ";
    name += std::to_string(seconds);

    // Capsule icon plus its name, bottom-right corner, mirroring the
    // lives row on the left.
    const float GLYPH = 16.f; // 8px debug font at 2x.
    const float y = game::HEIGHT - 18.f;
    const float x_text = game::WIDTH - 10.f - name.size() * GLYPH;

    engine::Sprite icon;
    icon.setTexture(this->rh->get(game::game_textures::MAIN));
    icon.setTextureRect({176 + 16 * static_cast<int>(this->active_bonus), 0, 16, 8});
    icon.setScale({2.f, 2.f});
    icon.setPosition(x_text - 42.f, y);

    this->gc->window->draw(icon);
    this->gc->window->drawText({x_text, y}, name, engine::Color::White, 2.f);
}

void CPlayState::clear()
{
    this->balls.clear();
    this->bonus.clear();
    this->bricks.clear();
    this->lasers.clear();
}

void CPlayState::snapshot_entities()
{
    this->paddle->snapshot();

    for (auto& ball : this->balls)
    {
        ball->snapshot();
    }
    for (auto& b : this->bonus)
    {
        b->snapshot();
    }
    for (auto& laser : this->lasers)
    {
        laser->snapshot();
    }
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

    if (this->balls.empty())
    {
        this->lose_life();
    }
}

void CPlayState::lose_life()
{
    this->lives--;

    if (this->lives == 0)
    {
        // Game over: restart the whole run.
        current_stage = 0;
        this->lives = STARTING_LIVES;

        this->bonus.clear();
        this->load_bricks();
        this->paddle->reset();
    }

    this->spawn_ball();
}

void CPlayState::spawn_ball()
{
    auto b = std::make_unique<CBall>(this, this->paddle.get());
    b->set_in_paddle();
    this->balls.push_back(std::move(b));
}

void CPlayState::update_bonus(const float dt)
{
    for (auto it = this->bonus.begin(); it != this->bonus.end();)
    {
        CBonus* b = it->get();

        if (b->is_removable())
        {
            it = this->bonus.erase(it);
        }
        else
        {
            b->update(dt);

            if (b->collision_bonus_paddle(this->paddle.get()))
            {
                this->apply_bonus(b->get_type());
            }

            ++it;
        }
    }
}

void CPlayState::apply_bonus(game::game_bonus::bonus type)
{
    using namespace game::game_bonus;

    // Catching any capsule cancels whatever mode was in effect, like in
    // the arcade original.
    this->paddle->reset_modes();
    this->active_bonus = game::game_bonus::COUNT;
    this->bonus_time_left = 0;

    switch (type)
    {
    case E:
        this->paddle->expand();
        this->arm_active_bonus(type);
        break;
    case R:
        this->paddle->shrink();
        this->arm_active_bonus(type);
        break;
    case C:
        this->paddle->set_sticky(true);
        this->arm_active_bonus(type);
        break;
    case L:
        this->paddle->set_laser(true);
        this->arm_active_bonus(type);
        break;
    case T:
        this->paddle->set_spin(true);
        this->arm_active_bonus(type);
        break;
    case D:
    {
        // Disruption: split one flying ball into three, Arkanoid style.
        const unsigned int MAX_BALLS = 8;
        const float SPLIT_ANGLE = 25.f * 3.14159265f / 180.f;

        CBall* source = nullptr;

        for (auto& ball : this->balls)
        {
            if (!ball->is_in_paddle())
            {
                source = ball.get();
                break;
            }
        }

        // Every ball is parked on the paddle: serve the first and split it.
        if (source == nullptr && !this->balls.empty())
        {
            source = this->balls.front().get();
            source->set_in_paddle(false);
        }

        if (source == nullptr)
        {
            break;
        }

        const engine::Vec2f v = source->get_velocity();

        for (const float angle : {-SPLIT_ANGLE, SPLIT_ANGLE})
        {
            if (this->balls.size() >= MAX_BALLS)
            {
                break;
            }

            const float c = std::cos(angle);
            const float s = std::sin(angle);

            auto twin = std::make_unique<CBall>(this, this->paddle.get());
            twin->set_in_paddle(false);
            twin->setPosition(source->getPosition());
            twin->set_velocity({v.x * c - v.y * s, v.x * s + v.y * c});

            this->balls.push_back(std::move(twin));
        }
        break;
    }
    case S:
        for (auto& ball : this->balls)
        {
            ball->scale_velocity(0.7f);
        }
        break;
    case P:
        for (auto& ball : this->balls)
        {
            ball->scale_velocity(1.3f);
        }
        break;
    case X:
        this->lives++;
        break;
    default:
        // B/M/N are on the sprite sheet but have no effect yet.
        break;
    }
}

void CPlayState::arm_active_bonus(game::game_bonus::bonus type)
{
    this->active_bonus = type;
    this->bonus_time_left = BONUS_DURATION;
}

void CPlayState::update_active_bonus(const float dt)
{
    if (this->active_bonus == game::game_bonus::COUNT)
    {
        return;
    }

    this->bonus_time_left -= dt;

    if (this->bonus_time_left <= 0)
    {
        this->paddle->reset_modes();
        this->active_bonus = game::game_bonus::COUNT;
    }
}

void CPlayState::fire_lasers()
{
    const engine::Vec2f p_pos = this->paddle->getPosition();
    const engine::Vec2f p_size = this->paddle->get_size();

    auto left = std::make_unique<CLaser>(this);
    left->setPosition(p_pos.x, p_pos.y - left->get_size().y);

    auto right = std::make_unique<CLaser>(this);
    right->setPosition(p_pos.x + p_size.x - right->get_size().x, p_pos.y - right->get_size().y);

    this->lasers.push_back(std::move(left));
    this->lasers.push_back(std::move(right));
}

void CPlayState::update_lasers(const float dt)
{
    for (auto it = this->lasers.begin(); it != this->lasers.end();)
    {
        CLaser* laser = it->get();

        if (laser->is_removable())
        {
            it = this->lasers.erase(it);
            continue;
        }

        laser->update(dt);

        // The shot dies on the first brick it damages; the brick's
        // removal (and bonus drop) happens in update_bricks.
        for (auto& brick : this->bricks)
        {
            if (laser->collision_laser_brick(brick.get()))
            {
                brick->quit_life();
                brick->play_animation();
                break;
            }
        }

        ++it;
    }
}

bool CPlayState::update_bricks(const float dt)
{
    for (auto& brick : this->bricks)
    {
        brick->update(dt);
    }

    // Each ball sweeps its whole tick's flight against the wall at once,
    // so the nearest brick along the path is resolved first.
    for (auto& ball : this->balls)
    {
        for (CBrick* brick : ball->collision_ball_bricks(this->bricks))
        {
            brick->quit_life();
            brick->play_animation();
        }
    }

    for (auto it = this->bricks.begin(); it != this->bricks.end();)
    {
        CBrick* brick = it->get();

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

void CPlayState::render_lasers()
{
    for (auto& laser : this->lasers)
    {
        this->gc->window->draw(*laser);
    }
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
    current_stage = next_stage_with_bricks(current_stage);

    this->balls.clear();
    this->bonus.clear();

    this->load_bricks();

    this->paddle->reset();

    this->spawn_ball();
}

void CPlayState::insert_bonus(CBrick* brick)
{
    // 10% chance of dropping a bonus at all.
    unsigned int percent = (std::rand() % 1000) + 1;

    if (percent > 100)
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
    else if (percent <= 13)
    { // Laser
        type = game::game_bonus::L;
    }
    else if (percent <= 25)
    { // Expand
        type = game::game_bonus::E;
    }
    else if (percent <= 37)
    { // Shrink
        type = game::game_bonus::R;
    }
    else if (percent <= 49)
    { // Sticky paddle
        type = game::game_bonus::C;
    }
    else if (percent <= 62)
    { // Multi-ball
        type = game::game_bonus::D;
    }
    else if (percent <= 75)
    { // Slow balls down
        type = game::game_bonus::S;
    }
    else if (percent <= 88)
    { // Speed up
        type = game::game_bonus::P;
    }
    else
    { // Spin paddle
        type = game::game_bonus::T;
    }

    auto b = std::make_unique<CBonus>(this, type);
    b->setPosition(brick->getPosition());

    this->bonus.push_back(std::move(b));
}

unsigned int CPlayState::get_current_stage()
{
    return current_stage;
}

unsigned int CPlayState::get_lives() const
{
    return this->lives;
}

game::game_bonus::bonus CPlayState::get_active_bonus() const
{
    return this->active_bonus;
}

std::list<std::unique_ptr<CBall>>& CPlayState::get_balls()
{
    return this->balls;
}

std::list<std::unique_ptr<CBrick>>& CPlayState::get_bricks()
{
    return this->bricks;
}

std::list<std::unique_ptr<CLaser>>& CPlayState::get_lasers()
{
    return this->lasers;
}

CPaddle* CPlayState::get_paddle()
{
    return this->paddle.get();
}
