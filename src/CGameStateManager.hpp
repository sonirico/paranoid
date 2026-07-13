#pragma once

#include "CState.hpp"

#include <memory>

class CGameContainer;

// Owns the active state and drives its event/update/render cycle.
class CGameStateManager
{
  public:
    CGameStateManager(CGameContainer* gc, int initial_state);

    void update(const float dt);
    void render();

    CState* getCurrentState() const;

  private:
    void set_state(int state_id);

  private:
    CGameContainer* gc;

    std::unique_ptr<CState> current_state;
};
