#include "CGameContainer.hpp"

#include "engine/AudioDevice.hpp"
#include "engine/Music.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <fstream>

CGameContainer::CGameContainer(engine::Window* window, engine::AudioDevice* audio,
                               CResourceHolder* rh, engine::Music* music)
    : window(window), rh(rh), music(music),
      current_sound{engine::Sound(*audio), engine::Sound(*audio), engine::Sound(*audio),
                    engine::Sound(*audio), engine::Sound(*audio), engine::Sound(*audio),
                    engine::Sound(*audio), engine::Sound(*audio), engine::Sound(*audio)}
{
    for (int i = 0; i < game::game_fx::COUNT; ++i)
    {
        this->current_sound[i].setBuffer(this->rh->get(static_cast<game::game_fx::fx>(i)));
    }

    this->font.setTexture(this->rh->get(game::game_textures::FONT));
}

void CGameContainer::events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            this->window->close();
            break;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F11 && !event.key.repeat)
        {
            this->window->setFullscreen(!this->window->isFullscreen());
        }
    }
}

void CGameContainer::play_music(const std::string& path, bool looping)
{
    if (this->music == nullptr || this->current_music == path)
    {
        return;
    }

    if (this->music->loadFromFile(path))
    {
        this->current_music = path;

        this->music->setLooping(looping);
        this->music->setVolume(this->music_volume);
        this->music->play();
    }
    else
    {
        SDL_Log("Music %s failed to load, continuing without it", path.c_str());
    }
}

float CGameContainer::get_music_duration() const
{
    return this->music != nullptr ? this->music->getDuration() : 0.f;
}

void CGameContainer::stop_music()
{
    if (this->music == nullptr)
    {
        return;
    }

    this->music->stop();
    this->current_music.clear();
}

void CGameContainer::play_fx(game::game_fx::fx id, float pitch)
{
    int i = static_cast<int>(id);

    this->current_sound[i].setVolume(this->fx_volume);
    this->current_sound[i].setPitch(pitch);
    this->current_sound[i].play();
}

float CGameContainer::get_music_volume() const
{
    return this->music_volume;
}

void CGameContainer::set_music_volume(float volume)
{
    this->music_volume = std::clamp(volume, 0.f, 100.f);

    if (this->music != nullptr)
    {
        this->music->setVolume(this->music_volume);
    }

    this->save_settings();
}

float CGameContainer::get_fx_volume() const
{
    return this->fx_volume;
}

void CGameContainer::set_fx_volume(float volume)
{
    this->fx_volume = std::clamp(volume, 0.f, 100.f);

    this->save_settings();
}

void CGameContainer::load_settings()
{
    if (this->data_dir.empty())
    {
        return;
    }

    std::ifstream file(this->data_dir + "settings");

    float music_vol = this->music_volume;
    float fx_vol = this->fx_volume;

    if (file >> music_vol >> fx_vol)
    {
        this->music_volume = std::clamp(music_vol, 0.f, 100.f);
        this->fx_volume = std::clamp(fx_vol, 0.f, 100.f);
    }
}

void CGameContainer::save_settings()
{
    if (this->data_dir.empty())
    {
        return;
    }

    std::ofstream file(this->data_dir + "settings");

    if (file)
    {
        file << this->music_volume << ' ' << this->fx_volume;
    }
}
