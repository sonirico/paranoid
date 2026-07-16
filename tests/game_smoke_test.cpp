#include "CBrick.hpp"
#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "CStageStore.hpp"
#include "engine/AudioDevice.hpp"
#include "engine/Music.hpp"
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
        holder->load(game::game_fx::CLING, TEST_MEDIA_DIR "/fx/cling.wav");

        music = std::make_unique<engine::Music>(*audio);

        stages = std::make_unique<CStageStore>();
        ASSERT_TRUE(stages->load(TEST_MEDIA_DIR "/stages"));

        container =
            std::make_unique<CGameContainer>(window.get(), audio.get(), holder.get(), music.get());
        container->stages = stages.get();
    }

    void TearDown() override
    {
        container.reset();
        music.reset();
        holder.reset();
        audio.reset();
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
    std::unique_ptr<engine::AudioDevice> audio;
    std::unique_ptr<engine::Music> music;
    std::unique_ptr<CStageStore> stages;
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

TEST_F(GameSmokeTest, RunsMapEditorForOneSecondWithoutCrashing)
{
    CGameStateManager manager(container.get(), game::game_states::MAP);

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

TEST_F(GameSmokeTest, HitBrickFlashesBriefly)
{
    CGameStateManager manager(container.get(), game::game_states::PLAY);
    CBrick brick(manager.getCurrentState(), game::game_bricks::SILVER);

    EXPECT_FALSE(brick.is_flashing());

    brick.quit_life();

    EXPECT_TRUE(brick.is_flashing());

    brick.update(0.2f);

    EXPECT_FALSE(brick.is_flashing());
}

TEST_F(GameSmokeTest, UndestroyableBrickShrugsOffHits)
{
    CGameStateManager manager(container.get(), game::game_states::PLAY);
    CBrick brick(manager.getCurrentState(), game::game_bricks::UNDESTROYABLE);

    brick.quit_life();
    brick.quit_life();

    EXPECT_FALSE(brick.is_removable());
}

TEST_F(GameSmokeTest, VolumeSettingsClampAndPersist)
{
    container->data_dir = ::testing::TempDir();

    container->set_music_volume(150.f);
    container->set_fx_volume(-10.f);

    EXPECT_FLOAT_EQ(container->get_music_volume(), 100.f);
    EXPECT_FLOAT_EQ(container->get_fx_volume(), 0.f);

    container->set_music_volume(40.f);
    container->set_fx_volume(20.f);

    // A fresh container pointed at the same data_dir reads them back.
    CGameContainer other(window.get(), audio.get(), holder.get(), music.get());
    other.data_dir = container->data_dir;
    other.load_settings();

    EXPECT_FLOAT_EQ(other.get_music_volume(), 40.f);
    EXPECT_FLOAT_EQ(other.get_fx_volume(), 20.f);
}
