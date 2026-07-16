#include "CPlayState.hpp"

#include "CGameContainer.hpp"
#include "CStageStore.hpp"
#include "engine/Gamepad.hpp"
#include "engine/Text.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <limits>
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
    this->score = 0;

    this->load_high_score();

    this->paddle = std::make_unique<CPaddle>(this);

    this->pause_menu =
        std::make_unique<CMenu>(this->gc, std::vector<std::string>{"RESUME", "MAIN MENU", "QUIT"},
                                game::WIDTH / 2.f, 280.f);

    this->load_bricks();

    this->spawn_ball();

    this->enter_intro(true);
}

void CPlayState::play_stage_music()
{
    // Each stage picks the next background from the rotation.
    const char* tracks[] = {
        "media/music/crystalhammer.ogg",  "media/music/arkanoid.ogg",
        "media/music/amiga_euphorie.ogg", "media/music/bst_7str.ogg",
        "media/music/iyarms.ogg",         "media/music/mangotetris.ogg",
        "media/music/tetrisduel.ogg",     "media/music/tetrismusicb.ogg",
    };

    this->gc->play_music(tracks[current_stage % std::size(tracks)], true);
}

void CPlayState::enter_intro(bool show_round)
{
    this->phase = Phase::Intro;
    this->intro_shows_round = show_round;
    this->hitstop_time = 0;

    if (show_round)
    {
        // Stage starts open on the round jingle, played once; the card
        // yields to gameplay just before the jingle's quiet tail, so the
        // wait stays snappy. Without a jingle (tests) use the fixed
        // fallback.
        this->gc->play_music("media/music/stage.ogg", false);

        const float jingle = this->gc->get_music_duration();

        this->phase_time = jingle > 0 ? std::max(jingle - .75f, 1.f) : ROUND_INTRO_DURATION;
    }
    else
    {
        // Respawns show a short READY and keep the music running.
        this->phase_time = READY_DURATION;
    }
}

void CPlayState::events()
{
    if (this->phase != Phase::Playing)
    {
        return;
    }

    const bool* keys = SDL_GetKeyboardState(nullptr);

    const engine::Gamepad* pad = this->gc->gamepad;
    const bool pad_ok = pad != nullptr && pad->isConnected();

    // Start on the pad pauses like Escape.
    const bool esc =
        keys[SDL_SCANCODE_ESCAPE] || (pad_ok && pad->isButtonDown(engine::Gamepad::Button::Start));

    if (esc && !this->esc_was_down)
    {
        this->paused = !this->paused;
    }
    this->esc_was_down = esc;

    if (this->paused)
    {
        return;
    }

    // Left click and the pad's south button mirror Space: release the
    // sticky ball, fire the laser.
    const bool fire = keys[SDL_SCANCODE_SPACE] ||
                      (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) ||
                      (pad_ok && pad->isButtonDown(engine::Gamepad::Button::South));

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

#ifdef DEV_MODE
    if (keys[SDL_SCANCODE_A])
    {
        // Developer cheat: spawn an extra ball on the paddle.
        this->spawn_ball();
    }
#endif
}

int CPlayState::update(const float dt)
{
    // The shake and the paddle explosion decay in any phase: they
    // start on the very tick a card (READY, GAME OVER) freezes the game.
    if (this->shake_time > 0)
    {
        this->shake_time -= dt;
    }
    if (this->paddle_death_time > 0)
    {
        this->paddle_death_time -= dt;
    }

    if (this->phase == Phase::Intro)
    {
        this->phase_time -= dt;

        if (this->phase_time <= 0)
        {
            this->phase = Phase::Playing;

            // The jingle is over: bring in the background track.
            this->play_stage_music();
        }

        return NULLSTATE;
    }

    if (this->phase == Phase::GameOver)
    {
        this->phase_time -= dt;

        return this->phase_time <= 0 ? MENU : NULLSTATE;
    }

    if (this->paused)
    {
        switch (this->pause_menu->update())
        {
        case 0:
            this->paused = false;
            break;
        case 1:
            return MENU;
        case 2:
            this->gc->window->close();
            break;
        }

        return NULLSTATE;
    }

    if (this->hitstop_time > 0)
    {
        this->hitstop_time -= dt;

        return NULLSTATE;
    }

    this->snapshot_entities();

    this->update_paddle(dt);
    this->update_balls(dt);
    this->update_bonus(dt);

    if (this->break_pending)
    {
        this->break_pending = false;
        this->next_stage();

        return NULLSTATE;
    }

    this->update_lasers(dt);
    this->update_active_bonus(dt);
    this->update_particles(dt);

    if (this->update_bricks(dt))
    {
        this->next_stage();
    }

    return NULLSTATE;
}

