#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "CStageStore.hpp"
#include "CStarfield.hpp"
#include "assets.h"
#include "engine/AudioDevice.hpp"
#include "engine/Clock.hpp"
#include "engine/Gamepad.hpp"
#include "engine/Music.hpp"
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

    rh.load(MAIN, "media/textures/base.png");
    rh.load(BRICKS, "media/textures/bricks.png");
    rh.load(FONT, "media/textures/font.png");

    rh.load(REBOTE1, "media/fx/rebote1.wav");
    rh.load(REBOTE2, "media/fx/rebote2.wav");
    rh.load(MUERTE, "media/fx/muerte.wav");
    rh.load(OPTION, "media/fx/option.wav");
    rh.load(POINTS, "media/fx/points.wav");
    rh.load(LIFEUP, "media/fx/lifeup.wav");
    rh.load(STICKY, "media/fx/sticky.wav");
    rh.load(SELECT, "media/fx/select.wav");
    rh.load(CLING, "media/fx/cling.wav");
    rh.load(POP, "media/fx/pop.wav");
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

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
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

        CStageStore stages;

        if (!stages.load("media/stages"))
        {
            SDL_Log("No stages could be loaded from media/stages");
            SDL_Quit();
            return 1;
        }

        engine::Music music(audio);
        engine::Gamepad gamepad;

        CGameContainer gc(&window, &audio, &rh, &music);
        gc.gamepad = &gamepad;
        gc.stages = &stages;

        if (char* pref_path = SDL_GetPrefPath("", "paranoid"))
        {
            gc.data_dir = pref_path;
            SDL_free(pref_path);
        }

        // On unless the player saved it off; load_settings overrides.
        window.setCrtFilter(true);

        gc.load_settings();

        // Smoke runs skip the menu so the frames exercise real gameplay.
        CGameStateManager gsm(&gc, smoke ? game::game_states::PLAY : game::game_states::MENU);

        CStarfield starfield;
        gc.starfield = &starfield;

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
                gamepad.update();
                gsm.update(dt);
                starfield.update(dt);
            }

            music.update();

            // How far into the next physics tick this render frame falls;
            // entities interpolate their position with it.
            window.setFrameAlpha(smoke ? 1.f : time_since_last_update / game::TIME_PER_FRAME);

            window.clear();

            starfield.draw(window);
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
