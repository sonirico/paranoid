#include "CGameContainer.hpp"

#include "engine/AudioDevice.hpp"
#include "engine/Music.hpp"

#include <SDL3/SDL.h>

CGameContainer::CGameContainer(engine::Window* window, engine::AudioDevice* audio,
                               CResourceHolder* rh, engine::Music* music)
    : window(window), rh(rh), music(music),
      current_sound{engine::Sound(*audio), engine::Sound(*audio), engine::Sound(*audio),
                    engine::Sound(*audio), engine::Sound(*audio), engine::Sound(*audio),
                    engine::Sound(*audio), engine::Sound(*audio)}
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

void CGameContainer::play_music(const std::string& path)
{
    if (this->music == nullptr || this->current_music == path)
    {
        return;
    }

    if (this->music->loadFromFile(path))
    {
        this->current_music = path;

        this->music->setVolume(30);
        this->music->play();
    }
    else
    {
        SDL_Log("Music %s failed to load, continuing without it", path.c_str());
    }
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

void CGameContainer::play_fx(game::game_fx::fx id)
{
    int i = static_cast<int>(id);

    this->current_sound[i].setVolume(10);
    this->current_sound[i].play();
}