void CPlayState::spawn_brick_particles(CBrick* brick)
{
    const engine::Vec2f center = brick->getPosition() + brick->get_size() * 0.5f;
    const engine::Color color = brick->get_color();

    for (unsigned int i = 0; i < PARTICLES_PER_BRICK; ++i)
    {
        const float angle = (std::rand() % 360) * 3.14159265f / 180.f;
        const float speed = 60.f + std::rand() % 140;

        Particle p;
        p.pos = center;
        p.vel = {std::cos(angle) * speed, std::sin(angle) * speed - 60.f};
        p.life = 0.4f + (std::rand() % 30) / 100.f;
        p.color = color;

        this->particles.push_back(p);
    }
}

void CPlayState::spawn_impact_sparks(const engine::Vec2f& center, const engine::Color& color)
{
    for (unsigned int i = 0; i < SPARKS_PER_IMPACT; ++i)
    {
        const float angle = (std::rand() % 360) * 3.14159265f / 180.f;
        const float speed = 40.f + std::rand() % 80;

        Particle p;
        p.pos = center;
        p.vel = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.life = 0.15f + (std::rand() % 15) / 100.f;
        p.color = color;

        this->particles.push_back(p);
    }
}

void CPlayState::update_particles(const float dt)
{
    for (auto it = this->particles.begin(); it != this->particles.end();)
    {
        it->life -= dt;

        if (it->life <= 0)
        {
            it = this->particles.erase(it);
            continue;
        }

        it->vel.y += PARTICLE_GRAVITY * dt;
        it->pos += it->vel * dt;

        ++it;
    }
}

void CPlayState::render_particles()
{
    for (const Particle& p : this->particles)
    {
        engine::Color color = p.color;

        // Fade out over the particle's lifetime.
        color.a = static_cast<std::uint8_t>(std::min(1.f, p.life / 0.4f) * 255);

        this->gc->window->drawRect(p.pos, {3.f, 3.f}, color);
    }
}

void CPlayState::start_shake(float duration, float strength)
{
    this->shake_time = duration;
    this->shake_strength = strength;
}

std::size_t CPlayState::get_particle_count() const
{
    return this->particles.size();
}

void CPlayState::render()
{
    if (this->shake_time > 0)
    {
        const engine::Vec2f offset{(std::rand() % 200 - 100) / 100.f * this->shake_strength,
                                   (std::rand() % 200 - 100) / 100.f * this->shake_strength};

        this->gc->window->setViewOffset(offset);
    }

    this->render_paddle();
    this->render_bricks();
    this->render_balls();
    this->render_bonus();
    this->render_particles();
    this->render_lasers();
    this->render_lives();
    this->render_active_bonus();
    this->render_score();

    // The net barrier (bonus N) shows as a bright band on the floor.
    if (this->active_bonus == game::game_bonus::N)
    {
        const engine::Color cyan{0, 255, 255, 255};

        for (int i = 1; i <= 3; ++i)
        {
            this->gc->window->drawLine(
                {0.f, game::HEIGHT - static_cast<float>(i)},
                {static_cast<float>(game::WIDTH), game::HEIGHT - static_cast<float>(i)}, cyan);
        }
    }

    this->render_phase_cards();

    if (this->paused)
    {
        this->render_pause_menu();
    }
}

