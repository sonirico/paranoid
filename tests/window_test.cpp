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
