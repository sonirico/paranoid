#include "CMapState.hpp"

#include "CGameContainer.hpp"
#include "CResourceHolder.hpp"
#include "engine/Sprite.hpp"
#include "engine/Text.hpp"

#include <SDL3/SDL.h>

#include <fstream>
#include <iostream>

using namespace game::game_states;
using namespace game::game_bricks;

// The editor cursor. Draws the grid, the placed bricks and itself.
struct SSelector : public CEntity
{
    const float dt_animation = 0.2f;

    // Animation slots.
    static const unsigned int USUAL = 0;
    static const unsigned int SELECTED = 1;

    engine::Animation v_animation[2];

    engine::Vec2f ij; // Grid coordinates: map[i][j].

    bool moved = false;
    float t_moved = 0;
    const float t_moved_value = .2f;

    bool selected = false;

    bool printed = false;

    int brick_type_map[LINES][BRICKS_PER_LINE];
    int current_brick_type = 0; // NONE

    CMapState* map_state;

    // Reused for every placed brick while drawing, hence mutable.
    mutable engine::Sprite wall;

    explicit SSelector(CMapState* st)
    {
        this->map_state = st;
        this->state = st;

        this->ij = engine::Vec2f{0, 0};

        this->v_animation[USUAL].setSpriteSheet(st->rh->get(game::game_textures::MAIN));
        this->v_animation[SELECTED].setSpriteSheet(st->rh->get(game::game_textures::MAIN));

        this->v_animation[USUAL].addFrame({128, 48, 16, 8});

        this->v_animation[SELECTED].addFrame({128, 56, 16, 8});
        this->v_animation[SELECTED].addFrame({128, 64, 16, 8});

        this->current_animation = &this->v_animation[USUAL];

        this->animated_sprite = engine::AnimatedSprite(dt_animation, true, false);
        this->animated_sprite.play(*this->current_animation);

        this->setPosition(MARGIN_LEFT, MARGIN_TOP);
        this->scale(this->scalation);

        for (unsigned int i = 0; i < LINES; i++)
            for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
                this->brick_type_map[i][j] = 0;

        this->wall.setTexture(this->map_state->rh->get(game::game_textures::BRICKS));
        this->wall.setScale(this->scalation);
    }

    void init() override {}

    void update(const float dt) override
    {
        this->animated_sprite.update(dt);
        this->animated_sprite.play(*this->current_animation);

        this->t_moved += dt;

        if (this->t_moved >= this->t_moved_value)
        {
            this->t_moved = 0;
            this->moved = false;
        }
    }

    void draw(engine::Window& target) const override
    {
        // The grid lines.
        for (unsigned int i = 0; i <= LINES; ++i)
        {
            target.drawLine({static_cast<float>(MARGIN_LEFT), MARGIN_TOP + BRICK_BOUND.y * 2 * i},
                            {static_cast<float>(game::WIDTH - MARGIN_LEFT),
                             MARGIN_TOP + BRICK_BOUND.y * 2 * i});
        }

        for (unsigned int i = 0; i <= BRICKS_PER_LINE; ++i)
        {
            target.drawLine(
                {MARGIN_LEFT + BRICK_BOUND.x * 2 * i, static_cast<float>(MARGIN_TOP)},
                {MARGIN_LEFT + BRICK_BOUND.x * 2 * i, MARGIN_TOP + BRICK_BOUND.y * 2 * LINES});
        }

        // The placed bricks.
        for (unsigned int i = 0; i < LINES; i++)
        {
            for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
            {
                int t = this->brick_type_map[i][j];

                if (t != 0)
                {
                    this->wall.setTextureRect({static_cast<int>(BRICK_BOUND.x) * (t - 1), 0,
                                               static_cast<int>(BRICK_BOUND.x),
                                               static_cast<int>(BRICK_BOUND.y)});
                    this->wall.setPosition(MARGIN_LEFT + BRICK_BOUND.x * 2 * i,
                                           MARGIN_TOP + BRICK_BOUND.y * 2 * j);

                    target.draw(this->wall);
                }
            }
        }

        // The selector itself.
        CEntity::draw(target);
    }

    void reset() override {}

    void set_brick(int dy)
    {
        int limit = static_cast<int>(COUNT) - 1; // NONE also counts.

        if (this->current_brick_type + dy < 0)
            this->current_brick_type = limit;
        else if (this->current_brick_type + dy > limit)
            this->current_brick_type = 0;
        else
            this->current_brick_type += dy;

        unsigned int i = static_cast<int>(this->ij.x);
        unsigned int j = static_cast<int>(this->ij.y);

        this->brick_type_map[i][j] = this->current_brick_type;
    }

    // Dumps the edited map to stdout in brick_map[] literal format.
    void print_map()
    {
        if (this->printed)
        {
            return;
        }

        for (unsigned int i = 0; i < LINES; i++)
            for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
                std::cout << this->brick_type_map[i][j] << ",";

        this->printed = true;
    }

    void move_selector(int dx, int dy)
    {
        if (this->moved)
        {
            return;
        }
        if (this->selected)
        {
            this->set_brick(dy);
            this->moved = true;
            this->t_moved = 0;
            return;
        }

        if (ij.x + dx < 0)
            ij.x = BRICKS_PER_LINE - 1;
        else if (ij.x + dx == BRICKS_PER_LINE)
            ij.x = 0;
        else
            ij.x += dx;

        if (ij.y + dy < 0)
            ij.y = LINES - 1;
        else if (ij.y + dy == LINES)
            ij.y = 0;
        else
            ij.y += dy;

        this->setPosition(MARGIN_LEFT + (ij.x * BRICK_BOUND.x * 2),
                          MARGIN_TOP + (ij.y * BRICK_BOUND.y * 2));
        this->moved = true;
        this->t_moved = 0;
    }

