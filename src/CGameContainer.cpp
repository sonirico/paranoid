#include "CGameContainer.hpp"

#include "engine/AudioDevice.hpp"

#include <SDL3/SDL.h>

CGameContainer::CGameContainer(engine::Window* window, engine::AudioDevice* audio,
                               CResourceHolder* rh)
    : window(window), rh(rh), current_sound{engine::Sound(*audio), engine::Sound(*audio)}
{
    this->current_sound[0].setBuffer(this->rh->get(game::game_fx::REBOTE1));
    this->current_sound[1].setBuffer(this->rh->get(game::game_fx::REBOTE2));
}

void CGameContainer::events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
        {
            this->window->close();
            break;
        }
    }
}

void CGameContainer::play_fx(game::game_fx::fx id)
{
    int i = static_cast<int>(id);

    this->current_sound[i].setVolume(10);
    this->current_sound[i].play();
}