void CPlayState::render_phase_cards()
{
    // Centers the line horizontally at the given height.
    const auto draw_centered =
        [this](const std::string& string, float y, float scale, const engine::Color& color)
    {
        engine::Text text;
        text.setFont(this->gc->font);
        text.setString(string);
        text.setScale({scale, scale});
        text.setColor(color);
        text.setPosition((game::WIDTH - text.getGlobalBounds().width) / 2, y);

        this->gc->window->draw(text);
    };

    if (this->phase == Phase::Intro)
    {
        if (this->intro_shows_round)
        {
            draw_centered("ROUND " + std::to_string(current_stage + 1), 260.f, 3.f,
                          engine::Color::White);
        }

        draw_centered("READY", 320.f, 3.f, engine::Color::Yellow);
    }
    else if (this->phase == Phase::GameOver)
    {
        draw_centered("GAME OVER", 280.f, 5.f, engine::Color::Red);
    }
}

void CPlayState::render_pause_menu()
{
    engine::Text title;
    title.setFont(this->gc->font);
    title.setString("PAUSED");
    title.setScale({4.f, 4.f});

    const engine::FloatRect bounds = title.getGlobalBounds();
    title.setPosition((game::WIDTH - bounds.width) / 2, 180.f);

    this->gc->window->draw(title);

    this->pause_menu->render();
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

void CPlayState::render_score()
{
    char buffer[32];

    std::snprintf(buffer, sizeof(buffer), "SCORE %06u", this->score);

    engine::Text score_text;
    score_text.setFont(this->gc->font);
    score_text.setString(buffer);
    score_text.setScale({2.f, 2.f});
    score_text.setPosition(10.f, 10.f);

    this->gc->window->draw(score_text);

    std::snprintf(buffer, sizeof(buffer), "HIGH %06u", this->high_score);

    engine::Text high_text;
    high_text.setFont(this->gc->font);
    high_text.setString(buffer);
    high_text.setScale({2.f, 2.f});
    high_text.setColor(engine::Color::Yellow);
    high_text.setPosition(game::WIDTH - 10.f - high_text.getGlobalBounds().width, 10.f);

    this->gc->window->draw(high_text);
}

void CPlayState::load_high_score()
{
    this->high_score = 0;

    if (this->gc->data_dir.empty())
    {
        return;
    }

    std::ifstream file(this->gc->data_dir + "highscore");

    if (file)
    {
        file >> this->high_score;
    }
}

void CPlayState::save_high_score()
{
    if (this->gc->data_dir.empty())
    {
        return;
    }

    std::ofstream file(this->gc->data_dir + "highscore");

    if (file)
    {
        file << this->high_score;
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
    case M:
        name = "MEGA";
        break;
    case N:
        name = "NET";
        break;
    case S:
        name = "SLOW";
        break;
    case P:
        name = "FAST";
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
    engine::Text label;
    label.setFont(this->gc->font);
    label.setString(name);
    label.setScale({2.f, 2.f});

    const float y = game::HEIGHT - 18.f;
    const float x_text = game::WIDTH - 10.f - label.getGlobalBounds().width;

    label.setPosition(x_text, y);

    engine::Sprite icon;
    icon.setTexture(this->rh->get(game::game_textures::MAIN));
    icon.setTextureRect({176 + 16 * static_cast<int>(this->active_bonus), 0, 16, 8});
    icon.setScale({2.f, 2.f});
    icon.setPosition(x_text - 42.f, y);

    this->gc->window->draw(icon);
    this->gc->window->draw(label);
}

void CPlayState::clear()
{
    this->save_high_score();

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
            const bool flying = !ball->is_in_paddle();
            const engine::Vec2f v_before = ball->get_velocity();

            ball->update(dt);

            // A flipped velocity axis means the ball just bounced off a
            // wall or the paddle: throw sparks at the impact point.
            const engine::Vec2f v_after = ball->get_velocity();

            if (flying && !ball->is_removable() &&
                (v_before.x * v_after.x < 0 || v_before.y * v_after.y < 0))
            {
                this->spawn_impact_sparks(ball->getPosition() + ball->get_size() * 0.5f,
                                          {220, 220, 255, 255});
            }

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

    // Death drops the mode capsule and its countdown, like in the
    // arcade original.
    this->cancel_active_bonus();

    this->gc->play_fx(game::game_fx::MUERTE);

    this->start_paddle_death();

    if (this->lives == 0)
    {
        // Game over: show the card in silence, then update() returns
        // to the menu, whose theme takes over.
        this->save_high_score();
        this->gc->stop_music();

        this->phase = Phase::GameOver;
        this->phase_time = GAME_OVER_DURATION;

        this->start_shake(0.5f, 10.f);

        return;
    }

    this->spawn_ball();

    this->enter_intro(false);

    this->start_shake(0.35f, 6.f);
}

void CPlayState::spawn_ball()
{
    auto b = std::make_unique<CBall>(this, this->paddle.get());
    b->set_in_paddle();

    // New balls join whatever ball mode is in effect.
    b->set_pierce(this->active_bonus == game::game_bonus::M);
    b->set_net(this->active_bonus == game::game_bonus::N);

    if (this->ball_speed_factor != 1.f)
    {
        b->scale_velocity(this->ball_speed_factor);
    }

    b->snapshot();

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
    this->cancel_active_bonus();

    game::game_fx::fx pickup_sound = game::game_fx::OPTION;

    if (type == X)
    {
        pickup_sound = game::game_fx::LIFEUP;
    }
    else if (type == C)
    {
        pickup_sound = game::game_fx::STICKY;
    }

    this->gc->play_fx(pickup_sound);

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

        // Split the flying ball nearest the paddle: with several in
        // play it is the one the player is actually tracking.
        CBall* source = nullptr;
        float best_distance = std::numeric_limits<float>::max();

        const engine::Vec2f paddle_center =
            this->paddle->getPosition() + this->paddle->get_size() * 0.5f;

        for (auto& ball : this->balls)
        {
            if (ball->is_in_paddle())
            {
                continue;
            }

            const float distance = (ball->getPosition() - paddle_center).length();

            if (distance < best_distance)
            {
                best_distance = distance;
                source = ball.get();
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

            // Seed the render interpolation so the twin's first frame
            // draws exactly where it was born.
            twin->snapshot();

            this->balls.push_back(std::move(twin));
        }
        break;
    }
    case S:
        this->ball_speed_factor = 0.7f;
        for (auto& ball : this->balls)
        {
            ball->scale_velocity(this->ball_speed_factor);
        }
        this->arm_active_bonus(type);
        break;
    case P:
        this->ball_speed_factor = 1.3f;
        for (auto& ball : this->balls)
        {
            ball->scale_velocity(this->ball_speed_factor);
        }
        this->arm_active_bonus(type);
        break;
    case X:
        this->lives++;
        break;
    case B:
        // Break: jump to the next stage as soon as this pass ends.
        this->break_pending = true;
        break;
    case M:
        for (auto& ball : this->balls)
        {
            ball->set_pierce(true);
        }
        this->arm_active_bonus(type);
        break;
    case N:
        for (auto& ball : this->balls)
        {
            ball->set_net(true);
        }
        this->arm_active_bonus(type);
        break;
    default:
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
        this->cancel_active_bonus();
    }
}

void CPlayState::cancel_active_bonus()
{
    this->paddle->reset_modes();

    for (auto& ball : this->balls)
    {
        ball->set_pierce(false);
        ball->set_net(false);

        // Undo a slow/fast capsule: this game stays arcade-quick.
        if (this->ball_speed_factor != 1.f)
        {
            ball->scale_velocity(1.f / this->ball_speed_factor);
        }
    }

    this->ball_speed_factor = 1.f;

    this->active_bonus = game::game_bonus::COUNT;
    this->bonus_time_left = 0;
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

            // A brick that shrugs the hit off still sparks in its own
            // color, so chipping silver/gold and bouncing off the
            // indestructible ones reads at a glance.
            if (!brick->is_removable())
            {
                this->spawn_impact_sparks(brick->getPosition() + brick->get_size() * 0.5f,
                                          brick->get_color());
            }
        }
    }

    unsigned int killed = 0;

    for (auto it = this->bricks.begin(); it != this->bricks.end();)
    {
        CBrick* brick = it->get();

        if (brick->is_removable())
        {
            this->score += brick->get_score();
            this->high_score = std::max(this->high_score, this->score);

            this->spawn_brick_particles(brick);
            this->insert_bonus(brick);
            it = this->bricks.erase(it);

            this->total_bricks--;
            killed++;

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

    if (killed > 0)
    {
        // Freeze the action for a blink; snapshotting right away pins
        // the render interpolation so nothing shivers while frozen.
        this->hitstop_time = HITSTOP_DURATION;
        this->snapshot_entities();
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
    if (this->paddle_death_time > 0)
    {
        // The explosion frames are 32x16: twice the paddle's height,
        // bottom-aligned over where it stood.
        const unsigned int frame =
            std::min(PADDLE_DEATH_FRAMES - 1,
                     static_cast<unsigned int>((PADDLE_DEATH_DURATION - this->paddle_death_time) /
                                               PADDLE_DEATH_DURATION * PADDLE_DEATH_FRAMES));

        engine::Sprite explosion;
        explosion.setTexture(this->rh->get(game::game_textures::MAIN));
        explosion.setTextureRect({128 + 32 * static_cast<int>(frame), 224, 32, 16});
        explosion.setScale({2.f, 2.f});
        explosion.setPosition(this->paddle_death_pos.x, this->paddle_death_pos.y - 16.f);

        this->gc->window->draw(explosion);

        return;
    }

    this->gc->window->draw(*(this->paddle));
}

void CPlayState::start_paddle_death()
{
    this->paddle_death_time = PADDLE_DEATH_DURATION;
    this->paddle_death_pos = this->paddle->getPosition();
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

    const std::vector<unsigned int>* current_map = this->gc->stages->get(current_stage);

    if (current_map == nullptr)
    {
        return;
    }

    this->total_bricks = 0;

    // Stage cells are row-major: index i sits at grid (i % 15, i / 15).
    for (unsigned int i = 0; i < (BRICKS_PER_LINE * LINES); ++i)
    {
        game::game_bricks::bricks type = static_cast<game::game_bricks::bricks>((*current_map)[i]);

        if (type != NONE)
        {
            auto b = std::make_unique<CBrick>(this, type);

            unsigned int x = MARGIN_LEFT + BRICK_BOUND.x * 2 * (i % BRICKS_PER_LINE);
            unsigned int y = MARGIN_TOP + BRICK_BOUND.y * 2 * (i / BRICKS_PER_LINE);

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
    current_stage = this->gc->stages->next_stage_with_bricks(current_stage);

    this->balls.clear();
    this->bonus.clear();

    // A new stage starts clean: no leftover mode or stale countdown.
    this->cancel_active_bonus();

    this->load_bricks();

    this->paddle->reset();

    this->spawn_ball();

    this->enter_intro(true);
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

    if (percent <= 4)
    { // Extra life
        type = game::game_bonus::X;
    }
    else if (percent <= 8)
    { // Break: skip to the next stage
        type = game::game_bonus::B;
    }
    else if (percent <= 15)
    { // Laser
        type = game::game_bonus::L;
    }
    else if (percent <= 25)
    { // Expand
        type = game::game_bonus::E;
    }
    else if (percent <= 35)
    { // Shrink
        type = game::game_bonus::R;
    }
    else if (percent <= 45)
    { // Sticky paddle
        type = game::game_bonus::C;
    }
    else if (percent <= 55)
    { // Multi-ball
        type = game::game_bonus::D;
    }
    else if (percent <= 65)
    { // Slow balls down
        type = game::game_bonus::S;
    }
    else if (percent <= 75)
    { // Speed up
        type = game::game_bonus::P;
    }
    else if (percent <= 83)
    { // Megaball
        type = game::game_bonus::M;
    }
    else if (percent <= 91)
    { // Net barrier
        type = game::game_bonus::N;
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

unsigned int CPlayState::get_score() const
{
    return this->score;
}

unsigned int CPlayState::get_high_score() const
{
    return this->high_score;
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
