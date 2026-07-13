#include "CGameContainer.hpp"
#include "CPlayState.hpp"
#include "CResourceHolder.hpp"
#include "engine/AudioDevice.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <cmath>
#include <gtest/gtest.h>

// Gameplay rules exercised headless against the real play state.
class PlayStateTest : public ::testing::Test
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
        holder->load(game::game_fx::REBOTE1, TEST_MEDIA_DIR "/fx/rebote1.wav");
        holder->load(game::game_fx::REBOTE2, TEST_MEDIA_DIR "/fx/rebote2.wav");

        container = std::make_unique<CGameContainer>(window.get(), audio.get(), holder.get());

        play_state = std::make_unique<CPlayState>(container.get());
        play_state->init();
    }

    void TearDown() override
    {
        play_state.reset();
        container.reset();
        holder.reset();
        audio.reset();
        window.reset();
        SDL_Quit();
    }

    // Drops every ball below the floor so the next update removes them.
    void loseAllBalls()
    {
        for (auto& ball : play_state->get_balls())
        {
            ball->set_in_paddle(false);
            ball->setPosition(game::WIDTH / 2.f, game::HEIGHT + 50.f);
        }

        // One update marks the balls removable, the next erases them.
        play_state->update(game::TIME_PER_FRAME);
        play_state->update(game::TIME_PER_FRAME);
    }

    std::unique_ptr<engine::Window> window;
    std::unique_ptr<engine::AudioDevice> audio;
    std::unique_ptr<CResourceHolder> holder;
    std::unique_ptr<CGameContainer> container;
    std::unique_ptr<CPlayState> play_state;
};

TEST_F(PlayStateTest, StartsWithThreeLivesAndOneBall)
{
    EXPECT_EQ(play_state->get_lives(), 3u);
    EXPECT_EQ(play_state->get_balls().size(), 1u);
}

TEST_F(PlayStateTest, LosingLastBallCostsALifeAndRespawnsBall)
{
    loseAllBalls();

    EXPECT_EQ(play_state->get_lives(), 2u);
    ASSERT_EQ(play_state->get_balls().size(), 1u);
    EXPECT_TRUE(play_state->get_balls().front()->is_in_paddle());
}

TEST_F(PlayStateTest, GameOverRestartsWithFullLivesOnFirstStage)
{
    loseAllBalls();
    loseAllBalls();
    loseAllBalls();

    EXPECT_EQ(play_state->get_lives(), 3u);
    EXPECT_EQ(CPlayState::get_current_stage(), 0u);
    ASSERT_EQ(play_state->get_balls().size(), 1u);
}

TEST_F(PlayStateTest, ExpandBonusWidensPaddle)
{
    const float before = play_state->get_paddle()->get_size().x;

    play_state->apply_bonus(game::game_bonus::E);

    EXPECT_GT(play_state->get_paddle()->get_size().x, before);
}

TEST_F(PlayStateTest, ShrinkBonusNarrowsPaddle)
{
    const float before = play_state->get_paddle()->get_size().x;

    play_state->apply_bonus(game::game_bonus::R);

    EXPECT_LT(play_state->get_paddle()->get_size().x, before);
}

TEST_F(PlayStateTest, StickyBonusMakesPaddleSticky)
{
    play_state->apply_bonus(game::game_bonus::C);

    EXPECT_TRUE(play_state->get_paddle()->is_sticky());
}

TEST_F(PlayStateTest, PaddleModesAreMutuallyExclusive)
{
    const float normal = play_state->get_paddle()->get_size().x;

    play_state->apply_bonus(game::game_bonus::E);
    play_state->apply_bonus(game::game_bonus::C);

    EXPECT_TRUE(play_state->get_paddle()->is_sticky());
    EXPECT_FLOAT_EQ(play_state->get_paddle()->get_size().x, normal);
}

TEST_F(PlayStateTest, MultiballBonusDoublesBalls)
{
    ASSERT_EQ(play_state->get_balls().size(), 1u);

    play_state->apply_bonus(game::game_bonus::D);

    EXPECT_EQ(play_state->get_balls().size(), 2u);
}

TEST_F(PlayStateTest, SlowBonusReducesBallSpeed)
{
    const float before = play_state->get_balls().front()->get_velocity().x;

    play_state->apply_bonus(game::game_bonus::S);

    EXPECT_LT(std::abs(play_state->get_balls().front()->get_velocity().x), std::abs(before));
}

TEST_F(PlayStateTest, SpeedBonusIncreasesBallSpeed)
{
    const float before = play_state->get_balls().front()->get_velocity().x;

    play_state->apply_bonus(game::game_bonus::P);

    EXPECT_GT(std::abs(play_state->get_balls().front()->get_velocity().x), std::abs(before));
}

TEST_F(PlayStateTest, ExtraLifeBonusAddsALife)
{
    play_state->apply_bonus(game::game_bonus::X);

    EXPECT_EQ(play_state->get_lives(), 4u);
}

TEST_F(PlayStateTest, LaserBonusArmsPaddleAndFiresPairs)
{
    play_state->apply_bonus(game::game_bonus::L);
    ASSERT_TRUE(play_state->get_paddle()->has_laser());

    play_state->fire_lasers();

    EXPECT_EQ(play_state->get_lasers().size(), 2u);
}

TEST_F(PlayStateTest, LasersFlyUpAndExpireOffScreen)
{
    play_state->apply_bonus(game::game_bonus::L);
    play_state->fire_lasers();

    // More than enough frames to cross the whole screen.
    for (unsigned int i = 0; i < game::FRAMES * 3; ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    EXPECT_TRUE(play_state->get_lasers().empty());
}
