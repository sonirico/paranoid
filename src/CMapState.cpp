#include "CMapState.hpp"

#include "CGameContainer.hpp"
#include "CResourceHolder.hpp"
#include "CStageStore.hpp"
#include "engine/Sprite.hpp"
#include "engine/Text.hpp"

#include <SDL3/SDL.h>

#include <fstream>

using namespace game::game_states;
using namespace game::game_bricks;

// The editor cursor. Draws the grid, the placed bricks and itself.
struct SSelector : public CEntity
{
    const float dt_animation = 0.2f;

    engine::Animation animation;

    engine::Vec2f ij; // Grid coordinates: map[i][j].

    // Held arrows repeat at this cadence.
    bool moved = false;
    float t_moved = 0;
    const float t_moved_value = .15f;

    int brick_type_map[LINES][BRICKS_PER_LINE];
    int current_brick_type = RED;

    CMapState* map_state;

    // Reused for every placed brick while drawing, hence mutable.
    mutable engine::Sprite wall;

    explicit SSelector(CMapState* st)
    {
        this->map_state = st;
        this->state = st;

        this->ij = engine::Vec2f{0, 0};

        this->animation.setSpriteSheet(st->rh->get(game::game_textures::MAIN));
        this->animation.addFrame({128, 56, 16, 8});
        this->animation.addFrame({128, 64, 16, 8});

        this->current_animation = &this->animation;

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

    // Stamps the current brick type into the cell under the cursor.
    void paint()
    {
        this->brick_type_map[static_cast<int>(this->ij.x)][static_cast<int>(this->ij.y)] =
            this->current_brick_type;
    }

    void erase()
    {
        this->brick_type_map[static_cast<int>(this->ij.x)][static_cast<int>(this->ij.y)] = NONE;
    }

    // Steps through the brick palette (NONE is what erase is for).
    void cycle_type(int delta)
    {
        const int first = static_cast<int>(RED);
        const int last = static_cast<int>(COUNT) - 1;
        const int span = last - first + 1;

        this->current_brick_type = first + (this->current_brick_type - first + delta + span) % span;
    }

    void move_selector(int dx, int dy)
    {
        if (this->moved)
        {
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

    const bool prev = keys[SDL_SCANCODE_Q];

    if (prev && !this->prev_type_was_down)
    {
        this->selector->cycle_type(-1);
    }
    this->prev_type_was_down = prev;

    const bool next = keys[SDL_SCANCODE_E];

    if (next && !this->next_type_was_down)
    {
        this->selector->cycle_type(1);
    }
    this->next_type_was_down = next;

    // Hold to paint while moving; arming on a release first keeps the
    // Enter press that opened the editor from stamping a brick.
    const bool paint = keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_SPACE];

    if (!paint)
    {
        this->paint_armed = true;
    }
    else if (this->paint_armed)
    {
        this->selector->paint();
    }

    if (keys[SDL_SCANCODE_BACKSPACE] || keys[SDL_SCANCODE_DELETE])
    {
        this->selector->erase();
    }
}

int CMapState::update(const float dt)
{
    if (this->back_requested)
    {
        return MENU;
    }

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

    // Same row-major ASCII format as the media/stages files, so a
    // finished map can ship as a built-in stage by copying the file.
    for (unsigned int y = 0; y < LINES; y++)
    {
        for (unsigned int x = 0; x < BRICKS_PER_LINE; x++)
        {
            file << CStageStore::char_from_cell(
                static_cast<unsigned int>(this->selector->brick_type_map[x][y]));
        }

        file << '\n';
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
    std::string line;

    for (unsigned int y = 0; y < LINES; y++)
    {
        if (!std::getline(file, line) || line.size() != BRICKS_PER_LINE)
        {
            this->show_status("LOAD FAILED");
            return;
        }

        for (unsigned int x = 0; x < BRICKS_PER_LINE; x++)
        {
            const int type = CStageStore::cell_from_char(line[x]);

            if (type < 0)
            {
                this->show_status("LOAD FAILED");
                return;
            }

            loaded[x][y] = type;
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
    this->bricks.clear();
}

void CMapState::render()
{
    this->render_bricks();

    this->gc->window->draw(*this->selector);

    this->render_palette_cursor();
    this->render_help();
    this->render_status();
}

void CMapState::render_palette_cursor()
{
    engine::Text cursor;
    cursor.setFont(this->gc->font);
    cursor.setString(">");
    cursor.setScale({2.f, 2.f});
    cursor.setColor(engine::Color::Yellow);
    cursor.setPosition(75.f, 100.f + 20.f * this->selector->current_brick_type);

    this->gc->window->draw(cursor);
}

void CMapState::render_help()
{
    engine::Text help;
    help.setFont(this->gc->font);
    help.setString("ARROWS MOVE  ENTER PAINT  DEL ERASE  Q/E TYPE  S/L SAVE/LOAD  ESC MENU");
    help.setScale({1.5f, 1.5f});

    help.setPosition((game::WIDTH - help.getGlobalBounds().width) / 2, game::HEIGHT - 30.f);

    this->gc->window->draw(help);
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

void CMapState::update_bricks(const float dt)
{
    for (auto& brick : this->bricks)
    {
        brick->update(dt);
    }
}

void CMapState::render_bricks()
{
    for (auto& brick : this->bricks)
    {
        this->gc->window->draw(*brick);
    }
}
