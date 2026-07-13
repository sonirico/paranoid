#pragma once

#include "CResourceHolder.hpp"
#include "assets.h"
#include "engine/BitmapFont.hpp"
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

    // Drains the OS event queue; closes the window on quit.
    void events();

    void play_fx(game::game_fx::fx id);

  public:
    engine::Window* window;
    CResourceHolder* rh;

    // The game's bitmap font, shared by every state that renders text.
    engine::BitmapFont font;

    // Writable per-user directory (trailing separator included) for
    // saves like the high score; empty disables persistence.
    std::string data_dir;

  private:
    // One dedicated voice per effect, indexed by its fx id.
    engine::Sound current_sound[game::game_fx::COUNT];
};
