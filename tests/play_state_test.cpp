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

    // Replaces the stage's wall with a single brick at the given position.
    CBrick* placeSingleBrick(const engine::Vec2f& pos, game::game_bricks::bricks type)
    {
        auto& bricks = play_state->get_bricks();
        bricks.clear();

        auto brick = std::make_unique<CBrick>(play_state.get(), type);
        brick->setPosition(pos);
        bricks.push_back(std::move(brick));

        return bricks.back().get();
    }

    // Puts the first ball in flight at the given position and velocity.
    CBall* launchBall(const engine::Vec2f& pos, const engine::Vec2f& vel)
    {
        CBall* ball = play_state->get_balls().front().get();
        ball->set_in_paddle(false);
        ball->setPosition(pos);
        ball->set_velocity(vel);

        return ball;
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

TEST_F(PlayStateTest, SpinBonusArmsPaddleExclusively)
{
    play_state->apply_bonus(game::game_bonus::T);

    EXPECT_TRUE(play_state->get_paddle()->has_spin());

    play_state->apply_bonus(game::game_bonus::C);

    EXPECT_FALSE(play_state->get_paddle()->has_spin());
    EXPECT_TRUE(play_state->get_paddle()->is_sticky());
}

TEST_F(PlayStateTest, ModeBonusesTrackTheActiveBonus)
{
    EXPECT_EQ(play_state->get_active_bonus(), game::game_bonus::COUNT);

    play_state->apply_bonus(game::game_bonus::L);

    EXPECT_EQ(play_state->get_active_bonus(), game::game_bonus::L);
}

TEST_F(PlayStateTest, ActiveBonusExpiresAfterItsTimeout)
{
    play_state->apply_bonus(game::game_bonus::L);
    ASSERT_TRUE(play_state->get_paddle()->has_laser());

    const int ticks = static_cast<int>(CPlayState::BONUS_DURATION / game::TIME_PER_FRAME) + 1;

    for (int i = 0; i < ticks; ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    EXPECT_FALSE(play_state->get_paddle()->has_laser());
    EXPECT_EQ(play_state->get_active_bonus(), game::game_bonus::COUNT);
}

TEST_F(PlayStateTest, AnyCapsuleCancelsTheActiveBonus)
{
    play_state->apply_bonus(game::game_bonus::L);
    ASSERT_TRUE(play_state->get_paddle()->has_laser());

    play_state->apply_bonus(game::game_bonus::S);

    EXPECT_FALSE(play_state->get_paddle()->has_laser());
    EXPECT_EQ(play_state->get_active_bonus(), game::game_bonus::COUNT);
}

TEST_F(PlayStateTest, StickyPaddleCatchesBallWhereItLanded)
{
    play_state->apply_bonus(game::game_bonus::C);

    CBall* ball = play_state->get_balls().front().get();
    CPaddle* paddle = play_state->get_paddle();

    // Drop the ball onto the paddle, off-centre.
    ball->set_in_paddle(false);
    ball->setPosition(paddle->getPosition().x + 10.f, paddle->getPosition().y - 40.f);
    ball->set_velocity({0.f, 300.f});

    for (int i = 0; i < 60 && !ball->is_in_paddle(); ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    ASSERT_TRUE(ball->is_in_paddle());
    EXPECT_NEAR(ball->getPosition().x - paddle->getPosition().x, 10.f, 1.f);
}

TEST_F(PlayStateTest, StickyPaddleStoresUpwardsReleaseVelocity)
{
    play_state->apply_bonus(game::game_bonus::C);

    CBall* ball = play_state->get_balls().front().get();
    CPaddle* paddle = play_state->get_paddle();

    // Drop the ball onto the paddle from above.
    ball->set_in_paddle(false);
    ball->setPosition(paddle->getPosition().x + 10.f, paddle->getPosition().y - 40.f);
    ball->set_velocity({0.f, 300.f});

    for (int i = 0; i < 60 && !ball->is_in_paddle(); ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    ASSERT_TRUE(ball->is_in_paddle());
    EXPECT_LT(ball->get_velocity().y, 0.f);
}

TEST_F(PlayStateTest, BallPastPaddleTopIsBeyondSaving)
{
    CBall* ball = play_state->get_balls().front().get();
    CPaddle* paddle = play_state->get_paddle();

    // Already deeper than the save grace, overlapping the paddle from
    // the side: it must fall through with no side bounce.
    ball->set_in_paddle(false);
    ball->setPosition(paddle->getPosition().x + 10.f, paddle->getPosition().y + 4.f);
    ball->set_velocity({50.f, 200.f});

    play_state->update(game::TIME_PER_FRAME);

    EXPECT_FALSE(ball->is_in_paddle());
    EXPECT_GT(ball->get_velocity().x, 0.f);
    EXPECT_GT(ball->get_velocity().y, 0.f);
}

TEST_F(PlayStateTest, FastBallNeverTunnelsThroughThePaddle)
{
    const engine::Vec2f p_pos = play_state->get_paddle()->getPosition();

    // Fast enough to cross the whole paddle within a single tick.
    CBall* ball = launchBall({p_pos.x + 20.f, p_pos.y - 40.f}, {0.f, 5000.f});

    play_state->update(game::TIME_PER_FRAME);

    EXPECT_LT(ball->get_velocity().y, 0.f);
}

TEST_F(PlayStateTest, LateSaveWithinTheGraceStillBounces)
{
    const engine::Vec2f p_pos = play_state->get_paddle()->getPosition();

    // The ball's bottom already dipped 4 px past the paddle's top edge —
    // within the save grace, as when the paddle slides under a low ball
    // at the last instant.
    CBall* ball = launchBall({p_pos.x + 10.f, p_pos.y - 4.f}, {0.f, 200.f});

    play_state->update(game::TIME_PER_FRAME);

    EXPECT_LT(ball->get_velocity().y, 0.f);
}

TEST_F(PlayStateTest, BrickCornerGrazeReflectsOnlyTheStruckAxis)
{
    // The brick is 32x16 on screen; the ball, 10x8.
    placeSingleBrick({200.f, 100.f}, game::game_bricks::RED);

    // Falling from the upper left, the ball clips the brick's lower-left
    // corner: it crosses the left face, so only X may mirror.
    CBall* ball = launchBall({185.f, 106.f}, {400.f, 400.f});

    play_state->update(game::TIME_PER_FRAME);

    EXPECT_LT(ball->get_velocity().x, 0.f);
    EXPECT_GT(ball->get_velocity().y, 0.f);
}

TEST_F(PlayStateTest, HeadOnBrickHitReflectsVertically)
{
    placeSingleBrick({200.f, 100.f}, game::game_bricks::RED);

    // Straight down onto the middle of the brick's top face.
    CBall* ball = launchBall({211.f, 80.f}, {0.f, 300.f});

    for (int i = 0; i < 5; ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    EXPECT_LT(ball->get_velocity().y, 0.f);
    EXPECT_FLOAT_EQ(ball->get_velocity().x, 0.f);
}

TEST_F(PlayStateTest, FastBallNeverTunnelsThroughABrick)
{
    placeSingleBrick({200.f, 100.f}, game::game_bricks::RED);

    // Fast enough to cross the whole brick within a single tick.
    CBall* ball = launchBall({211.f, 80.f}, {0.f, 5000.f});

    play_state->update(game::TIME_PER_FRAME);

    EXPECT_LT(ball->get_velocity().y, 0.f);
    EXPECT_TRUE(play_state->get_bricks().empty());
}

TEST_F(PlayStateTest, BrickHitConsumesExactlyOneLife)
{
    // Silver bricks take two hits: one bounce must leave it standing.
    placeSingleBrick({200.f, 100.f}, game::game_bricks::SILVER);

    launchBall({211.f, 80.f}, {0.f, 300.f});

    for (int i = 0; i < 5; ++i)
    {
        play_state->update(game::TIME_PER_FRAME);
    }

    EXPECT_EQ(play_state->get_bricks().size(), 1u);
}

TEST_F(PlayStateTest, MultiballBonusSplitsBallInThree)
{
    ASSERT_EQ(play_state->get_balls().size(), 1u);

    play_state->apply_bonus(game::game_bonus::D);

    ASSERT_EQ(play_state->get_balls().size(), 3u);

    // The split releases the parked ball and every ball keeps its speed.
    const float speed = play_state->get_balls().front()->get_velocity().length();

    for (auto& ball : play_state->get_balls())
    {
        EXPECT_FALSE(ball->is_in_paddle());
        EXPECT_NEAR(ball->get_velocity().length(), speed, 0.5f);
    }
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
