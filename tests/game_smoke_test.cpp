#include "CBrick.hpp"
#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "engine/AudioDevice.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

// Boots the whole game headless (dummy video and audio drivers) and
// runs it for a while: the "does not crash, does not leak" contract
// that valgrind checks over the entire wiring.
class GameSmokeTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) << SDL_GetError();

        window = std::make_unique<engine::Window>(game::TITLE, game::WIDTH, game::HEIGHT);
        ASSERT_TRUE(window->isOpen());

        audio = std::make_unique<engine::AudioDevice>();

        holder = std::make_unique<CResourceHolder>(window->getRenderer());
        holder->load(game::game_textures::BACKGROUND, TEST_MEDIA_DIR "/textures/background.png");
        holder->load(game::game_textures::MAIN, TEST_MEDIA_DIR "/textures/base.png");
        holder->load(game::game_textures::BRICKS, TEST_MEDIA_DIR "/textures/bricks.png");
        holder->load(game::game_textures::FONT, TEST_MEDIA_DIR "/textures/font.png");
        holder->load(game::game_fx::REBOTE1, TEST_MEDIA_DIR "/fx/rebote1.wav");
        holder->load(game::game_fx::REBOTE2, TEST_MEDIA_DIR "/fx/rebote2.wav");
        holder->load(game::game_fx::MUERTE, TEST_MEDIA_DIR "/fx/muerte.wav");
        holder->load(game::game_fx::OPTION, TEST_MEDIA_DIR "/fx/option.wav");
        holder->load(game::game_fx::POINTS, TEST_MEDIA_DIR "/fx/points.wav");
        holder->load(game::game_fx::LIFEUP, TEST_MEDIA_DIR "/fx/lifeup.wav");
        holder->load(game::game_fx::STICKY, TEST_MEDIA_DIR "/fx/sticky.wav");
        holder->load(game::game_fx::SELECT, TEST_MEDIA_DIR "/fx/select.wav");

        container = std::make_unique<CGameContainer>(window.get(), audio.get(), holder.get());
    }

    void TearDown() override
    {
        container.reset();
        holder.reset();
        audio.reset();
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
    std::unique_ptr<engine::AudioDevice> audio;
    std::unique_ptr<CResourceHolder> holder;
    std::unique_ptr<CGameContainer> container;
};

TEST_F(GameSmokeTest, RunsPlayStateForTwoSecondsWithoutCrashing)
{
    CGameStateManager manager(container.get(), game::game_states::PLAY);

    const float dt = 1.f / game::FRAMES;

    for (unsigned int frame = 0; frame < game::FRAMES * 2; ++frame)
    {
        container->events();
        manager.update(dt);

        window->clear();
        manager.render();
        window->display();
    }
}

TEST_F(GameSmokeTest, RunsMenuStateForOneSecondWithoutCrashing)
{
    CGameStateManager manager(container.get(), game::game_states::MENU);

    const float dt = 1.f / game::FRAMES;

    for (unsigned int frame = 0; frame < game::FRAMES; ++frame)
    {
        container->events();
        manager.update(dt);

        window->clear();
        manager.render();
        window->display();
    }
}

TEST_F(GameSmokeTest, BrickLosesLifeAndBecomesRemovable)
{
    CGameStateManager manager(container.get(), game::game_states::PLAY);

    // Any CState works as the entity's context; build a brick through
    // the real wall-loading path instead of poking internals.
    CBrick brick(manager.getCurrentState(), game::game_bricks::RED);

    EXPECT_FALSE(brick.is_removable());

    brick.quit_life();

    EXPECT_TRUE(brick.is_removable());
}

TEST_F(GameSmokeTest, GoldBrickSurvivesTwoHits)
{
    CGameStateManager manager(container.get(), game::game_states::PLAY);
    CBrick brick(manager.getCurrentState(), game::game_bricks::GOLD);

    brick.quit_life();
    brick.quit_life();

    EXPECT_FALSE(brick.is_removable());

    brick.quit_life();

    EXPECT_TRUE(brick.is_removable());
}
