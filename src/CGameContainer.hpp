#pragma once

#include "CResourceHolder.hpp"
#include "assets.h"
#include "engine/BitmapFont.hpp"
#include "engine/Sound.hpp"
#include "engine/Window.hpp"

namespace engine
{
class AudioDevice;
class Gamepad;
class Music;
} // namespace engine

// Shared services every state needs: the window, the loaded resources
// and the sound-effect voices. Owns none of its dependencies.
class CStageStore;
class CStarfield;

class CGameContainer
{
  public:
    CGameContainer(engine::Window* window, engine::AudioDevice* audio, CResourceHolder* rh,
                   engine::Music* music);

    CGameContainer(const CGameContainer&) = delete;
    CGameContainer& operator=(const CGameContainer&) = delete;

    // Drains the OS event queue; closes the window on quit.
    void events();

    void play_fx(game::game_fx::fx id, float pitch = 1.f);

    // Switches the background track; asking for the track that is
    // already playing is a no-op. Non-looping tracks play once (jingles).
    void play_music(const std::string& path, bool looping);
    void stop_music();

    // Length in seconds of the last loaded track; 0 when none.
    float get_music_duration() const;

    // Volumes in the 0..100 range; setters clamp, apply and persist.
    float get_music_volume() const;
    void set_music_volume(float volume);
    float get_fx_volume() const;
    void set_fx_volume(float volume);

    // The window's CRT filter; the setter applies and persists.
    bool get_crt_filter() const;
    void set_crt_filter(bool enabled);

    // Volumes live in <data_dir>/settings; no-ops while data_dir is unset.
    void load_settings();
    void save_settings();

  public:
    engine::Window* window;
    CResourceHolder* rh;

    // The game's bitmap font, shared by every state that renders text.
    engine::BitmapFont font;

    // Writable per-user directory (trailing separator included) for
    // saves like the high score; empty disables persistence.
    std::string data_dir;

    // The controller, when main wired one up (tests leave it null).
    engine::Gamepad* gamepad = nullptr;

    // The stage walls loaded from media/stages.
    CStageStore* stages = nullptr;

    // The backdrop, when main wired one up (tests may leave it null);
    // states pulse it on big moments.
    CStarfield* starfield = nullptr;

  private:
    // One dedicated voice per effect, indexed by its fx id.
    engine::Sound current_sound[game::game_fx::COUNT];

    engine::Music* music;
    std::string current_music;

    float music_volume = 30;
    float fx_volume = 10;
};
