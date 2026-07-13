#include "CMenu.hpp"

#include "CGameContainer.hpp"
#include "engine/Text.hpp"

#include <SDL3/SDL.h>

#include <utility>

CMenu::CMenu(CGameContainer* gc, std::vector<std::string> entries, float center_x, float top_y)
    : gc(gc), entries(std::move(entries)), center_x(center_x), top_y(top_y)
{
}

int CMenu::update()
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    const bool up = keys[SDL_SCANCODE_UP];
    const bool down = keys[SDL_SCANCODE_DOWN];

    if (up && !this->up_was_down)
    {
        this->move_selection(-1);
    }
    if (down && !this->down_was_down)
    {
        this->move_selection(1);
    }

    this->up_was_down = up;
    this->down_was_down = down;

    // In game coordinates, so fullscreen and letterbox don't skew it.
    const engine::Vec2f mouse = this->gc->window->getMousePosition();
    const bool click = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK;

    if (mouse.x != this->last_mouse.x || mouse.y != this->last_mouse.y)
    {
        this->last_mouse = mouse;

        const int hovered = this->entry_at(mouse.x, mouse.y);

        if (hovered >= 0 && hovered != this->selected)
        {
            this->selected = hovered;
            this->gc->play_fx(game::game_fx::SELECT);
        }
    }

    int activated = -1;

    // Return rather than Space activates, so closing a pause menu does
    // not also release a sticky ball on the same key.
    const bool enter = keys[SDL_SCANCODE_RETURN];

    if (enter && !this->enter_was_down)
    {
        activated = this->selected;
    }
    this->enter_was_down = enter;

    if (click && !this->click_was_down)
    {
        const int under = this->entry_at(mouse.x, mouse.y);

        if (under >= 0)
        {
            this->selected = under;
            activated = under;
        }
    }
    this->click_was_down = click;

    if (activated >= 0)
    {
        this->gc->play_fx(game::game_fx::SELECT);
    }

    return activated;
}

void CMenu::render()
{
    for (unsigned int i = 0; i < this->entries.size(); ++i)
    {
        const bool is_selected = static_cast<int>(i) == this->selected;

        engine::Text text;
        text.setFont(this->gc->font);
        text.setString(this->entries[i]);
        text.setScale({SCALE, SCALE});
        text.setPosition(this->entry_left(i), this->entry_top(i));
        text.setColor(is_selected ? engine::Color::Yellow : engine::Color::White);

        this->gc->window->draw(text);

        if (is_selected)
        {
            engine::Text cursor;
            cursor.setFont(this->gc->font);
            cursor.setString(">");
            cursor.setScale({SCALE, SCALE});
            cursor.setPosition(this->entry_left(i) - 2 * 8.f * SCALE, this->entry_top(i));
            cursor.setColor(engine::Color::Yellow);

            this->gc->window->draw(cursor);
        }
    }
}

int CMenu::get_selected() const
{
    return this->selected;
}

void CMenu::set_entry(unsigned int i, std::string entry)
{
    this->entries[i] = std::move(entry);
}

void CMenu::move_selection(int delta)
{
    const int count = static_cast<int>(this->entries.size());

    this->selected = (this->selected + delta + count) % count;

    this->gc->play_fx(game::game_fx::SELECT);
}

int CMenu::entry_at(float x, float y) const
{
    for (unsigned int i = 0; i < this->entries.size(); ++i)
    {
        const float left = this->entry_left(i);
        const float top = this->entry_top(i);

        if (x >= left && x <= left + this->entry_width(i) && y >= top && y <= top + 8.f * SCALE)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

float CMenu::entry_width(unsigned int i) const
{
    return this->entries[i].size() * 8.f * SCALE;
}

float CMenu::entry_left(unsigned int i) const
{
    return this->center_x - this->entry_width(i) / 2;
}

float CMenu::entry_top(unsigned int i) const
{
    return this->top_y + i * SPACING;
}
