#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "assets.h"
#include "engine/AudioDevice.hpp"
#include "engine/Clock.hpp"
#include "engine/Music.hpp"
#include "engine/Sprite.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <random>
#include <string>

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
    rh.load(MUERTE, "media/fx/muerte.wav");
    rh.load(OPTION, "media/fx/option.wav");
    rh.load(POINTS, "media/fx/points.wav");
    rh.load(LIFEUP, "media/fx/lifeup.wav");
    rh.load(STICKY, "media/fx/sticky.wav");
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

        engine::Music music(audio);

        if (music.loadFromFile("media/music/arkanoid.ogg"))
        {
            music.setVolume(30);
            music.play();
        }
        else
        {
            SDL_Log("Music failed to load, continuing without it");
        }

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

                if (!gc.is_paused())
                {
                    gsm.update(dt);
                }
            }

            music.update();

            // How far into the next physics tick this render frame falls;
            // entities interpolate their position with it.
            window.setFrameAlpha(smoke ? 1.f : time_since_last_update / game::TIME_PER_FRAME);

            window.clear();

            window.draw(background);
            gsm.render();

            if (gc.is_paused())
            {
                const std::string label = "PAUSA";
                const float scale = 4.f;
                const float text_width = label.size() * 8.f * scale;

                window.drawText({(game::WIDTH - text_width) / 2, (game::HEIGHT - 8.f * scale) / 2},
                                label, engine::Color::White, scale);
            }

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
