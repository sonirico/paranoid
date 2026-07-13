#pragma once

#include "CBall.hpp"
#include "CBrick.hpp"
#include "CPaddle.hpp"
#include "CState.hpp"
#include "assets.h"

#include <list>
#include <memory>
#include <string>

class CGameContainer;
struct SSelector;

// The stage editor: move a selector over the grid, place brick types,
// save/load the map on disk (S/L) or dump it to stdout (Space).
class CMapState : public CState
{
  public:
    explicit CMapState(CGameContainer* gc);
    ~CMapState() override;

    void init() override;
    void events() override;
    int update(const float dt) override;
    void render() override;
    void clear() override;

  private:
    void update_balls(const float dt);
    void update_bricks(const float dt);
    void update_paddle(const float dt);

    void render_balls();
    void render_bricks();
    void render_paddle();
    void render_status();

    // The edited map persists to <data_dir>/custom.map.
    void save_map();
    void load_map();

    // Flashes a short confirmation ("SAVED", "LOADED") over the grid.
    void show_status(const std::string& text);

  private:
    std::unique_ptr<CPaddle> paddle;

    std::list<std::unique_ptr<CBall>> balls;
    std::list<std::unique_ptr<CBrick>> bricks;

    std::unique_ptr<SSelector> selector;

    std::string status;
    float status_time = 0;

    bool esc_was_down = false;
    bool save_was_down = false;
    bool load_was_down = false;

    // Escape asks to leave; update() returns to the menu.
    bool back_requested = false;
};
