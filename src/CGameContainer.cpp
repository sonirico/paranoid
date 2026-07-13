#include "CGameContainer.hpp"

#include "engine/AudioDevice.hpp"

#include <SDL3/SDL.h>

CGameContainer::CGameContainer(engine::Window* window, engine::AudioDevice* audio,
                               CResourceHolder* rh)
    : window(window), rh(rh),
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
    }
}

void CGameContainer::play_fx(game::game_fx::fx id)
{
    int i = static_cast<int>(id);

    this->current_sound[i].setVolume(10);
    this->current_sound[i].play();
}
