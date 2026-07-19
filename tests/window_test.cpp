#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

// Exercises the window's presentation settings against SDL's dummy
// video driver, so it runs headless.
class WindowTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

        window = std::make_unique<engine::Window>("test", 320, 240);
        ASSERT_TRUE(window->isOpen()) << SDL_GetError();
    }

    void TearDown() override
    {
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
};

TEST_F(WindowTest, StartsWindowedWithLetterboxScaling)
{
    EXPECT_FALSE(window->isFullscreen());
    EXPECT_EQ(window->getScaleMode(), engine::Window::ScaleMode::Letterbox);
}

TEST_F(WindowTest, ScaleModeCanBeSwitchedToStretchAndBack)
{
    window->setScaleMode(engine::Window::ScaleMode::Stretch);
    EXPECT_EQ(window->getScaleMode(), engine::Window::ScaleMode::Stretch);

    window->setScaleMode(engine::Window::ScaleMode::Letterbox);
    EXPECT_EQ(window->getScaleMode(), engine::Window::ScaleMode::Letterbox);
}

TEST_F(WindowTest, CrtFilterStartsDisabled)
{
    EXPECT_FALSE(window->isCrtFilter());
}

TEST_F(WindowTest, CrtFilterCanBeToggled)
{
    window->setCrtFilter(true);
    EXPECT_TRUE(window->isCrtFilter());

    window->setCrtFilter(false);
    EXPECT_FALSE(window->isCrtFilter());
}

TEST_F(WindowTest, CrtFilterRendersAFrame)
{
    window->setCrtFilter(true);
    ASSERT_TRUE(window->isCrtFilter());

    // Runs the offscreen scene through the warp compositor; SDL errors
    // would surface through the render calls' logs and a crash here.
    window->clear();
    window->drawRect({10.f, 10.f}, {50.f, 50.f}, engine::Color::White);
    window->display();
}
