#pragma once

#include "CMenu.hpp"
#include "CState.hpp"

#include <memory>

class CGameContainer;

// The main menu screen: Play / Options / Quit, with the options page
// (scale mode, fullscreen) one level in.
class CMenuState : public CState
{
  public:
    explicit CMenuState(CGameContainer* gc);

    void init() override;
    void events() override;
    int update(const float dt) override;
    void render() override;
    void clear() override;

  private:
    // Rewrites the options entries from the window's current settings.
    void refresh_options();

    std::unique_ptr<CMenu> menu;
    std::unique_ptr<CMenu> options_menu;

    bool in_options = false;
    bool esc_was_down = false;
};
