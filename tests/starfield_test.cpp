#include "CStarfield.hpp"
#include "assets.h"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>
#include <memory>

// The procedural backdrop, exercised headless against the dummy
// video driver.
class StarfieldTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

        window = std::make_unique<engine::Window>(game::TITLE, game::WIDTH, game::HEIGHT);
        ASSERT_TRUE(window->isOpen()) << SDL_GetError();
    }

    void TearDown() override
    {
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
};

TEST_F(StarfieldTest, GeneratesItsStars)
{
    CStarfield field;

    EXPECT_GT(field.get_star_count(), 0u);
}

TEST_F(StarfieldTest, ShootingStarEventuallyCrossesTheSky)
{
    CStarfield field;

    // The first cooldown arms at eight seconds: ten must catch one.
    bool seen = false;

    for (unsigned int i = 0; i < game::FRAMES * 10 && !seen; ++i)
    {
        field.update(game::TIME_PER_FRAME);
        seen = field.has_shooting_star();
    }

    EXPECT_TRUE(seen);
}

TEST_F(StarfieldTest, PulseBrightensThenDecaysOnItsOwn)
{
    CStarfield field;

    EXPECT_FLOAT_EQ(field.get_pulse(), 0.f);

    field.pulse(1.f);

    EXPECT_GT(field.get_pulse(), 0.9f);

    // The decay is exponential: a few seconds leave next to nothing.
    for (unsigned int i = 0; i < game::FRAMES * 3; ++i)
    {
        field.update(game::TIME_PER_FRAME);
    }

    EXPECT_LT(field.get_pulse(), 0.05f);
}

TEST_F(StarfieldTest, DriftsAndDrawsAfterALongRun)
{
    CStarfield field;

    // Half a minute of drift: every star wrapped at least once.
    for (unsigned int i = 0; i < game::FRAMES * 30; ++i)
    {
        field.update(game::TIME_PER_FRAME);
    }

    window->clear();
    field.draw(*window);
    window->display();

    EXPECT_GT(field.get_star_count(), 0u);
}