    void change_animation()
    {
        if (this->selected)
            this->current_animation = &this->v_animation[SELECTED];
        else
            this->current_animation = &this->v_animation[USUAL];
    }

    void set_selected(bool b)
    {
        this->selected = b;

        this->change_animation();
    }
};

CMapState::CMapState(CGameContainer* gc)
{
    this->gc = gc;
    this->rh = gc->rh;
}

CMapState::~CMapState() = default;

void CMapState::init()
{
    this->selector = std::make_unique<SSelector>(this);

    this->paddle = std::make_unique<CPaddle>(this);

    // A sample column with every brick type, for reference while editing.
    for (int i = 1; i <= 11; ++i)
    {
        auto b = std::make_unique<CBrick>(this, static_cast<game::game_bricks::bricks>(i));
        b->setPosition(100, 100 + 20 * i);
        this->bricks.push_back(std::move(b));
    }
}

void CMapState::events()
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    const bool esc = keys[SDL_SCANCODE_ESCAPE];

    if (esc && !this->esc_was_down)
    {
        this->back_requested = true;
    }
    this->esc_was_down = esc;

    const bool save = keys[SDL_SCANCODE_S];

    if (save && !this->save_was_down)
    {
        this->save_map();
    }
    this->save_was_down = save;

    const bool load = keys[SDL_SCANCODE_L];

    if (load && !this->load_was_down)
    {
        this->load_map();
    }
    this->load_was_down = load;

    if (keys[SDL_SCANCODE_SPACE])
    {
        this->selector->print_map();
    }

    if (keys[SDL_SCANCODE_LEFT])
    {
        this->selector->move_selector(-1, 0);
    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        this->selector->move_selector(1, 0);
    }
    if (keys[SDL_SCANCODE_UP])
    {
        this->selector->move_selector(0, -1);
    }
    if (keys[SDL_SCANCODE_DOWN])
    {
        this->selector->move_selector(0, 1);
    }
    if (keys[SDL_SCANCODE_RETURN])
    {
        this->selector->set_selected(true);
    }
    if (keys[SDL_SCANCODE_BACKSPACE])
    {
        this->selector->set_selected(false);
    }
}

int CMapState::update(const float dt)
{
    if (this->back_requested)
    {
        return MENU;
    }

    this->update_paddle(dt);
    this->update_balls(dt);
    this->update_bricks(dt);

    this->selector->update(dt);

    if (this->status_time > 0)
    {
        this->status_time -= dt;
    }

    return NULLSTATE;
}

void CMapState::save_map()
{
    if (this->gc->data_dir.empty())
    {
        return;
    }

    std::ofstream file(this->gc->data_dir + "custom.map");

    if (!file)
    {
        this->show_status("SAVE FAILED");
        return;
    }

    for (unsigned int i = 0; i < LINES; i++)
    {
        for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
        {
            file << this->selector->brick_type_map[i][j] << ' ';
        }
    }

    this->show_status("SAVED");
}

void CMapState::load_map()
{
    if (this->gc->data_dir.empty())
    {
        return;
    }

    std::ifstream file(this->gc->data_dir + "custom.map");

    if (!file)
    {
        this->show_status("NO SAVED MAP");
        return;
    }

    int loaded[LINES][BRICKS_PER_LINE];

    for (unsigned int i = 0; i < LINES; i++)
    {
        for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
        {
            int type = 0;

            if (!(file >> type) || type < 0 || type >= static_cast<int>(COUNT))
            {
                this->show_status("LOAD FAILED");
                return;
            }

            loaded[i][j] = type;
        }
    }

    for (unsigned int i = 0; i < LINES; i++)
    {
        for (unsigned int j = 0; j < BRICKS_PER_LINE; j++)
        {
            this->selector->brick_type_map[i][j] = loaded[i][j];
        }
    }

    this->show_status("LOADED");
}

void CMapState::show_status(const std::string& text)
{
    this->status = text;
    this->status_time = 2.f;
}

void CMapState::clear()
{
    this->balls.clear();
    this->bricks.clear();
}

void CMapState::render()
{
    this->render_balls();
    this->render_paddle();
    this->render_bricks();

    this->gc->window->draw(*this->selector);

    this->render_status();
}

void CMapState::render_status()
{
    if (this->status_time <= 0)
    {
        return;
    }

    engine::Text text;
    text.setFont(this->gc->font);
    text.setString(this->status);
    text.setScale({2.f, 2.f});
    text.setColor(engine::Color::Yellow);
    text.setPosition((game::WIDTH - text.getGlobalBounds().width) / 2, 40.f);

    this->gc->window->draw(text);
}

void CMapState::update_balls(const float dt)
{
    for (auto& ball : this->balls)
    {
        ball->update(dt);
    }
}

void CMapState::update_bricks(const float dt)
{
    for (auto& brick : this->bricks)
    {
        brick->update(dt);
    }
}

void CMapState::update_paddle(const float dt)
{
    this->paddle->update(dt);
}

void CMapState::render_balls()
{
    for (auto& ball : this->balls)
    {
        this->gc->window->draw(*ball);
    }
}

void CMapState::render_bricks()
{
    for (auto& brick : this->bricks)
    {
        this->gc->window->draw(*brick);
    }
}

void CMapState::render_paddle()
{
    this->gc->window->draw(*(this->paddle));
}
