#pragma once

#include "engine/Vec2.hpp"

#include <string>
#include <vector>

class CGameContainer;

// A vertical list of selectable entries driven by keyboard (arrows +
// Return) and mouse (hover + click), used by the menus.
class CMenu
{
  public:
    // Entries stack downwards, each centered on center_x from top_y.
    CMenu(CGameContainer* gc, std::vector<std::string> entries, float center_x, float top_y);

    // Processes navigation input; returns the activated entry index,
    // or -1 while nothing is chosen.
    int update();

    void render();

    int get_selected() const;

    // Rewrites an entry's label, e.g. to reflect a toggled setting.
    void set_entry(unsigned int i, std::string entry);

  private:
    void move_selection(int delta);

    // The entry under the given point, or -1.
    int entry_at(float x, float y) const;

    float entry_width(unsigned int i) const;
    float entry_left(unsigned int i) const;
    float entry_top(unsigned int i) const;

    static constexpr float SCALE = 3.f;
    static constexpr float SPACING = 48.f;

    CGameContainer* gc;

    std::vector<std::string> entries;
    int selected = 0;

    float center_x;
    float top_y;

    bool up_was_down = false;
    bool down_was_down = false;
    bool enter_was_down = false;
    bool click_was_down = false;

    // Hover only retargets the selection while the mouse moves, so it
    // cannot fight the keyboard.
    engine::Vec2f last_mouse{-1.f, -1.f};
};
