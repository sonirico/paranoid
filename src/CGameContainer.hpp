#pragma once

#include "CResourceHolder.hpp"
#include "assets.h"
#include "engine/Sound.hpp"
#include "engine/Window.hpp"

namespace engine
{
class AudioDevice;
}

// Shared services every state needs: the window, the loaded resources
// and the sound-effect voices. Owns none of its dependencies.
class CGameContainer
{
  public:
    CGameContainer(engine::Window* window, engine::AudioDevice* audio, CResourceHolder* rh);

    CGameContainer(const CGameContainer&) = delete;
    CGameContainer& operator=(const CGameContainer&) = delete;

    // Drains the OS event queue; closes the window on quit or Escape.
    void events();

    void play_fx(game::game_fx::fx id);

  public:
    engine::Window* window;
    CResourceHolder* rh;

  private:
    // One dedicated voice per effect, indexed by its fx id.
    engine::Sound current_sound[game::game_fx::COUNT];
};
