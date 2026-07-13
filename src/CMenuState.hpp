#pragma once

#include "CMenu.hpp"
#include "CState.hpp"

#include <memory>

class CGameContainer;

// The main menu screen: Play / Options / Quit.
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
    std::unique_ptr<CMenu> menu;

    // Options has nothing to configure yet; selecting it says so.
    bool coming_soon = false;
};
