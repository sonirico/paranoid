#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "assets.h"
#include "engine/AudioDevice.hpp"
#include "engine/Clock.hpp"
#include "engine/Sprite.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <random>

namespace
{
void load_resources(CResourceHolder& rh)
{
    using namespace game::game_textures;
    using namespace game::game_fx;

    rh.load(BACKGROUND, "media/textures/background.png");
    rh.load(MAIN, "media/textures/base.png");
    rh.load(BRICKS, "media/textures/bricks.png");

    rh.load(REBOTE1, "media/fx/rebote1.wav");
    rh.load(REBOTE2, "media/fx/rebote2.wav");
}
} // namespace

// Entry point. Passing --smoke runs a fixed number of headless frames
// and exits, so tests and valgrind can exercise the full game loop.
int main(int argc, char** argv)
{
    const bool smoke = argc > 1 && std::strcmp(argv[1], "--smoke") == 0;

    if (smoke)
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    std::srand(std::random_device{}());

    int exit_code = 0;

    // Scoped so everything is torn down before SDL_Quit.
    {
        engine::Window window(game::TITLE, game::WIDTH, game::HEIGHT);

        if (!window.isOpen())
        {
            SDL_Quit();
            return 1;
        }

        engine::AudioDevice audio;
        CResourceHolder rh(window.getRenderer());
        load_resources(rh);

        CGameContainer gc(&window, &audio, &rh);
        CGameStateManager gsm(&gc);

        engine::Sprite background;
        background.setTexture(rh.get(game::game_textures::BACKGROUND));

        engine::Clock clock;
        float time_since_last_update = 0;

        const float dt = game::TIME_PER_FRAME;
        unsigned int smoke_frames = game::FRAMES * 2;

        while (window.isOpen())
        {
            time_since_last_update += smoke ? dt : clock.restart();

            while (time_since_last_update > game::TIME_PER_FRAME)
            {
                time_since_last_update -= game::TIME_PER_FRAME;

                gc.events();
                gsm.update(dt);
            }

            window.clear();

            window.draw(background);
            gsm.render();

            window.display();

            if (smoke && --smoke_frames == 0)
            {
                window.close();
            }
        }
    }

    SDL_Quit();

    return exit_code;
}
